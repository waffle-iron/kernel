#include <smkos/kernel.h>
#include <smkos/arch.h>

// void VBA_Set (void *address, int width, int height, int depth);
// int kTty_PreSystem (uint32_t *base, int width, int height, int depth);
// extern char *klogStart;
// extern int klogLg;


/* ----------------------------------------------------------------------- */
/** Read memory map */
static int grub_memory (uint32_t *mmap)
{
  int64_t base;
  int64_t length;
  // uint64_t total = 0;

  mmu_prolog();

  for (; mmap[0] == 0x14; mmap += 6) {

    base = (int64_t)mmap[1] | (int64_t)mmap[2] << 32;
    length = (int64_t)mmap[3] | (int64_t)mmap[4] << 32;

    // if (base + length > total) total = base + length;
    // kprintf ("MEM RECORD [%x-%x] <%x>\n", (uint32_t)base, (uint32_t)length, (uint32_t)(total >> 32ULL));
    if (mmap[5] == 1) {
      mmu_ram(base, length);
      //kpg_ram (base, length);
    }
  }

  // kprintf ("Memory detected %d Kb\n", (uint32_t)(total / 1024) );
  // kpg_init ();
  mmu_init();
  return __seterrno(0);
}


/* ----------------------------------------------------------------------- */
/** Using grub at boot, we send to the kernel all known machine infos. */
int grub_initialize (uint32_t *bTable)
{
  if (bTable[0] & (1 << 11) && bTable[22] != 0x000B8000) {
    VGA_Info(bTable[22], bTable[25], bTable[26], 4);
  }
  //   kTty_PreSystem ((void *)bTable[22], bTable[25], bTable[26], 4);
  //   VBA_Set ((void *)bTable[22], bTable[25], bTable[26], 4);
  // } else  {
  //   kTty_PreSystem ((void *)0x000B8000, 80, 25, 2);
  // }

  // memset (klogStart, 0, klogLg);

  // kprintf ("\n");
  if (bTable[0] & (1 << 9)) {
    kprintf ("Boot Loader: %s\n", (char *)bTable[16]);
  }

  if (bTable[0] & (1 << 1)) {
    kprintf ("Booting device: ");

    if (bTable[3] >> 24 == 0x80)
      kprintf ("HDD\n");
    else if (bTable[3] >> 24 == 0x80)
      kprintf ("HDD\n");
    else if (bTable[3] >> 24 == 0xe0)
      kprintf ("CD\n");
    else
      kprintf ("Unknown <%x>\n", bTable[3] >> 24);
  }

  if (!(bTable[0] & (1 << 6))) {
    return -1;
  }

  grub_memory ((uint32_t *)bTable[12]);
  // kprintf ("POINTER SCREEN 0x%08x\n", bTable[22]);
  return __seterrno(0);
}
