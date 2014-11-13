/*
 *      This file is part of the Smoke project.
 *
 *  Copyright of this program is the property of its author(s), without
 *  those written permission reproduction in whole or in part is prohibited.
 *  More details on the LICENSE file delivered with the project.
 *
 *   - - - - - - - - - - - - - - -
 *
 *      Routines to manipulate the nodes tree.
 */
#include <kernel/inodes.h>
#include <kernel/info.h>
#include <kernel/params.h>

// ===========================================================================
/**
 * Attach a new inode to the file system hierarchie.
 *   EEXIST the method found an item with the same name
 * @note The inode are attach in alphabetic order.
 */
static int kfs_attach (kInode_t* ino, kInode_t* top, const char* name)
{
  int k;
  kInode_t* cursor = top->child_;
  assert (kislocked (&top->lock_));
  assert (kislocked (&ino->lock_));
  ino->parent_ = top;

  if (top->child_ == NULL) {
    top->child_ = ino;
    return __noerror ();
  }

  if (0 < strcmp(cursor->name_, name)) {
    ino->next_ = top->child_;
    cursor->prev_ = ino;
    top->child_ = ino;
    return __noerror ();
  }

  for (;;) {
    if (cursor->next_ == NULL) {
      ino->prev_ = cursor;
      cursor->next_ = ino;
      return __noerror ();
    }

    k = strcmp(cursor->next_->name_, name);

    if (k < 0 && cursor->next_ != NULL) {
      cursor = cursor->next_;
      continue;
    }

    if (k == 0) {
      return __seterrno(EEXIST);
    }

    ino->prev_ = cursor;
    ino->next_ = cursor->next_;
    cursor->next_->prev_ = ino;
    cursor->next_ = ino;
    return __noerror ();
  }
}


// ---------------------------------------------------------------------------
/** */
static kInode_t* kfs_stat (const char* name, kInode_t* dir)
{
  if (!dir->dev_->lookup) {
    kunlock(&dir->lock_);
    __seterrno (ENOENT);
    return NULL;
  }

  // kprintf ("FS] REQ FS LookChild %s \n", name);
  // Request the file system
  kStat_t stat = { 0 };
  MODULE_ENTER(&dir->lock_, &dir->dev_->lock_);
  int err = dir->dev_->lookup (name, dir, &stat);
  MODULE_LEAVE(&dir->lock_, &dir->dev_->lock_);
  if (err) {
    kunlock(&dir->lock_);
    __seterrno (err);
    return NULL;
  }

  // kprintf ("FS] REQ FS FIND Child %s \n", name);
  return kfs_register (name, dir, &stat);
}


// ===========================================================================
/** */
kInode_t* kfs_lookup(const char* path, kInode_t* dir)
{
  int k;
  int symLinkLoop = 0;
  char uri [PATH_MAX];
  char* name;
  char* rentTok;
  kInode_t* ino;

  NO_LOCK;

  // IF VOLUME LOOK FOR VOLUME
  if (strrchr (path, VOLUME_SEPARATOR)) {
    // FIXME read volume
  }

  // IF NO DIR OR / START WITH ROOT
  if (dir == NULL || path[0] == '/' || path[0] == '\\')
    dir = kSYS.rootNd_;

  // Look the first node file
  strncpy (uri, path, PATH_MAX);
  name = strtok_r (uri, FILENAME_SEPARATORS, &rentTok);
  klock (&dir->lock_);
  while (name != NULL) {
    assert (kislocked(&dir->lock_));

    // Follow symlink
    if (S_ISLNK(dir->stat_.mode_)) {
      kfs_follow_link(&dir, &symLinkLoop);
      if (__geterrno()) {
        return NULL;
      }

      if (symLinkLoop > MAX_SYMLINK_LOOP) {
        kunlock (&dir->lock_);
        __seterrno (ELOOP);
        return NULL;
      }
    }

    // Is a directory
    if (!S_ISDIR (dir->stat_.mode_)) {
      kunlock (&dir->lock_);
      __seterrno(ENOTDIR);
      return NULL;
    }

    // If no child, request the file system
    if (dir->child_ == NULL) {
      // 
      dir = kfs_stat (name, dir);
      if (dir == NULL)  {
        NO_LOCK;
        return NULL;
      }
      // kprintf ("FS] Request %s, get %s\n", name, dir->name_);

    } else {
      ino = dir->child_;
      for (;;) {

        k = strcmp(ino->name_, name);
        if (k != 0) {
          // kprintf ("FS] Browse %s - %s, get %x\n", ino->name_, name, ino->next_);
          if (ino->next_ != NULL) {
            ino = ino->next_;
            continue;

          } else {
            // kprintf ("FS] Browse LOOK %s - %s, get %x\n", ino->name_, name, ino->next_);
            assert (klockcount() == 1);
            assert (kislocked(&dir->lock_));
            dir = kfs_stat (name, dir);
            if (dir == NULL) {
              NO_LOCK;
              return NULL;
            }
            // kprintf ("FS] Browse FIND %s - %s\n", ino->name_, name);
            break;
          }
        }

        klock (&ino->lock_, LOCK_FS_LOOK);
        kunlock (&dir->lock_);
        dir = ino;
        break;
      }
    }

    name = strtok_r (NULL, FILENAME_SEPARATORS, &rentTok);
  }

  kunlock (&dir->lock_);
  NO_LOCK;
  return dir;
}


// ---------------------------------------------------------------------------
/** Try to add a new inode on the VFS tree
 * The returned inode is still locked
 */
kInode_t* kfs_register(const char* name, kInode_t* dir, kStat_t* stat)
{
  assert (kislocked(&dir->lock_));

  kInode_t* ino;
  __noerror ();

  if (name == NULL || dir == NULL || stat == NULL) {
    __seterrno (EINVAL);
    return NULL;
  }

  if ( !S_ISDIR (dir->stat_.mode_)) {
    __seterrno(ENOTDIR);
    return NULL;
  }

  ino = (kInode_t*) kalloc(sizeof(kInode_t));
  ino->name_ = kcopystr(name);
  ino->dev_ = dir->dev_;
  ino->devinfo_ = dir->devinfo_;
  memcpy (&ino->stat_, stat, sizeof(kStat_t));
  ino->stat_.ino_ = kSys_NewIno();
  ino->stat_.mode_ &= (S_IALLUGO | S_IFMT);
  // klock(&dir->lock_, LOCK_FS_REGISTER);
  klock(&ino->lock_, LOCK_FS_REGISTER);
  if (kfs_attach (ino, dir, name)) {
    kunlock (&dir->lock_);
    kunlock (&ino->lock_);
    kfree(ino);
    return NULL;
  }

  kunlock (&dir->lock_);
  return ino;
}


// ---------------------------------------------------------------------------
/** */
int kfs_unregister(kInode_t* ino)
{
  return __seterrno (ENOSYS);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
