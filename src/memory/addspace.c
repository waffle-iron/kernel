/*
 *      This file is part of the Smoke project.
 *
 *  Copyright of this program is the property of its author(s), without
 *  those written permission reproduction in whole or in part is prohibited.
 *  More details on the LICENSE file delivered with the project.
 *
 *   - - - - - - - - - - - - - - -
 *
 *      Initialize Memory address space
 */
#include <kernel/memory.h>
#include <kernel/vfs.h>
#include <kernel/info.h>


// ===========================================================================
/** Initialize memory addressing
 */
void kvma_init ()
{
  return;
}


// ---------------------------------------------------------------------------
/** Initialize a new address space structure with a first user-stack */
kAddSpace_t* kvma_new (size_t peb_size)
{
  kAddSpace_t* addsp = (kAddSpace_t*)kalloc(sizeof(kAddSpace_t));
  peb_size = ALIGN_UP(peb_size, PAGE_SIZE);
  addsp->first_ = (kVma_t*)kalloc(sizeof(kVma_t));
  addsp->last_ = addsp->first_;
  addsp->first_->limit_ = kHDW.userSpaceLimit_;
  addsp->first_->base_ = kHDW.userSpaceLimit_ - peb_size;
  addsp->first_->flags_ = VMA_READ | VMA_WRITE;
  addsp->vrtPages_ += peb_size / PAGE_SIZE;
  return addsp;
}


// ---------------------------------------------------------------------------
/** Initialize a new address space structure from an already existing one */
kAddSpace_t* kvma_clone (kAddSpace_t* addp)
{
  kAddSpace_t* addsp = (kAddSpace_t*)kalloc(sizeof(kAddSpace_t));
  kVma_t* md = addp->first_;

  while (md) {
    if (addsp->last_) {
      addsp->last_->next_ = (kVma_t*)kalloc(sizeof(kVma_t));
      addsp->last_->next_->prev_ = addsp->last_;
      addsp->last_ = addsp->last_->next_;

    } else {
      addsp->first_ = (kVma_t*)kalloc(sizeof(kVma_t));
      addsp->last_ = addsp->first_;
    }

    addsp->last_->base_ = md->base_;
    addsp->last_->limit_ = md->limit_;
    addsp->last_->flags_ = md->flags_;
    if (md->ino_) {
      kfs_grab (md->ino_);
      addsp->last_->ino_ = md->ino_;
      addsp->last_->offset_ = md->offset_;
    }

    md = md->next_;
  }

  addsp->vrtPages_ = addp->vrtPages_;
  return addsp;
}


// ---------------------------------------------------------------------------
/** Free all memory area inforation */
int kvma_destroy (kAddSpace_t* addp)
{
  kVma_t* nx = addp->first_;
  kVma_t* md = addp->first_;

  while (md) {
    nx = md->next_;

    if (md->ino_)
      kfs_release (md->ino_);

    kfree(md);
    md = nx;
  }

  kfree(addp);
  return __noerror();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------