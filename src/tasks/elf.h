#ifndef _ELF_H__
#define _ELF_H__

#define ELFCLASSNONE    0       // Invalid class
#define ELFCLASS32      1       // 32-bit objects
#define ELFCLASS64      2       // 64-bit objects

#define ET_NONE         0       // No file type
#define ET_REL          1       // Relocatable file
#define ET_EXEC         2       // Executable file
#define ET_DYN          3       // Shared object file
#define ET_CORE         4       // Core file
#define ET_LOPROC       0xff00  // Processor-specific
#define ET_HIPROC       0xffff  // Processor-specific

#define EM_NONE         0       // No machine
#define EM_M32          1       // AT&T WE 32100
#define EM_SPARC        2       // SPARC
#define EM_386          3       // Intel 80386
#define EM_68K          4       // Motorola 68000
#define EM_88K          5       // Motorola 88000
#define EM_860          7       // Intel 80860
#define EM_MIPS         8       // MIPS RS3000

/* Sh Type */
#define PT_NULL             0
#define PT_LOAD             1
#define PT_DYNAMIC          2
#define PT_INTERP           3
#define PT_NOTE             4
#define PT_SHLIB            5
#define PT_PHDR             6
#define PT_LOPROC  0x70000000
#define PT_HIPROC  0x7fffffff


typedef struct
{
  uint8_t     ident_[16];
  uint16_t    type_;
  uint16_t    machine_;
  uint32_t    version_;
  uint32_t    entry_;
  uint32_t    phOff_;
  uint32_t    shOff_;
  uint32_t    flags_;
  uint16_t    ehSize_;
  uint16_t    phSize_;
  uint16_t    phCount_;
  uint16_t    shSize_;
  uint16_t    shCount_;
  uint16_t    shstRndx_;

} ELF_Header_t;

typedef struct
{
  uint32_t    type_;
  uint32_t    fileAddr_;
  uint32_t    virtAddr_;
  uint32_t    physAddr_;
  uint32_t    fileSize_;
  uint32_t    memSize_;
  uint32_t    flags_;
  uint32_t    align_;
} ELF_PhEntry_t;

static uint8_t ELFident[] = {
  0x7f, 0x45, 0x4c, 0x46, ELFCLASS32, 0x01, 0x01, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

#endif /* _ELF_H__ */
