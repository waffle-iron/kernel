# Makefile

#ifeq ($(MAKE_SCRIPTS_DIR),)
  MAKE_SCRIPTS_DIR = tools
#endif

-include $(MAKE_SCRIPTS_DIR)/settings.mk
-include $(MAKE_SCRIPTS_DIR)/common.mk


# ===========================================================================

linuxname = $(shell uname -sr)
date = $(shell date '+%d %b %Y')
AXLIBC = ../axlibc

pack_cflags =  -D_DATE_=\"'$(date)'\" -D_OS_FULLNAME_=\"'$(linuxname)'\"

cflags = $(pack_cflags) -Wall -Wextra -Wno-unused-parameter

std_debug_cflags = $(cflags) -g -ggdb
std_cov_cflags = $(cflags) --coverage -fprofile-arcs -ftest-coverage
std_release_cflags = -fPIC -O3 $(cflags)


std_cov_lflags = $(lflags) -fprofile-arcs

check: all
	$(bin_dir)/inodes
	$(bin_dir)/memory
	$(bin_dir)/scheduler

# ===========================================================================


# ---------------------------------------------------------------------------
# Target: Program  inodes
inodes_src = $(patsubst src/%,%,$(wildcard src/inodes/*.c)) \
			       $(patsubst src/%,%,$(wildcard src/core/*.c)) \
			       $(patsubst src/%,%,$(wildcard src/fs/tmpfs/*.c)) \
			       $(patsubst src/%,%,$(wildcard src/fs/img/*.c)) \
			       $(patsubst src/%,%,$(wildcard src/fs/iso9660/*.c)) \
			       dbg/inodes.c
inodes_inc = include/
inodes_cflags = $(std_$(mode)_cflags)
inodes_lflags = $(std_$(mode)_lflags)
$(eval $(call PROGRAM,inodes))


# ---------------------------------------------------------------------------
# Target: Program  memory
memory_src = $(patsubst src/%,%,$(wildcard src/memory/*.c)) \
			       $(patsubst src/%,%,$(wildcard src/core/*.c)) \
			       dbg/memory.c
memory_inc = include/
memory_cflags = $(std_$(mode)_cflags)
memory_lflags = $(std_$(mode)_lflags)
$(eval $(call PROGRAM,memory))


# ---------------------------------------------------------------------------
# Target: Program  scheduler
scheduler_src = $(patsubst src/%,%,$(wildcard src/scheduler/*.c)) \
			          $(patsubst src/%,%,$(wildcard src/core/*.c)) \
			          dbg/scheduler.c

scheduler_inc = include/
scheduler_cflags = $(std_$(mode)_cflags)
scheduler_lflags = $(std_$(mode)_lflags)
$(eval $(call PROGRAM,scheduler))


# ---------------------------------------------------------------------------
# # Target: Program  mods
# mods_src = $(patsubst src/%,%,$(wildcard src/core/*.c)) \
# 		       $(patsubst src/%,%,$(wildcard src/fs/tmpfs/*.c)) \
# 		       $(patsubst src/%,%,$(wildcard src/fs/img/*.c)) \
# 		       $(patsubst src/%,%,$(wildcard src/fs/iso9660/*.c)) \
#            $(patsubst src/%,%,$(wildcard src/inodes/*.c)) \
#            $(patsubst src/%,%,$(wildcard src/memory/*.c)) \
#            $(patsubst src/%,%,$(wildcard src/tasks/*.c)) \
#            dbg/mods.c
# mods_inc = include/
# mods_cflags = $(std_$(mode)_cflags)
# mods_lflags = $(std_$(mode)_lflags)
# $(eval $(call PROGRAM,mods))


# ===========================================================================

CRTK_out = $(obj_dir)/crtk.o
CRTK_src = arch/i386/crtk.asm $(wildcard arch/i386/kern/*.asm)
$(eval $(call CRT,CRTK))

CRT0_out = $(obj_dir)/crt0.o
CRT0_src = arch/i386/crt0.asm
$(eval $(call CRT,CRT0))


# ===========================================================================


# ---------------------------------------------------------------------------
# Target: Program  kMin
kMin_src = $(patsubst src/%,%,$(wildcard src/start/*.c)) \
           $(patsubst src/%,%,$(wildcard src/core/*.c))
kMin_crt = $(obj_dir)/crtk.o
kMin_inc = include/ $(AXLIBC)/include/ $(AXLIBC)/internal/
kMin_cflags = $(std_$(mode)_cflags) -nostdinc -D__EX -D__KERNEL
kMin_lflags = $(AXLIBC)/lib/libAxRaw.a
$(eval $(call KERNEL,kMin))


# ---------------------------------------------------------------------------
# Target: Program  kImage
			       # $(patsubst src/%,%,$(wildcard src/fs/krp/*.c))
kImage_src = $(patsubst src/%,%,$(wildcard src/start/*.c)) \
             $(patsubst src/%,%,$(wildcard src/syscalls/*.c))  \
             $(patsubst src/%,%,$(wildcard src/inodes/*.c))  \
             $(patsubst src/%,%,$(wildcard src/assembly/*.c))  \
             $(patsubst src/%,%,$(wildcard src/memory/*.c)) \
             $(patsubst src/%,%,$(wildcard src/scheduler/*.c)) \
			       $(patsubst src/%,%,$(wildcard src/fs/tmpfs/*.c)) \
			       $(patsubst src/%,%,$(wildcard src/fs/ata/*.c)) \
			       $(patsubst src/%,%,$(wildcard src/fs/iso9660/*.c)) \
             $(patsubst src/%,%,$(wildcard src/core/*.c))
kImage_crt = $(obj_dir)/crtk.o
kImage_inc = include/ $(AXLIBC)/include/ $(AXLIBC)/internal/
kImage_cflags = $(std_$(mode)_cflags) -nostdinc -D__EX -D__KERNEL
kImage_lflags = $(AXLIBC)/lib/libAxRaw.a
$(eval $(call KERNEL,kImage))


# ===========================================================================
# ===========================================================================


# ---------------------------------------------------------------------------
master_src = dummy/master.c
master_crt = $(obj_dir)/crt0.o
master_cflags = $(std_$(mode)_cflags) -nostdinc
master_lflags = $(AXLIBC)/lib/libaxc.a
$(eval $(call KPROGRAM,master))
# ---------------------------------------------------------------------------
deamon_src = dummy/deamon.c
deamon_crt = $(obj_dir)/crt0.o
deamon_cflags = $(std_$(mode)_cflags) -nostdinc
deamon_lflags = $(AXLIBC)/lib/libaxc.a
$(eval $(call KPROGRAM,deamon))

# ===========================================================================
# ===========================================================================
kimg = kImage

cdrom: Os.iso
Os.iso: $(kimg) master deamon
	$(VVV) mkdir -p iso/boot/grub
#	$(VVV) mkdir -p iso/usr/{,local/}{bin,lib,sbin}
	$(VVV) mkdir -p iso/usr/bin
	$(VVV) mkdir -p iso/usr/lib
	$(VVV) mkdir -p iso/usr/sbin
	$(VVV) mkdir -p iso/usr/local/bin
	$(VVV) mkdir -p iso/usr/local/lib
	$(VVV) mkdir -p iso/usr/local/sbin

	$(VV) cp tools/grub/grub.cfg  iso/boot/grub/grub.cfg
	$(VV) cp $(bin_dir)/$(kimg) iso/boot/kImage

	$(VV) cp $(bin_dir)/master iso/usr/bin/master
	$(VV) cp $(bin_dir)/deamon iso/usr/bin/deamon

	$(VV) cp ../axBox/bin/i686/krn/buzybox iso/usr/bin/buzybox
	$(VV) cp ../axBox/scripts/* iso/usr/bin/

	$(V) grub-mkrescue -o $@ iso -A Os_Core > /dev/null
	$(VV) rm -rf iso


# ===========================================================================

-include $(MAKE_SCRIPTS_DIR)/utils.mk

