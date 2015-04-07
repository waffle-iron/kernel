/*
 *      This file is part of the Smoke project.
 *
 *  Copyright of this program is the property of its author(s), without
 *  those written permission reproduction in whole or in part is prohibited.
 *  More details on the LICENSE file delivered with the project.
 *
 *   - - - - - - - - - - - - - - -
 *
 *      Methods used to handle default temporary file system (tmpfs).
 */
#include <kernel/vfs.h>
#include <kernel/info.h>


// ===========================================================================
// ---------------------------------------------------------------------------
int TMPFS_Read(kInode_t *fp, void *buffer, size_t count, size_t lba)
{
  memset (buffer, 0, count * PAGE_SIZE);
  return 0;
}


// ---------------------------------------------------------------------------
int TMPFS_Create (const char *name, kInode_t *dir, kStat_t *file)
{
  time_t now = time(NULL);
  file->atime_ = now;
  file->mtime_ = now;
  file->ctime_ = now;
  // file->dblock_ = 1;
  file->block_ = PAGE_SIZE;
  return 0;
}

// ===========================================================================
/** Initialize the VFS kernel module
  * Create the root node and start device detection
  * @note Requests a list of device drivers.
  */
int initialize_vfs()
{
  time_t now = time(NULL);
  kSYS.rootNd_ = KALLOC (kInode_t);
  kDevice_t *tmpFs = KALLOC (kDevice_t);

  kSYS.rootNd_->stat_.ino_ = 1;
  kSYS.rootNd_->stat_.mode_ = S_IFDIR | 0755;
  kSYS.rootNd_->stat_.atime_ = now;
  kSYS.rootNd_->stat_.mtime_ = now;
  kSYS.rootNd_->stat_.ctime_ = now;
  kSYS.rootNd_->stat_.block_ = PAGE_SIZE;
  kSYS.rootNd_->dev_ = tmpFs;

  tmpFs->read = TMPFS_Read;
  tmpFs->create = TMPFS_Create;

  kSYS.devNd_ = create_inode(FS_DEV_NODE, kSYS.rootNd_, S_IFDIR | 0755, 0);
  kSYS.mntNd_ = create_inode(FS_MNT_NODE, kSYS.rootNd_, S_IFDIR | 0755, 0);
  kSYS.pipeNd_ = create_inode(FS_PIPE_NODE, kSYS.devNd_, S_IFDIR | 0755, 0);

  return __noerror();
}