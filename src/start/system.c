#include "tty.h"
#include "cpu.h"
#include <kernel/info.h>

#include "kernel/inodes.h"
#include "kernel/memory.h"
#include "kernel/scheduler.h"
#include <kernel/assembly.h>

void kinit()
{
  meminit_r(&kSYS.kheap, (void*)0xD0000000, 0x20000000);
  // kCPU.asp = kMem_Create (4 * _Mb_);

  // kTty_NewTerminal (0x7000, 0x10000 - 0x7000);
}



struct tm RTC_GetTime ();
void RTC_EnableCMOS () ;
void RTC_DisableCMOS () ;
void PIT_Initialize (uint32_t frequency);
int ATA_Initialize(kInode_t* dev);
int VBA_Initialize(kInode_t* dev);
int ISO_mount (kInode_t* dev, kInode_t* mnt, const char* name);
void ksymbols_load (kInode_t* ino);


  // int    tm_sec;   /**< seconds [0,61] */
  // int    tm_min;   /**< minutes [0,59] */
  // int    tm_hour;  /**< hour [0,23] */
  // int    tm_mday;  /**< day of month [1,31] */
  // int    tm_mon;   *< month of year [0,11] 
  // int    tm_year;  /**< years since 1900 */
  // int    tm_wday;  /**< day of week [0,6] (Sunday = 0) */
  // int    tm_yday;  /**< day of year [0,365] */
  // int    tm_isdst; /**< daylight savings flag */

int kCore_Initialize ()
{
  if (screen._mode == 1) {
    screen._ptr = (uint32_t*)(4 * _Mb_);
  }


  // I. Print kernel info
  kprintf ("Kernel Smoke v0.0 Build 0 compiled Jul 26 2014 with gcc 4.7.2\n");

  // II. Initialize system core
  kinit ();


  // III. Set Date and initiate timer
  char tmp[510];
  struct tm dt = RTC_GetTime ();

// kprintf("Dt sec: %d \n", dt.tm_sec);
// kprintf("Dt min: %d \n", dt.tm_min);
// kprintf("Dt hour: %d \n", dt.tm_hour);
// kprintf("Dt tm_mday: %d \n", dt.tm_mday);
// kprintf("Dt tm_mon: %d \n", dt.tm_mon);
// kprintf("Dt tm_year: %d \n", dt.tm_year);
// kprintf("Dt tm_wday: %d \n", dt.tm_wday);
// kprintf("Dt tm_yday: %d \n", dt.tm_yday);
// kprintf("Dt tm_isdst: %d \n", dt.tm_mday);

  asctime_r (&dt, tmp);
  kprintf ("Date: %s\n", tmp);
  // RTC_EnableCMOS ();
  PIT_Initialize(CLOCK_HZ);


  // IV. Start system build
  kCpu_SetStatus (CPU_STATE_SYSCALL);

  kfs_init ();
  kvma_init ();
  ATA_Initialize (kSYS.devNd_);
  VBA_Initialize (kSYS.devNd_);


  // V. Mount the disc system
  kInode_t* cd = kfs_lookup ("/dev/sdA", NULL);
  kInode_t* mnt = kfs_lookup ("/mnt/", NULL);
  ISO_mount (cd, mnt, "system");
  // KRP_Mount (NULL, mnt);

  // VI. Look for debug symbols
  kInode_t* sym = kfs_lookup ("/mnt/system/BOOT/KIMAGE.MAP", NULL);
  if (sym != NULL)
    ksymbols_load(sym);
  else
    kprintf ("We can't found the file kImage.map\n");

  // VII. Start default programs
  kInode_t* path = kfs_lookup ("/mnt/system/USR/BIN/", NULL);
  kInode_t* master = kfs_lookup ("MASTER.", path);
  kInode_t* deamon = kfs_lookup ("DEAMON.", path);



  ksch_create_process (NULL, master, path, "");
  ksch_create_process (NULL, deamon, path, "");

  // kfs_log_all();

  // VIII. Initialize per-cpu scheduler
  ksch_init ();

  return 0;
}
