#      This file is part of the SmokeOS project.
#  Copyright (C) 2015  <Fabien Bavent>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Affero General Public License as
#  published by the Free Software Foundation, either version 3 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Affero General Public License for more details.
#
#  You should have received a copy of the GNU Affero General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
is_pc= $(shell [ $(ARCH) = x86 ] && echo y)

# M O D U L E S -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Kernel sources
mod_krn-y += $(wildcard $(srcdir)/kern/*.c)

# Minimum file systems
mod_mfs-y += $(srcdir)/fs/gpt.c $(srcdir)/fs/iso.c $(srcdir)/fs/tmpfs.c
mod_mfs-y += $(srcdir)/fs/kdb.c $(srcdir)/fs/fat.c

# Configured file systems
mod_kfs-y += $(mod_mfs-y)
mod_kfs-$(is_pc) += $(srcdir)/fs/ata.c $(srcdir)/fs/svga.c


# F L A G S -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
CFLAGS += -Wall -Wextra -Wno-unused-parameter 
CFLAGS += -D_DATE_=\"'$(DATE)'\" -D_OSNAME_=\"'$(LINUX)'\"
CFLAGS += -D_GITH_=\"'$(GIT)'\" -D_VTAG_=\"'$(VERSION)'\"

ut_LFLAGS += --coverage
ut_CFLAGS += -ggdb3 --coverage $(CFLAGS) -D_FS -D_FS_UM
ut_CFLAGS += -I $(topdir)/include
ut_CFLAGS += -I $(topdir)/include/_um

kum_LFLAGS += --coverage
kum_CFLAGS += -ggdb3 --coverage $(CFLAGS) -D_FS -D_FS_UM
kum_CFLAGS += -I $(topdir)/include
kum_CFLAGS += -I $(topdir)/include/_um

krn_$(ARCH)_LFLAGS += -nostdlib
krn_$(ARCH)_CFLAGS += $(CFLAGS)
krn_$(ARCH)_CFLAGS += -I $(topdir)/include
krn_$(ARCH)_CFLAGS += -I $(topdir)/include/_$(ARCH)


# D E L I V E R I E S -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Kernel sources
kImage_src-y += $(mod_krn-y) $(mod_kfs-y)
kImage_src-y += $(wildcard $(srcdir)/_$(ARCH)/*.c)
kImage_src-y += $(wildcard $(srcdir)/libc/*.c)


# kernel usermode (functional tests)
kum_src-y += $(mod_krn-y) $(mod_mfs-y)
kum_src-y += $(wildcard $(srcdir)/_um/*.c)
kum_src-y += $(srcdir)/fs/bmp.c $(srcdir)/fs/hdd.c


# U N I T - T E S T S -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# UT_utils (unit tests)
ut_utils_src-y += $(srcdir)/src/test/utils.c

# UT_vfs (unit tests)
ut_vfs_src-y += $(srcdir)/src/test/vfs.c


# T A R G E T S -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
$(eval $(call ccpl,ut))
$(eval $(call ccpl,kum))
$(eval $(call ccpl,krn_$(ARCH)))
$(eval $(call link,ut_utils,ut))
$(eval $(call link,ut_vfs,ut))
$(eval $(call link,kum,kum))
$(eval $(call kimg,kImage,krn_$(ARCH)))
$(eval $(call crt,crt0))
$(eval $(call crt,crtk))

DV_CHECK += $(bindir)/ut_utils $(bindir)/ut_vfs
DV_UTILS += $(bindir)/kum $(gendir)/kImage
