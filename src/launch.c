#include <smkos/kernel.h>
#include <smkos/core.h>
#include <smkos/io.h>
#include <smkos/memory.h>
#include <smkos/arch.h>


static const char *masterPaths[] = {
  "sbin/master.xe",
  "bin/master.xe",
  "master.xe",
  "sbin/master",
  "bin/master",
  "master",
  NULL
};


/* ----------------------------------------------------------------------- */
void kernel_ready ()
{
  kprintf ("CPU %d is ready\n", kCpuNo);
  // for (;;);
  // while (!kSYS.ready_) __delayX(10000);
}


/* ----------------------------------------------------------------------- */
void kernel_start ()
{
  int idx;
  kUser_t* user;
  kInode_t *ino;
  struct tm dateTime;
  
  /* Initialize kernel environment */
  kernel_state (KST_KERNSP);
  kprintf("SmokeOS " _VTAG_ ", build at " __DATE__ " from git:" _GITH_ " on " _OSNAME_ ".\n");
  mmu_load_env();

  /* Initialize time managment */
  dateTime = cpu_get_clock();
  kprintf ("Date: %s\n", asctime(&dateTime));

  initialize_smp();

  /* Initialize the VFS */
  initialize_vfs();

  /* Search kernel helper files */
  ino = search_inode ("boot/kImage.map", kSYS.sysIno_, 0);
  if (ino)
   ksymbols_load(ino);
  
  /* Create basic users */
  user = create_user("system", CAP_SYSTEM);
  if (!user)
    kpanic("Unable to create system user.\n");

  /* Load master program */
  idx = 0;
  while (masterPaths[idx]) {
    ino = search_inode (masterPaths[idx], kSYS.sysIno_, 0);
    if (ino && NULL != load_assembly(ino))
      break;
    ++idx;
  }

  // display_inodes();
  
  if (!masterPaths[idx])
    kpanic("Unable to find startup program 'MASTER'\n");

  create_logon_process(ino, user, kSYS.sysIno_, masterPaths[idx]);
  scavenge_area(kSYS.mspace_);

  // Open Graphic buffer
  ino = search_inode ("/dev/Fb0", NULL, 0);
  if (ino) 
    ktty(ino);

  kprintf ("CPU %d is ready\n", kCpuNo);
  cpu_start_scheduler();
}


/* ----------------------------------------------------------------------- */
/* ----------------------------------------------------------------------- */