#include <kernel/task.h>
#include <kernel/vfs.h>
#include <kernel/memory.h>

extern kAssembly_t* elf_open (kInode_t* ino);

// ---------------------------------------------------------------------------
/** Read an image file and create the corresponding assembly.
 */
kAssembly_t* load_assembly (kInode_t* ino)
{
  assert (S_ISREG (ino->stat_.mode_));

  if (ino->assembly_ == NULL) {
    kAssembly_t* image;
    if (inode_open(ino))
      return NULL;

    image = elf_open (ino);
    if (image == NULL) {
      int err = __geterrno();
      inode_close (ino);
      __seterrno(err);
      return NULL;
    }

    image->ino_ = ino;
    ino->assembly_ = image;
  }

  return ino->assembly_;
}

// ---------------------------------------------------------------------------
void destroy_assembly (kAssembly_t* image)
{
  kSection_t* section = klist_pop(&image->sections_, kSection_t, node_);
  while (section) {
    kSection_t* pick = section;
    section = klist_pop(&image->sections_, kSection_t, node_);
    kfree (pick);
  }

  if (image->ino_)
    inode_close (image->ino_);
  kfree(image);
}


// ---------------------------------------------------------------------------
/** Load an assembly image on an address space.
 *  Basically, it place the correct virtual memory area at the right addresses.
 *  \note A nice feature will be to compute offset for relocation.
 */
int map_assembly (kAddSpace_t* mspace, kAssembly_t* image)
{
  kSection_t* section;
  for_each (section, &image->sections_, kSection_t, node_) {
    assert ((section->offset_ & (PAGE_SIZE -1)) == 0);
    vmarea_map_section (mspace, section, image->ino_);
  }

  return __noerror();
}




