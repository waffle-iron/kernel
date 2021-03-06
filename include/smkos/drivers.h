#pragma once
#include <smkos/kapi.h>

/* Those are major driver number - they're used only inside a driver but we
 * keep them here for listing */
#define ISO_No 12
#define GPT_No 35
#define ATA_No 41
#define HDD_No 42
#define VGA_No 23
#define BMP_No 24
#define KDB_No 38
#define FAT_No 16


/* --- File Systems ------------------------------------------------------ */
void TMPFS(kDriver_t *driver);
void GPT(kDriver_t *driver);
void ISO9660(kDriver_t *driver);
void FATFS(kDriver_t *driver);
/* --- Block drivers ----------------------------------------------------- */
void ATA(kDriver_t *driver);
void VGA(kDriver_t *driver);
void HDD(kDriver_t *driver);
void BMP(kDriver_t *driver);
/* --- Char drivers ------------------------------------------------------ */
void KDB(kDriver_t *driver);


/* --- Register all drivers ---------------------------------------------- */
static inline void init_drivers()
{
#if !defined(_FS_NONE)
  register_driver(GPT);
  register_driver(ISO9660);
  register_driver(FATFS);
  register_driver(KDB);

#if !defined(_FS) || defined(_FS_x86) /* _x86 */
  register_driver(ATA);
  register_driver(VGA);
#elif defined(_FS) && defined(_FS_UM) /* _um */
  register_driver(HDD);
  register_driver(BMP);
#endif
#endif
}


/* ----------------------------------------------------------------------- */
/* ----------------------------------------------------------------------- */
