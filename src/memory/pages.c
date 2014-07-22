#include <memory.h>
#include <scheduler.h>
#include <inodes.h>
#include <kinfo.h>

/** 
  Table   xxxx----HDA--UWP b

  P: Is the page present in memory
  W: Is the page writable
  U: Is the page accessible in user space
  A: Read operation happends
  D: Write operation happends
  H: Huge page

 */
#define PG_KERNEL_ONLY    3
#define PG_USER_RDWR      7
#define PG_USER_RDONLY    5

#define TABLE_DIR_THR ((uint32_t *)0xfffff000)
#define TABLE_DIR_PRC ((uint32_t *)0xffffe000)
#define TABLE_DIR_KRN ((uint32_t *)0xffffd000)
#define TABLE_DIR_WIN ((uint32_t *)0xffffc000)

#define TABLE_PAGE_TH(s) ((uint32_t *)(0xffc00000 | ((s) << 12)))


// ---------------------------------------------------------------------------
void kPg_DumpTable (uint32_t *table)
{
  int i;
  for (i=0; i<1024; ++i) {
    if ((i % 128) == 0)
      kTty_Putc ('\n');
    if (table[i] == 0)
      kTty_Putc ('.');
    else 
      kTty_Putc ((table[i] & 0x7) + '0');


    // else if ((table[i] & 0x7) == 7)
    //   kTty_Putc ('U');
    // else if ((table[i] & 0x7) == 3)
    //   kTty_Putc ('K');
    // else if ((table[i] & 0x7) == 5)
    //   kTty_Putc ('R');
    // else
    //   kTty_Putc ('?');
  }  
  
  kTty_Putc ('\n');
}


// ---------------------------------------------------------------------------
/** Resolve a soft page fault by allocating a new page in designed context */
void kPg_Resolve (uint32_t address, uint32_t *table, int rights, int dirRight)
{
  int dir = (address >> 22) & 0x3ff;
  int tbl = (address >> 12) & 0x3ff;

  if (KLOG_PF) kprintf ("PF] Resolve at %x with %d,%d <%d,%d>\n", address, rights, dirRight, dir, tbl);
  if (table[dir] == 0) {
    uint32_t page = kPg_AllocPage();
    table[dir] = page | dirRight;

    if (table != TABLE_DIR_THR) {
      TABLE_DIR_THR[dir] = table[dir] | dirRight;
    }

    memset (TABLE_PAGE_TH(dir), 0, PAGE_SIZE);
  }

  if (TABLE_PAGE_TH(dir)[tbl] == 0) {
    TABLE_PAGE_TH(dir)[tbl] = kPg_AllocPage() | rights;
    memset ((void*)(address & ~(PAGE_SIZE-1)), 0, PAGE_SIZE);
  }
}


// ---------------------------------------------------------------------------
/** Resolve a soft page fault by allocating a new page in designed context */
void kPg_FillStream (kVma_t* vma, uint32_t address, int rights)
{
  // uint32_t address = vma->base_;
  // size_t lg = vma->limit_ - vma->base_;
  // for (;address < vma->limit_; address += PAGE_SIZE)

  size_t lg = PAGE_SIZE / vma->ino_->stat_.cblock_;
  size_t off = (vma->offset_ + (address - vma->base_)) / vma->ino_->stat_.cblock_;
  kPg_Resolve (address, TABLE_DIR_PRC, rights, PG_USER_RDWR);
  kFs_Read (vma->ino_, (void*)address, lg, off);
  // kTty_HexDump (address, 0x40);
 if (KLOG_PF) kprintf ("PF] fill stream at <%x> \n", address);

  //for (;;);
}


// ---------------------------------------------------------------------------
uint32_t last = 0;
int kPg_Fault (uint32_t address)
{
  kprintf ("PF] PF at <%x> \n", address);
  if (address == last && address < USR_SPACE_LIMIT) {
    kprintf ("PF] DOUBLE, WTF %x !?\n", address);
    kPg_DumpTable (TABLE_DIR_THR);
    for (;;);
  } else
    last = address;
  if (address < USR_SPACE_BASE)
    kpanic ("PF] PG NOT ALLOWED <%x> \n", address);
  else if (address < USR_SPACE_LIMIT) {
    kAddSpace_t* mmspc = kCPU.current_->process_->memSpace_;
    kVma_t* vma = kVma_FindAt (mmspc, address);
    if (vma == NULL ) {
      kprintf ("PF] Page fault in user space <%x> SIGFAULT \n", address);
      for (;;);
    } else if (vma->flags_ & VMA_STACK) {
      kPg_Resolve (address, TABLE_DIR_KRN, PG_USER_RDWR, PG_USER_RDWR);
    } else if (vma->ino_ != NULL)
      kPg_FillStream (vma, ALIGN_DW (address, PAGE_SIZE),  PG_USER_RDWR);
    else {
      kprintf ("PF] Page fault in user space <%x> [%x] \n", address, vma);
      for (;;);
    }
  } else if (address < 0xffffc000) 
    kPg_Resolve (address, TABLE_DIR_KRN, PG_KERNEL_ONLY, PG_KERNEL_ONLY);
  else 
    kpanic ("PF] PG NOT ALLOWED <%x> \n", address);

  return __noerror();
}

// ---------------------------------------------------------------------------
uint32_t kPg_NewDir () 
{
  int i; 
  uint32_t page = kPg_AllocPage();
  // FIXME check this operation is possible (sti, realy clean here!)
  TABLE_DIR_THR [1020] = page | PG_KERNEL_ONLY;
  if (KLOG_PF) kprintf ("pg] Create a new directory <%X> \n", page);

  memset (TABLE_DIR_WIN, 0, PAGE_SIZE);
  TABLE_DIR_WIN [0] = TABLE_DIR_THR [0];
  TABLE_DIR_WIN [1] = TABLE_DIR_THR [1];  // FIXME Handle the kernel better than that

  int kstart = (USR_SPACE_LIMIT >> 22) & 0x3ff;
  for (i = kstart; i < 1021; ++i) 
    TABLE_DIR_WIN [i] = TABLE_DIR_THR [i];
  TABLE_DIR_WIN [1021] = TABLE_DIR_THR [1021];
  TABLE_DIR_WIN [1022] = page | PG_KERNEL_ONLY;
  TABLE_DIR_WIN [1023] = page | PG_KERNEL_ONLY;

  // kPg_DumpTable (TABLE_DIR_WIN);
  kprintf ("pg] Directory is ready\n"); // FIXME can't remove this, need to handle TBL invalidation first
  TABLE_DIR_THR [1020] = 0;
  return page;
}


