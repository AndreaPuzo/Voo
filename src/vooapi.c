#include "voo.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#define r(__type, __x) (__type *)(voo->ker.reg + (__x))
#define s(__x)         (voo->ker.seg + (__x))

#define r8(__x)  r(u8_t , (__x))
#define r16(__x) r(u16_t, (__x))
#define r32(__x) r(u32_t, (__x))
#define r64(__x) r(u64_t, (__x))

#define rAX    *r64(VOO_REG_AX)
#define rCX    *r64(VOO_REG_CX)
#define rDX    *r64(VOO_REG_DX)
#define rBX    *r64(VOO_REG_BX)

#define rSP    *r64(VOO_REG_SP)
#define rBP    *r64(VOO_REG_BP)
#define rFLAGS *r32(VOO_REG_FLAGS)
#define rIP    *r64(VOO_REG_IP)
#define rCLOCK *r64(VOO_REG_CLOCK)
#define rIRQ   *r32(VOO_REG_IRQ)

#define rCODE  voo->ker.seg[VOO_SEG_CODE]
#define rDATA  voo->ker.seg[VOO_SEG_DATA]
#define rSTACK voo->ker.seg[VOO_SEG_STACK]

#define IOPL ((rFLAGS >> 12) & 3)

#define u8(__x)  r(u8_t , (__x))
#define u16(__x) r(u16_t, (__x))
#define u32(__x) r(u32_t, (__x))
#define u64(__x) r(u64_t, (__x))
#define i8(__x)  r(i8_t , (__x))
#define i16(__x) r(i16_t, (__x))
#define i32(__x) r(i32_t, (__x))
#define i64(__x) r(i64_t, (__x))

enum {
  PERM_X    = 1 << 0 ,
  PERM_R    = 1 << 1 ,
  PERM_W    = 1 << 2 ,
  PERM_P    = 1 << 3 ,
  PERM_IOPL = 3 << 4
} ;

typedef struct desc_s desc_t ;

struct desc_s {
  u64_t addr ;
  u64_t size ;
  u32_t perm ;
} ;

__VOO_STATIC
u32_t __vaddr_to_paddr (
  voo_t *  voo  ,
  u32_t    seg  ,
  u64_t    addr ,
  desc_t * desc
)
{
  desc->addr =
    ((seg >> 4) & 0x3FF) << (10 * ((seg >> 0) & 3))
  ;

  desc->size =
    (((seg >> 14) & 0x3FF) + 1) << (10 * ((seg >> 2) & 3))
  ;

  desc->perm =
    (seg >> 24) & 0xFF
  ;

  if (0 == (desc->perm & PERM_P))
    return voo_int(voo, VOO_IRQ_SEGMENT_PRESENT) ;

  if (((desc->perm >> 4) & 3) < IOPL)
    return voo_int(voo, VOO_IRQ_SEGMENT_ACCESS) ;

  if (desc->size <= addr)
    return voo_int(voo, VOO_IRQ_SEGMENT_FAULT) ;

  return VOO_N_IRQS ;
}

__VOO_STATIC
u32_t __push_8 (
  voo_t * voo  ,
  u8_t    data
)
{
  voo->opr.stack = 1 ;
  rSP -= sizeof(data) ;

  if (
    VOO_N_IRQS != voo_write_8(
      voo    ,
      rSTACK ,
      rSP    ,
      data
    )
  ) return voo_int(voo, VOO_IRQ_STACK_OVERFLOW) ;

  return VOO_N_IRQS ;
}

__VOO_STATIC
u32_t __push_16 (
  voo_t * voo  ,
  u16_t   data
)
{
  voo->opr.stack = 1 ;
  rSP -= sizeof(data) ;

  if (
    VOO_N_IRQS != voo_write_16(
      voo    ,
      rSTACK ,
      rSP    ,
      data
    )
  ) return voo_int(voo, VOO_IRQ_STACK_OVERFLOW) ;

  return VOO_N_IRQS ;
}

__VOO_STATIC
u32_t __push_32 (
  voo_t * voo  ,
  u32_t   data
)
{
  voo->opr.stack = 1 ;
  rSP -= sizeof(data) ;

  if (
    VOO_N_IRQS != voo_write_32(
      voo    ,
      rSTACK ,
      rSP    ,
      data
    )
  ) return voo_int(voo, VOO_IRQ_STACK_OVERFLOW) ;

  return VOO_N_IRQS ;
}

__VOO_STATIC
u32_t __push_64 (
  voo_t * voo  ,
  u64_t   data
)
{
  voo->opr.stack = 1 ;
  rSP -= sizeof(data) ;

  if (
    VOO_N_IRQS != voo_write_64(
      voo    ,
      rSTACK ,
      rSP    ,
      data
    )
  ) return voo_int(voo, VOO_IRQ_STACK_OVERFLOW) ;

  return VOO_N_IRQS ;
}

__VOO_STATIC
u32_t __pop_8 (
  voo_t * voo  ,
  u8_t *  data
)
{
  voo->opr.stack = 1 ;

  if (
    VOO_N_IRQS != voo_read_8(
      voo    ,
      rSTACK ,
      rSP    ,
      data
    )
  ) return voo_int(voo, VOO_IRQ_STACK_UNDERFLOW) ;

  rSP += sizeof(*data) ;

  return VOO_N_IRQS ;
}

__VOO_STATIC
u32_t __pop_16 (
  voo_t * voo  ,
  u16_t * data
)
{
  voo->opr.stack = 1 ;

  if (
    VOO_N_IRQS != voo_read_16(
      voo    ,
      rSTACK ,
      rSP    ,
      data
    )
  ) return voo_int(voo, VOO_IRQ_STACK_UNDERFLOW) ;

  rSP += sizeof(*data) ;

  return VOO_N_IRQS ;
}

__VOO_STATIC
u32_t __pop_32 (
  voo_t * voo  ,
  u32_t * data
)
{
  voo->opr.stack = 1 ;

  if (
    VOO_N_IRQS != voo_read_32(
      voo    ,
      rSTACK ,
      rSP    ,
      data
    )
  ) return voo_int(voo, VOO_IRQ_STACK_UNDERFLOW) ;

  rSP += sizeof(*data) ;

  return VOO_N_IRQS ;
}

__VOO_STATIC
u32_t __pop_64 (
  voo_t * voo  ,
  u64_t * data
)
{
  voo->opr.stack = 1 ;

  if (
    VOO_N_IRQS != voo_read_64(
      voo    ,
      rSTACK ,
      rSP    ,
      data
    )
  ) return voo_int(voo, VOO_IRQ_STACK_UNDERFLOW) ;

  rSP += sizeof(*data) ;

  return VOO_N_IRQS ;
}

__VOO_API
u32_t voo_boot (
  voo_t * voo
)
{
  if (0 != (rFLAGS & VOO_FLAG_1))
    return voo_int(voo, VOO_IRQ_GENERAL_PROTECT) ;

  for (int i = 0 ; i < VOO_N_SEGS ; ++i)
    voo->ker.seg[i] = 0 ;

  for (int i = 0 ; i < VOO_N_REGS ; ++i)
    voo->ker.reg[i] = 0 ;

  voo->opr.seg  = 0    ;
  voo->opr.addr = 0    ;
  voo->opr.data = 0    ;
  voo->mem.size = 0    ;
  voo->mem.data = NULL ;

  FILE * fp = fopen(".voocfg", "r") ;

  if (NULL == fp) {
    fprintf(stderr, "error: cannot open `.voocfg`\n") ;
    return VOO_IRQ_RESERVED_1 ;
  }

  u64_t mem ;
  char  mul ;
  char  img [__VOO_MAX_PATH + __VOO_MAX_FILENAME + 1] ;

  fscanf(
    fp                       ,
    "MEM %" PRIu64 " %ciB\n"
    "IMG %s"                 ,
    &mem                     ,
    &mul                     ,
    img
  ) ;

  fclose(fp) ;

  switch (mul) {
  case 'K' : mul = 10 ; break ;
  case 'M' : mul = 20 ; break ;
  case 'G' : mul = 30 ; break ;

  default : {
    fprintf(stderr, "error: invalid memory multiplier `%ciB`\n", mul) ;
    return VOO_IRQ_RESERVED_1 ;
  }
  }

  voo->mem.size = mem << mul ;

  if (voo->mem.size < __VOO_MEMORY_SIZE) {
    fprintf(stderr, "error: not enough memory to boot the system\n") ;
    return VOO_IRQ_RESERVED_1 ;
  }

  voo->mem.data = (u8_t *)calloc(voo->mem.size, sizeof(u8_t)) ;

  if (NULL == voo->mem.data)
    return VOO_IRQ_RESERVED_1 ;

  fp = fopen(img, "rb") ;

  if (NULL == fp) {
    fprintf(stderr, "error: cannot open `%s`\n", img) ;
    return VOO_IRQ_RESERVED_1 ;
  }

  u8_t mag [] = {
    0x45 ,
    0x70 ,
    0xFA ,
    0xDE
  } ;

  u8_t ver [] = {
    __VOO_VERSION_MAJOR ,
    __VOO_VERSION_MINOR ,
    __VOO_VERSION_PATCH ,
    __VOO_VERSION_EXTRA
  } ;

  if (
    mag[0] != fgetc(fp) ||
    mag[1] != fgetc(fp) ||
    mag[2] != fgetc(fp) ||
    mag[3] != fgetc(fp)
  ) {
    fprintf(stderr, "error: invalid magic number\n") ;
    fclose(fp) ;
    return VOO_IRQ_RESERVED_1 ;
  }

  if (
    ver[0] != fgetc(fp) ||
    ver[1] != fgetc(fp) ||
    ver[2] != fgetc(fp) ||
    ver[3] != fgetc(fp)
  ) {
    fprintf(stderr, "error: invalid version number\n") ;
    fclose(fp) ;
    return VOO_IRQ_RESERVED_1 ;
  }

  u32_t endian =
    ((u32_t)fgetc(fp) << 24) |
    ((u32_t)fgetc(fp) << 16) |
    ((u32_t)fgetc(fp) <<  8) |
    ((u32_t)fgetc(fp) <<  0)
  ;

  if (endian != 0x00010203 && endian != 0x03020100) {
    fprintf(stderr, "error: invalid endian\n") ;
    fclose(fp) ;
    return VOO_IRQ_RESERVED_1 ;
  }

  u32_t secs ;

  if (1 != fread(&secs, sizeof(secs), 1, fp)) {
    fprintf(stderr, "error: cannot read the number of sections\n") ;
    fclose(fp) ;
    return VOO_IRQ_RESERVED_1 ;
  }

  if (__VOO_ENDIAN != endian)
    secs = voo_bswap_32(secs) ;

  for (u32_t i = 0 ; i < secs ; ++i) {
    u64_t addr ;
    u64_t size ;
    u32_t type ;

    if (1 != fread(&addr, sizeof(addr), 1, fp)) {
      fprintf(stderr, "In section %" PRIu32 ":\n", i) ;
      fprintf(stderr, "  error: cannot read the address\n") ;
      fclose(fp) ;
      return VOO_IRQ_RESERVED_1 ;
    }

    if (1 != fread(&size, sizeof(size), 1, fp)) {
      fprintf(stderr, "In section %" PRIu32 ":\n", i) ;
      fprintf(stderr, "  error: cannot read the size\n") ;
      fclose(fp) ;
      return VOO_IRQ_RESERVED_1 ;
    }

    if (1 != fread(&type, sizeof(type), 1, fp)) {
      fprintf(stderr, "In section %" PRIu32 ":\n", i) ;
      fprintf(stderr, "  error: cannot read the type\n") ;
      fclose(fp) ;
      return VOO_IRQ_RESERVED_1 ;
    }

    if (__VOO_ENDIAN != endian) {
      addr = voo_bswap_64(addr) ;
      size = voo_bswap_64(size) ;
      type = voo_bswap_32(type) ;
    }

    if (0 == size) {
      fprintf(stderr, "In section %" PRIu32 ":\n", i) ;
      fprintf(stderr, "  error: empty section\n") ;
      fclose(fp) ;
      return VOO_IRQ_RESERVED_1 ;
    }

    if (voo->mem.size < addr + size) {
      fprintf(stderr, "In section %" PRIu32 ":\n", i) ;
      fprintf(
        stderr                                                       ,
        "  error: out of memory: the max address is 0x%" PRIX64 "\n"
        "  note: the max memory address is at 0x%" PRIX64 "\n"       ,
        addr + size - 1                                              ,
        voo->mem.size - 1
      ) ;
      fclose(fp) ;
      return VOO_IRQ_RESERVED_1 ;
    }

    if (size != fread(voo->mem.data + addr, sizeof(u8_t), size, fp)) {
      fprintf(stderr, "In section %" PRIu32 ":\n", i) ;
      fprintf(stderr, "  error: cannot read data from the section\n") ;
      fclose(fp) ;
      return VOO_IRQ_RESERVED_1 ;
    }

    switch (type) {
    case 0x00 :   // 1-aligned (or unaligned)
      break ;

    case 0x01 :   // 2-aligned
    case 0x02 :   // 4-aligned
    case 0x03 : { // 8-aligned
      if (size < (1 << type) || 0 != (size & ((1 << type) - 1))) {
        fprintf(stderr, "In section %" PRIu32 ":\n", i) ;
        fprintf(
          stderr                                                 ,
          "  error: invalid size\n"                              ,
          "  note: the size have to be a power of %" PRIu32 "\n" ,
          1 << type
        ) ;
        fclose(fp) ;
        return VOO_IRQ_RESERVED_1 ;
      }

      if (__VOO_ENDIAN != endian) {
        u8_t * data = voo->mem.data + addr ;

        switch (type) {
        case 0x01 : {
          for (u64_t i = 0 ; i < size ; i += 1 << type) {
            u16_t x ;

            voo_read_16(voo, 0, addr + i, &x) ;
#if __VOO_ENDIAN == __VOO_HOST_ENDIAN
            x = voo_bswap_16(x) ;
#endif
            voo_write_16(voo, 0, addr + i, x) ;
          }
        } break ;

        case 0x02 : {
          for (u64_t i = 0 ; i < size ; i += 1 << type) {
            u32_t x ;

            voo_read_32(voo, 0, addr + i, &x) ;
#if __VOO_ENDIAN == __VOO_HOST_ENDIAN
            x = voo_bswap_32(x) ;
#endif
            voo_write_32(voo, 0, addr + i, x) ;
          }
        } break ;

        case 0x03 : {
          for (u64_t i = 0 ; i < size ; i += 1 << type) {
            u64_t x ;

            voo_read_64(voo, 0, addr + i, &x) ;
#if __VOO_ENDIAN == __VOO_HOST_ENDIAN
            x = voo_bswap_64(x) ;
#endif
            voo_write_64(voo, 0, addr + i, x) ;
          }
        } break ;
        }
      }
    } break ;
    
    default : {
      fprintf(stderr, "In section %u:\n", i) ;
      fprintf(stderr, "  error: invalid type 0x" PRIX32 "\n", type) ;
      fclose(fp) ;
      return VOO_IRQ_RESERVED_1 ;
    }
    }
  }

  fclose(fp) ;

  return VOO_N_IRQS ;
}

__VOO_API
u32_t voo_unboot (
  voo_t * voo
)
{
  if (0 != (rFLAGS & VOO_FLAG_1))
    return voo_int(voo, VOO_IRQ_GENERAL_PROTECT) ;

  if (NULL != voo->mem.data)
    free(voo->mem.data) ;

  for (int i = 0 ; i < VOO_N_SEGS ; ++i)
    voo->ker.seg[i] = 0 ;

  for (int i = 0 ; i < VOO_N_REGS ; ++i)
    voo->ker.reg[i] = 0 ;

  voo->opr.seg  = 0    ;
  voo->opr.addr = 0    ;
  voo->opr.data = 0    ;
  voo->mem.size = 0    ;
  voo->mem.data = NULL ;

  return VOO_N_IRQS ;
}

__VOO_API
u32_t voo_int (
  voo_t * voo ,
  u32_t   IRQ
)
{
  if (0 == (rFLAGS & VOO_FLAG_1))
    return VOO_IRQ_RESERVED_1 ;

  if (IRQ <= VOO_N_IRQS)
    return voo_int(voo, VOO_IRQ_UNDEF_INTERRUPT) ;

  if (
    IRQ == VOO_IRQ_NON_MASKABLE || 0 != (rFLAGS & VOO_FLAG_I)
#ifdef __VOO_HAS_OPTIONS
    || 0 != voo->opt.force_IRQ
#endif
  ) {
    if (
      VOO_N_IRQS != voo_read_32(
        voo                                     ,
        voo->ker.seg[VOO_SEG_IVT]               ,
        voo->ker.reg[VOO_REG_IVT]             +
        IRQ * (sizeof(u32_t) + sizeof(u64_t))   ,
        voo->ker.seg + VOO_SEG_ISR
      )
    ) return rIRQ ;

    if (
      VOO_N_IRQS != voo_read_64(
        voo                                     ,
        voo->ker.seg[VOO_SEG_IVT]               ,
        voo->ker.reg[VOO_REG_IVT]             +
        IRQ * (sizeof(u32_t) + sizeof(u64_t)) +
        sizeof(u32_t)                           ,
        voo->ker.reg + VOO_REG_ISR
      )
    ) return rIRQ ;

    if (VOO_N_IRQS != __push_32(voo, rFLAGS))
      return rIRQ ;

    if (VOO_N_IRQS != __push_64(voo, rIP))
      return rIRQ ;

    if (VOO_N_IRQS != __push_32(voo, rCODE))
      return rIRQ ;

    rFLAGS &= ~VOO_FLAG_I ;
    rFLAGS |=  VOO_FLAG_T ;

    // go to the kernel
    rFLAGS &= ~VOO_FLAG_IOPL ;

    rCODE = voo->ker.seg[VOO_SEG_ISR] ;
    rIP   = voo->ker.reg[VOO_REG_ISR] ;

#ifdef __VOO_HAS_OPTIONS
    if (0 != voo->opt.halt_IRQ)
      rFLAGS &= ~VOO_FLAG_1 ;
#endif
  }

  return rIRQ = IRQ ;
}

__VOO_API
u32_t voo_iret (
  voo_t * voo
)
{
  if (0 == (rFLAGS & VOO_FLAG_T))
    return voo_int(voo, VOO_IRQ_UNDEF_INTERRUPT) ;

  if (VOO_N_IRQS != __pop_64(voo, &rIP))
    return rIRQ ;

  if (VOO_N_IRQS != __pop_32(voo, &rCODE))
    return rIRQ ;

  if (VOO_N_IRQS != __pop_32(voo, &rFLAGS))
    return rIRQ ;

  return VOO_N_IRQS ;
}

__VOO_API
u32_t voo_write (
  voo_t * voo  ,
  u32_t   seg  ,
  u64_t   addr ,
  u64_t   size ,
  void *  data
)
{
  if (0 != (rFLAGS & VOO_FLAG_V)) {
    desc_t desc ;

    if (VOO_N_IRQS != __vaddr_to_paddr(voo, seg, addr, &desc))
      return rIRQ ;

    if (0 == (desc.perm & PERM_W))
      return voo_int(voo, VOO_IRQ_SEGMENT_ACCESS) ;

    if (desc.size < addr + size)
      size = desc.size - addr ;

    addr += desc.addr ;
  } else {
    if (voo->mem.size <= addr)
      return voo_int(voo, VOO_IRQ_OUT_OF_MEMORY) ;

    if (voo->mem.size < addr + size)
      size = voo->mem.size - addr ;
  }

  memcpy(voo->mem.data + addr, data, size) ;

#ifdef __VOO_HAS_OPTIONS
  if (0 != voo->opt.dump_DATA) {
    fprintf(stderr, "\n>> Write %" PRIu64 " bytes at 0x%" PRIX64 "\n\n", size, addr) ;
    voo_dump(stderr, addr, size, voo->mem.data + addr) ;
  }

  if (0 != voo->opr.stack && 0 != voo->opt.dump_STACK) {
    fprintf(stderr, "\n>> Push %" PRIu64 " bytes onto the stack\n\n", size) ;
    voo_dump(stderr, 0, rBP - rSP, voo->mem.data + addr) ;
  }
#endif

  return VOO_N_IRQS ;
}

__VOO_API
u32_t voo_write_8 (
  voo_t * voo  ,
  u32_t   seg  ,
  u64_t   addr ,
  u8_t    data
)
{
  return voo_write(
    voo          ,
    seg          ,
    addr         ,
    sizeof(data) ,
    &data
  ) ;
}

__VOO_API
u32_t voo_write_16 (
  voo_t * voo  ,
  u32_t   seg  ,
  u64_t   addr ,
  u16_t   data
)
{
#if __VOO_ENDIAN != __VOO_HOST_ENDIAN
  data = voo_bswap_16(data) ;
#endif

  return voo_write(
    voo          ,
    seg          ,
    addr         ,
    sizeof(data) ,
    &data
  ) ;
}

__VOO_API
u32_t voo_write_32 (
  voo_t * voo  ,
  u32_t   seg  ,
  u64_t   addr ,
  u32_t   data
)
{
#if __VOO_ENDIAN != __VOO_HOST_ENDIAN
  data = voo_bswap_32(data) ;
#endif

  return voo_write(
    voo          ,
    seg          ,
    addr         ,
    sizeof(data) ,
    &data
  ) ;
}

__VOO_API
u32_t voo_write_64 (
  voo_t * voo  ,
  u32_t   seg  ,
  u64_t   addr ,
  u64_t   data
)
{
#if __VOO_ENDIAN != __VOO_HOST_ENDIAN
  data = voo_bswap_64(data) ;
#endif

  return voo_write(
    voo          ,
    seg          ,
    addr         ,
    sizeof(data) ,
    &data
  ) ;
}

__VOO_API
u32_t voo_read (
  voo_t * voo  ,
  u32_t   seg  ,
  u64_t   addr ,
  u64_t   size ,
  void *  data
)
{
  memset(data, 0, size) ;

  if (0 != (rFLAGS & VOO_FLAG_V)) {
    desc_t desc ;

    if (VOO_N_IRQS != __vaddr_to_paddr(voo, seg, addr, &desc))
      return rIRQ ;

    if (0 == (desc.perm & PERM_R))
      return voo_int(voo, VOO_IRQ_SEGMENT_ACCESS) ;

    if (desc.size < addr + size)
      size = desc.size - addr ;

    addr += desc.addr ;
  } else {
    if (voo->mem.size <= addr)
      return voo_int(voo, VOO_IRQ_OUT_OF_MEMORY) ;

    if (voo->mem.size < addr + size)
      size = voo->mem.size - addr ;
  }

  memcpy(data, voo->mem.data + addr, size) ;

#ifdef __VOO_HAS_OPTIONS
  if (0 != voo->opt.dump_DATA) {
    fprintf(stderr, "\n>> Read %" PRIu64 " bytes at 0x%" PRIX64 "\n\n", size, addr) ;
    voo_dump(stderr, 0, size, data) ;
  }

  if (0 != voo->opr.stack && 0 != voo->opt.dump_STACK) {
    fprintf(stderr, "\n>> Pop %" PRIu64 " bytes from the stack\n\n", size) ;
    voo_dump(stderr, 0, rBP - rSP - size, voo->mem.data + addr) ;
  }
#endif

  return VOO_N_IRQS ;
}

__VOO_API
u32_t voo_read_8 (
  voo_t * voo  ,
  u32_t   seg  ,
  u64_t   addr ,
  u8_t *  data
)
{
  return voo_read(
    voo           ,
    seg           ,
    addr          ,
    sizeof(*data) ,
    data
  ) ;
}

__VOO_API
u32_t voo_read_16 (
  voo_t * voo  ,
  u32_t   seg  ,
  u64_t   addr ,
  u16_t * data
)
{
  if (
    VOO_N_IRQS != voo_read(
      voo           ,
      seg           ,
      addr          ,
      sizeof(*data) ,
      data
    )
  ) return rIRQ ;

#if __VOO_ENDIAN != __VOO_HOST_ENDIAN
  *data = voo_bswap_16(*data) ;
#endif

  return VOO_N_IRQS ;
}

__VOO_API
u32_t voo_read_32 (
  voo_t * voo  ,
  u32_t   seg  ,
  u64_t   addr ,
  u32_t * data
)
{
  if (
    VOO_N_IRQS != voo_read(
      voo           ,
      seg           ,
      addr          ,
      sizeof(*data) ,
      data
    )
  ) return rIRQ ;

#if __VOO_ENDIAN != __VOO_HOST_ENDIAN
  *data = voo_bswap_32(*data) ;
#endif

  return VOO_N_IRQS ;
}

__VOO_API
u32_t voo_read_64 (
  voo_t * voo  ,
  u32_t   seg  ,
  u64_t   addr ,
  u64_t * data
)
{
  if (
    VOO_N_IRQS != voo_read(
      voo           ,
      seg           ,
      addr          ,
      sizeof(*data) ,
      data
    )
  ) return rIRQ ;

#if __VOO_ENDIAN != __VOO_HOST_ENDIAN
  *data = voo_bswap_64(*data) ;
#endif

  return VOO_N_IRQS ;
}

#define O   ((voo->opr.data >> 24) & 0x000000FF)
#define A   ((voo->opr.data >> 20) & 0x0000000F)
#define B   ((voo->opr.data >> 16) & 0x0000000F)
#define C   ((voo->opr.data >> 12) & 0x0000000F)
#define I24 ((voo->opr.data >>  0) & 0x00FFFFFF)
#define I20 ((voo->opr.data >>  0) & 0x000FFFFF)
#define I16 ((voo->opr.data >>  0) & 0x0000FFFF)
#define I12 ((voo->opr.data >>  0) & 0x00000FFF)

static u64_t __c_mask [] = {
  UINT64_C(0x00000000000000FF) ,
  UINT64_C(0x000000000000FFFF) ,
  UINT64_C(0x00000000FFFFFFFF) ,
  UINT64_C(0xFFFFFFFFFFFFFFFF)
} ;

static u64_t __c_imask [] = {
  UINT64_C(0xFFFFFFFFFFFFFF00) ,
  UINT64_C(0xFFFFFFFFFFFF0000) ,
  UINT64_C(0xFFFFFFFF00000000) ,
  UINT64_C(0x0000000000000000)
} ;

static u64_t __c_sign [] = {
  UINT64_C(0x0000000000000080) ,
  UINT64_C(0x0000000000008000) ,
  UINT64_C(0x0000000080000000) ,
  UINT64_C(0x8000000000000000)
} ;

static u64_t __c_isign [] = {
  UINT64_C(0x000000000000007F) ,
  UINT64_C(0x0000000000007FFF) ,
  UINT64_C(0x000000007FFFFFFF) ,
  UINT64_C(0x7FFFFFFFFFFFFFFF)
} ;

static u64_t __c_bits [] = {
   8 ,
  16 ,
  32 ,
  64
} ;

__VOO_STATIC
u64_t __reg_get (
  voo_t * voo  ,
  u32_t   regi ,
  u32_t   size
)
{
  return voo->ker.reg[regi] & __c_mask[size] ;
}

__VOO_STATIC
void __reg_set (
  voo_t * voo  ,
  u32_t   regi ,
  u32_t   size ,
  u64_t   data
)
{
  voo->ker.reg[regi] =
    (voo->ker.reg[regi] & __c_imask[size]) |
    (data & __c_mask[size])
  ;
}

__VOO_STATIC
u64_t __sext (
  u32_t data ,
  u32_t size
)
{
  u64_t x = data ;

  return
    (~((x >> size) - 1) << size) |
    (x & ((1 << size) - 1))
  ;
}

#define __sext_8(__data)  __sext((__data),  7)
#define __sext_12(__data) __sext((__data), 11)
#define __sext_16(__data) __sext((__data), 15)
#define __sext_20(__data) __sext((__data), 19)
#define __sext_24(__data) __sext((__data), 23)
#define __sext_32(__data) __sext((__data), 31)

__VOO_STATIC
u32_t __cond_is_set (
  voo_t * voo  ,
  u32_t   cond
)
{
#define _SO(__op)        \
  (                      \
    ((rFLAGS >>  7) & 1) \
      __op               \
    ((rFLAGS >> 11) & 1) \
  )

  switch (cond) {
  case 0x0 : return 0 != (rFLAGS & VOO_FLAG_C) ;
  case 0x1 : return 0 == (rFLAGS & VOO_FLAG_C) ;
  case 0x2 : return 0 != (rFLAGS & VOO_FLAG_P) ;
  case 0x3 : return 0 == (rFLAGS & VOO_FLAG_P) ;
  case 0x4 : return 0 != (rFLAGS & VOO_FLAG_Z) ;
  case 0x5 : return 0 == (rFLAGS & VOO_FLAG_Z) ;
  case 0x6 : return 0 != (rFLAGS & VOO_FLAG_S) ;
  case 0x7 : return 0 == (rFLAGS & VOO_FLAG_S) ;
  case 0x8 : return 0 != (rFLAGS & VOO_FLAG_O) ;
  case 0x9 : return 0 == (rFLAGS & VOO_FLAG_O) ;
  case 0xA : return 0 == (rFLAGS & (VOO_FLAG_C | VOO_FLAG_Z)) ;
  case 0xB : return 0 != (rFLAGS & (VOO_FLAG_C | VOO_FLAG_Z)) ;
  case 0xC : return 0 == (rFLAGS & VOO_FLAG_Z) && _SO(==) ;
  case 0xD : return 0 != (rFLAGS & VOO_FLAG_Z) || _SO(!=) ;
  case 0xE : return _SO(==) ;
  case 0xF : return _SO(!=) ;
  default  : return 0 ;
  }

#undef _SO
}

__VOO_STATIC
void __set_flags_mov (
  voo_t * voo   ,
  u32_t  size ,
  u64_t  r0
)
{
  rFLAGS &= ~(VOO_FLAG_C | VOO_FLAG_P | VOO_FLAG_Z | VOO_FLAG_S | VOO_FLAG_O) ;

  u32_t s0 = !!(r0 & __c_sign[size]) ;

  rFLAGS |= (0 == (r0 & 1)) * VOO_FLAG_P ;
  rFLAGS |= (0 == r0)       * VOO_FLAG_Z ;
  rFLAGS |= s0              * VOO_FLAG_S ;
}

__VOO_STATIC
void __set_flags_add (
  voo_t * voo  ,
  u32_t   size ,
  u64_t   r0   ,
  u64_t   r1   ,
  u64_t   r2
)
{
  rFLAGS &= ~(VOO_FLAG_C | VOO_FLAG_P | VOO_FLAG_Z | VOO_FLAG_S | VOO_FLAG_O) ;

  u32_t s0 = !!(r0 & __c_sign[size]) ;
  u32_t s1 = !!(r1 & __c_sign[size]) ;
  u32_t s2 = !!(r2 & __c_sign[size]) ;

  rFLAGS |= (0 != (r2 >> __c_bits[size])) * VOO_FLAG_C ;
  rFLAGS |= (0 == (r2 & 1))               * VOO_FLAG_P ;
  rFLAGS |= (0 == r2)                     * VOO_FLAG_Z ;
  rFLAGS |= s2                            * VOO_FLAG_S ;
  rFLAGS |= (!(s0 ^ s1) && s0 ^ s2)       * VOO_FLAG_O ;
}

__VOO_STATIC
void __set_flags_sub (
  voo_t * voo  ,
  u32_t   size ,
  u64_t   r0   ,
  u64_t   r1   ,
  u64_t   r2
)
{
  rFLAGS &= ~(VOO_FLAG_C | VOO_FLAG_P | VOO_FLAG_Z | VOO_FLAG_S | VOO_FLAG_O) ;

  u32_t s0 = !!(r0 & __c_sign[size]) ;
  u32_t s1 = !!(r1 & __c_sign[size]) ;
  u32_t s2 = !!(r2 & __c_sign[size]) ;

  rFLAGS |= (0 != (r2 >> __c_bits[size])) * VOO_FLAG_C ;
  rFLAGS |= (0 == (r2 & 1))               * VOO_FLAG_P ;
  rFLAGS |= (0 == r2)                     * VOO_FLAG_Z ;
  rFLAGS |= s2                            * VOO_FLAG_S ;
  rFLAGS |= (s0 ^ s1 && s0 ^ s2)          * VOO_FLAG_O ;
}

__VOO_STATIC
void __set_flags_mul_div (
  voo_t * voo  ,
  u32_t   size ,
  u64_t   r0   ,
  u64_t   r1   ,
  u64_t   r2
)
{  
  rFLAGS &= ~(VOO_FLAG_C | VOO_FLAG_P | VOO_FLAG_Z | VOO_FLAG_S | VOO_FLAG_O) ;

  u32_t s2 = !!(r2 & __c_sign[size]) ;

  rFLAGS |= (0 != (r2 >> __c_bits[size])) * (VOO_FLAG_C | VOO_FLAG_O) ;
  rFLAGS |= (0 == (r2 & 1))               * VOO_FLAG_P ;
  rFLAGS |= (0 == r2)                     * VOO_FLAG_Z ;
  rFLAGS |= s2                            * VOO_FLAG_S ;
}

__VOO_STATIC
void __set_flags_imul_idiv (
  voo_t * voo  ,
  u32_t   size ,
  i64_t   r0   ,
  i64_t   r1   ,
  i64_t   r2
)
{
  rFLAGS &= ~(VOO_FLAG_C | VOO_FLAG_P | VOO_FLAG_Z | VOO_FLAG_S | VOO_FLAG_O) ;

  u32_t s2 = !!(r2 & __c_sign[size]) ;

  rFLAGS |= (-1 != (r2 >> __c_bits[size])) * (VOO_FLAG_C | VOO_FLAG_O) ;
  rFLAGS |= (0 == (r2 & 1))                * VOO_FLAG_P ;
  rFLAGS |= (0 == r2)                      * VOO_FLAG_Z ;
  rFLAGS |= s2                             * VOO_FLAG_S ;
}

__VOO_STATIC
void __set_flags_and_or_xor (
  voo_t * voo  ,
  u32_t   size ,
  u64_t   r0   ,
  u64_t   r1   ,
  u64_t   r2
)
{
  rFLAGS &= ~(VOO_FLAG_C | VOO_FLAG_P | VOO_FLAG_Z | VOO_FLAG_S | VOO_FLAG_O) ;

  u32_t s2 = !!(r2 & __c_sign[size]) ;

  rFLAGS |= (0 == (r2 & 1)) * VOO_FLAG_P ;
  rFLAGS |= (0 == r2)       * VOO_FLAG_Z ;
  rFLAGS |= s2              * VOO_FLAG_S ;
}

__VOO_STATIC
void __set_flags_shl (
  voo_t * voo  ,
  u32_t   size ,
  u64_t   r0   ,
  u64_t   r1   ,
  u64_t   r2
)
{
  rFLAGS &= ~(VOO_FLAG_C | VOO_FLAG_P | VOO_FLAG_Z | VOO_FLAG_S | VOO_FLAG_O) ;

  u32_t s2 = !!(r2 & __c_sign[size]) ;

  rFLAGS |= ((r0 >> (__c_bits[size] - r1)) & 1) * VOO_FLAG_C ;
  rFLAGS |= (0 == (r2 & 1))                     * VOO_FLAG_P ;
  rFLAGS |= (0 == r2)                           * VOO_FLAG_Z ;
  rFLAGS |= s2                                  * VOO_FLAG_S ;
  
  if (1 == r1)
    rFLAGS |= (
      0 !=
        ((r0 >> (__c_bits[size] - 1)) & 1) ^
        ((r0 >> (__c_bits[size] - 2)) & 1)
    ) * VOO_FLAG_O ;
}

__VOO_STATIC
void __set_flags_shr (
  voo_t * voo  ,
  u32_t   size ,
  u64_t   r0   ,
  u64_t   r1   ,
  u64_t   r2
)
{
  rFLAGS &= ~(VOO_FLAG_C | VOO_FLAG_P | VOO_FLAG_Z | VOO_FLAG_S | VOO_FLAG_O) ;

  u32_t s2 = !!(r2 & __c_sign[size]) ;

  rFLAGS |= ((r0 >> (r1 - 1)) & 1) * VOO_FLAG_C ;
  rFLAGS |= (0 == (r2 & 1))        * VOO_FLAG_P ;
  rFLAGS |= (0 == r2)              * VOO_FLAG_Z ;
  rFLAGS |= s2                     * VOO_FLAG_S ;
  
  if (1 == r1)
    rFLAGS |= (
      0 !=
        ((r0 >> (__c_bits[size] - 1)) & 1)
    ) * VOO_FLAG_O ;
}

__VOO_STATIC
void __set_flags_sar (
  voo_t * voo  ,
  u32_t   size ,
  i64_t   r0   ,
  u64_t   r1   ,
  i64_t   r2
)
{
  rFLAGS &= ~(VOO_FLAG_C | VOO_FLAG_P | VOO_FLAG_Z | VOO_FLAG_S | VOO_FLAG_O) ;

  u32_t s2 = !!(r2 & __c_sign[size]) ;

  rFLAGS |= ((r0 >> (r1 - 1)) & 1) * VOO_FLAG_C ;
  rFLAGS |= (0 == (r2 & 1))        * VOO_FLAG_P ;
  rFLAGS |= (0 == r2)              * VOO_FLAG_Z ;
  rFLAGS |= s2                     * VOO_FLAG_S ;
  
  if (1 == r1)
    rFLAGS |= (
      0 !=
        ((r0 >> (__c_bits[size] - 1)) & 1)
    ) * VOO_FLAG_O ;
}

__VOO_STATIC
u32_t __clock (
  voo_t * voo
)
{
  // fetch the next instruction

  if (0 != (rFLAGS & VOO_FLAG_V)) {
    if (0 == ((rCODE >> 24) & 1))
      return voo_int(voo, VOO_IRQ_SEGMENT_ACCESS) ;
  }
  
  if (
    VOO_N_IRQS != voo_read_32(
      voo            ,
      voo->opr.seg   ,
      voo->opr.addr  ,
      &voo->opr.data
    )
  ) return rIRQ ;

  rIP += sizeof(voo->opr.data) ;

#ifdef __VOO_HAS_OPTIONS
  if (0 != voo->opt.dump_CODE) {
    fprintf(stderr, "\n>> Instruction\n\n") ;
    voo_dump(stderr, 0, sizeof(voo->opr.data), &voo->opr.data) ;
  }
#endif

  // decode and execute the fetched instruction

  u32_t code , size ;
  u64_t r0 , r1 , r2 , r3 ;

#define S(__code) size = (code - (__code)) & 3

  code = O ;

  switch (O) {
  case 0x00 :   // add byte r(a), r(b), i
  case 0x01 :   // add half r(a), r(b), i
  case 0x02 :   // add word r(a), r(b), i
  case 0x03 : { // add long r(a), r(b), i
    S(0x00) ;

    r0 = __reg_get(voo, A, size) ;
    r1 = __reg_get(voo, B, size) ;
    r2 = __sext_16(I16) ;

    r3 = r0 + r1 + r2 ;

    __reg_set(voo, A, size, r3) ;
    __set_flags_add(voo, size, r0, r1 + r2, r3) ;
  } break ;

  case 0x04 :   // sub byte r(a), r(b), i
  case 0x05 :   // sub half r(a), r(b), i
  case 0x06 :   // sub word r(a), r(b), i
  case 0x07 : { // sub long r(a), r(b), i
    S(0x04) ;

    r0 = __reg_get(voo, A, size) ;
    r1 = __reg_get(voo, B, size) ;
    r2 = __sext_16(I16) ;

    r3 = r0 - r1 - r2 ;

    __reg_set(voo, A, size, r3) ;
    __set_flags_sub(voo, size, r0, r1 + r2, r3) ;
  } break ;

  case 0x08 : { // xhg s(a), s(b)
    r0 = *s(A) ;
    *s(A) = *s(B) ;
    *s(B) = (u32_t)r0 ;
  } break ;

  case 0x09 : { // mov s(a), s(b)
    *s(A) = *s(B) ;
    __set_flags_mov(voo, 2, *s(A)) ;
  } break ;

  case 0x0A : { // mov s(a), r(b)
    *s(A) = *r32(B) ;
    __set_flags_mov(voo, 2, *s(A)) ;
  } break ;

  case 0x0B : { // mov r(a), s(b)
    *r32(A) = *s(B) ;
    __set_flags_mov(voo, 2, *r32(A)) ;
  } break ;

  case 0x0C : { // psh s(a)
    __push_32(voo, *s(A)) ;
  } break ;

  case 0x0D : { // pop s(a)
    __pop_32(voo, s(A)) ;
  } break ;

  case 0x0E : { // nop

  } break ;

  case 0x0F : { // hlt
    rFLAGS &= ~VOO_FLAG_1 ;
  } break ;

  case 0x10 :   // not byte r(a), i
  case 0x11 :   // not half r(a), i
  case 0x12 :   // not word r(a), i
  case 0x13 : { // not long r(a), i
    S(0x10) ;

    r0 = __reg_get(voo, A, size) ;
    r2 = __sext_20(I20) ;
    r3 = ~r0 + r2 ;

    __reg_set(voo, A, size, r3) ;
    __set_flags_add(voo, size, r0, r2, r3) ;
  } break ;

  case 0x14 :   // neg byte r(a), i
  case 0x15 :   // neg half r(a), i
  case 0x16 :   // neg word r(a), i
  case 0x17 : { // neg long r(a), i
    S(0x14) ;

    r0 = __reg_get(voo, A, size) ;
    r2 = __sext_20(I20) ;
    r3 = -r0 + r2 ;

    __reg_set(voo, A, size, r3) ;
    __set_flags_add(voo, size, r0, r2, r3) ;
  } break ;

  case 0x18 :   // adc byte r(a), r(b), i
  case 0x19 :   // adc half r(a), r(b), i
  case 0x1A :   // adc word r(a), r(b), i
  case 0x1B : { // adc long r(a), r(b), i
    S(0x18) ;

    r0 = __reg_get(voo, A, size) ;
    r1 = __reg_get(voo, B, size) ;
    r2 = __sext_16(I16) + !!(rFLAGS & VOO_FLAG_C) ;

    r3 = r0 + r1 + r2 ;

    __reg_set(voo, A, size, r3) ;
    __set_flags_add(voo, size, r0, r1 + r2, r3) ;
  } break ;

  case 0x1C :   // sbb byte r(a), r(b), i
  case 0x1D :   // sbb half r(a), r(b), i
  case 0x1E :   // sbb word r(a), r(b), i
  case 0x1F : { // sbb long r(a), r(b), i
    S(0x1C) ;

    r0 = __reg_get(voo, A, size) ;
    r1 = __reg_get(voo, B, size) ;
    r2 = __sext_16(I16) + !!(rFLAGS & VOO_FLAG_C) ;

    r3 = r0 - r1 - r2 ;

    __reg_set(voo, A, size, r3) ;
    __set_flags_sub(voo, size, r0, r1 + r2, r3) ;
  } break ;

  case 0x20 :   // and byte r(a), r(b)
  case 0x21 :   // and half r(a), r(b)
  case 0x22 :   // and word r(a), r(b)
  case 0x23 : { // and long r(a), r(b)
    S(0x20) ;

    r0 = __reg_get(voo, A, size) ;
    r1 = __reg_get(voo, B, size) ;

    r3 = r0 & r1 ;

    __reg_set(voo, A, size, r3) ;
    __set_flags_and_or_xor(voo, size, r0, r1, r3) ;
  } break ;

  case 0x24 :   // or byte r(a), r(b)
  case 0x25 :   // or half r(a), r(b)
  case 0x26 :   // or word r(a), r(b)
  case 0x27 : { // or long r(a), r(b)
    S(0x24) ;

    r0 = __reg_get(voo, A, size) ;
    r1 = __reg_get(voo, B, size) ;

    r3 = r0 | r1 ;

    __reg_set(voo, A, size, r3) ;
    __set_flags_and_or_xor(voo, size, r0, r1, r3) ;
  } break ;

  case 0x28 :   // xor byte r(a), r(b)
  case 0x29 :   // xor half r(a), r(b)
  case 0x2A :   // xor word r(a), r(b)
  case 0x2B : { // xor long r(a), r(b)
    S(0x28) ;

    r0 = __reg_get(voo, A, size) ;
    r1 = __reg_get(voo, B, size) ;

    r3 = r0 ^ r1 ;

    __reg_set(voo, A, size, r3) ;
    __set_flags_and_or_xor(voo, size, r0, r1, r3) ;
  } break ;

  case 0x2C :   // shl/sal byte r(a), r(b), i
  case 0x2D :   // shl/sal half r(a), r(b), i
  case 0x2E :   // shl/sal word r(a), r(b), i
  case 0x2F : { // shl/sal long r(a), r(b), i
    S(0x2C) ;

    r0 = __reg_get(voo, A, size) ;
    r1 = __reg_get(voo, B, size) ;
    r2 = __sext_16(I16) ;

    r3 = r0 << (r1 + r2) ;

    __reg_set(voo, A, size, r3) ;
    __set_flags_shl(voo, size, r0, r1 + r2, r3) ;
  } break ;

  case 0x30 :   // shr byte r(a), r(b), i
  case 0x31 :   // shr half r(a), r(b), i
  case 0x32 :   // shr word r(a), r(b), i
  case 0x33 : { // shr long r(a), r(b), i
    S(0x30) ;

    r0 = __reg_get(voo, A, size) ;
    r1 = __reg_get(voo, B, size) ;
    r2 = __sext_16(I16) ;

    r3 = r0 >> (r1 + r2) ;

    __reg_set(voo, A, size, r3) ;
    __set_flags_shr(voo, size, r0, r1 + r2, r3) ;
  } break ;

  case 0x34 :   // sar byte r(a), r(b), i
  case 0x35 :   // sar half r(a), r(b), i
  case 0x36 :   // sar word r(a), r(b), i
  case 0x37 : { // sar long r(a), r(b), i
    S(0x34) ;

    r0 = __sext(__reg_get(voo, A, size), (1 << size) - 1) ;
    r1 = __reg_get(voo, B, size) ;
    r2 = __sext_16(I16) ;

    r3 = (i64_t)r0 >> (r1 + r2) ;

    __reg_set(voo, A, size, r3) ;
    __set_flags_sar(voo, size, r0, r1 + r2, r3) ;
  } break ;

  case 0x38 :   // tst byte r(a), r(b)
  case 0x39 :   // tst half r(a), r(b)
  case 0x3A :   // tst word r(a), r(b)
  case 0x3B : { // tst long r(a), r(b)
    S(0x38) ;

    r0 = __reg_get(voo, A, size) ;
    r1 = __reg_get(voo, B, size) ;

    r3 = r0 & r1 ;

    __set_flags_and_or_xor(voo, size, r0, r1, r3) ;
  } break ;

  case 0x3C :   // cmp byte r(a), r(b)
  case 0x3D :   // cmp half r(a), r(b)
  case 0x3E :   // cmp word r(a), r(b)
  case 0x3F : { // cmp long r(a), r(b)
    S(0x3C) ;

    r0 = __reg_get(voo, A, size) ;
    r1 = __reg_get(voo, B, size) ;

    r3 = r0 - r1 ;

    __set_flags_sub(voo, size, r0, r1, r3) ;
  } break ;

  case 0x40 :   // tst byte r(a), i
  case 0x41 :   // tst half r(a), i
  case 0x42 :   // tst word r(a), i
  case 0x43 : { // tst long r(a), i
    S(0x40) ;

    r0 = __reg_get(voo, A, size) ;
    r2 = I20 ;

    r3 = r0 & r2 ;

    __set_flags_and_or_xor(voo, size, r0, r2, r3) ;
  } break ;

  case 0x44 :   // tst byte r(a), sext i
  case 0x45 :   // tst half r(a), sext i
  case 0x46 :   // tst word r(a), sext i
  case 0x47 : { // tst long r(a), sext i
    S(0x44) ;

    r0 = __reg_get(voo, A, size) ;
    r2 = __sext_20(I20) ;

    r3 = r0 & r2 ;

    __set_flags_and_or_xor(voo, size, r0, r2, r3) ;
  } break ;

  case 0x48 :   // cmp byte r(a), i
  case 0x49 :   // cmp half r(a), i
  case 0x4A :   // cmp word r(a), i
  case 0x4B : { // cmp long r(a), i
    S(0x48) ;

    r0 = __reg_get(voo, A, size) ;
    r2 = I20 ;

    r3 = r0 - r2 ;

    __set_flags_sub(voo, size, r0, r2, r3) ;
  } break ;

  case 0x4C :   // cmp byte r(a), sext i
  case 0x4D :   // cmp half r(a), sext i
  case 0x4E :   // cmp word r(a), sext i
  case 0x4F : { // cmp long r(a), sext i
    S(0x4C) ;

    r0 = __reg_get(voo, A, size) ;
    r2 = __sext_20(I20) ;

    r3 = r0 - r2 ;

    __set_flags_sub(voo, size, r0, r2, r3) ;
  } break ;

  case 0x50 :   // xhg byte r(a), r(b)
  case 0x51 :   // xhg half r(a), r(b)
  case 0x52 :   // xhg word r(a), r(b)
  case 0x53 : { // xhg long r(a), r(b)
    S(0x50) ;

    r0 = __reg_get(voo, A, size) ;
    r1 = __reg_get(voo, B, size) ;

    __reg_set(voo, A, size, r1) ;
    __reg_set(voo, B, size, r0) ;
  } break ;

  case 0x54 :   // mov byte r(a), r(b)
  case 0x55 :   // mov half r(a), r(b)
  case 0x56 :   // mov word r(a), r(b)
  case 0x57 : { // mov long r(a), r(b)
    S(0x54) ;

    r0 = __reg_get(voo, B, size) ;

    __reg_set(voo, A, size, r0) ;
    __set_flags_mov(voo, size, r0) ;
  } break ;

  case 0x58 : { // mov half r(a), byte r(b)
    r0 = __sext_8(__reg_get(voo, B, 0)) ;

    __reg_set(voo, A, 1, r0) ;
    __set_flags_mov(voo, 1, r0) ;
  } break ;

  case 0x59 : { // mov word r(a), byte r(b)
    r0 = __sext_8(__reg_get(voo, B, 0)) ;

    __reg_set(voo, A, 2, r0) ;
    __set_flags_mov(voo, 2, r0) ;
  } break ;

  case 0x5A : { // mov word r(a), half r(b)
    r0 = __sext_16(__reg_get(voo, B, 1)) ;

    __reg_set(voo, A, 2, r0) ;
    __set_flags_mov(voo, 2, r0) ;
  } break ;

  case 0x5B : { // mov long r(a), byte r(b)
    r0 = __sext_8(__reg_get(voo, B, 0)) ;

    __reg_set(voo, A, 3, r0) ;
    __set_flags_mov(voo, 3, r0) ;
  } break ;

  case 0x5C : { // mov long r(a), half r(b)
    r0 = __sext_8(__reg_get(voo, B, 1)) ;

    __reg_set(voo, A, 3, r0) ;
    __set_flags_mov(voo, 3, r0) ;
  } break ;

  case 0x5D : { // mov long r(a), word r(b)
    r0 = __sext_8(__reg_get(voo, B, 2)) ;

    __reg_set(voo, A, 3, r0) ;
    __set_flags_mov(voo, 3, r0) ;
  } break ;

  case 0x5E : // int i
    return voo_int(voo, I24 & 0xFF) ;

  case 0x5F : // iret
    return voo_iret(voo) ;

  case 0x60 : { // cl{i}
    r0 = I24 ;

    if (
      0 != (r0 & (VOO_FLAG_1 | VOO_FLAG_I | VOO_FLAG_V | VOO_FLAG_IOPL)) &&
      0 != ((rFLAGS >> 12) & 3)
    ) return voo_int(voo, VOO_IRQ_GENERAL_PROTECT) ;

    rFLAGS &= ~r0 ;
  } break ;

  case 0x61 : { // st{i}
    r0 = I24 ;

    if (
      0 != (r0 & (VOO_FLAG_1 | VOO_FLAG_I | VOO_FLAG_V | VOO_FLAG_IOPL)) &&
      0 != ((rFLAGS >> 12) & 3)
    ) return voo_int(voo, VOO_IRQ_GENERAL_PROTECT) ;

    rFLAGS |= r0 ;
  } break ;

  case 0x62 : { // set{b} r(a)
    r0 = __cond_is_set(voo, B) ;

    __reg_set(voo, A, 2, r0) ;
    __set_flags_mov(voo, 2, r0) ;
  } break ;

  case 0x63 : { // lea r(a), [s(c):r(b) + i]
    if (VOO_REG_BP != B) {
      r1 = *r64(B) ;
      r2 = __sext_12(I12) ;
    } else {
      r1 = 0 ;
      r2 = I12 ;
    }

    r0 = r1 + r2 ;

    __reg_set(voo, A, size, r0) ;
    __set_flags_mov(voo, size, r0) ;
  } break ;

  case 0x64 : { // jmp s(b):r(a) + i
    rCODE = *s(B) ;
    rIP   = (*r64(A) + I16) << 2 ;
  } break ;

  case 0x65 : { // jmp r(a) + i
    rIP += (*r64(A) + __sext_20(I20)) << 2 ;
  } break ;

  case 0x66 : { // call s(b):r(a) + i
    if (VOO_N_IRQS != __push_32(voo, rCODE))
      return rIRQ ;

    if (VOO_N_IRQS != __push_64(voo, rIP))
      return rIRQ ;

    rCODE = *s(B) ;
    rIP   = (*r64(A) + I16) << 2 ;
  } break ;

  case 0x67 : { // call r(a) + i
    if (VOO_N_IRQS != __push_64(voo, rIP))
      return rIRQ ;

    rIP += (*r64(A) + __sext_20(I20)) << 2 ;
  } break ;

  case 0x68 : { // ret r(a) + i (abs)
    rSP -= *r64(A) + I20 ;

    if (VOO_N_IRQS != __pop_64(voo, &rIP))
      return rIRQ ;

    if (VOO_N_IRQS != __pop_32(voo, &rCODE))
      return rIRQ ;
  } break ;

  case 0x69 : { // ret r(a) + i (rel)
    rSP -= *r64(A) + I20 ;

    if (VOO_N_IRQS != __pop_64(voo, &rIP))
      return rIRQ ;
  } break ;

  case 0x6A : { // j{c} s(b):r(a) + i
    if (0 != __cond_is_set(voo, C)) {
      rCODE = *s(B) ;
      rIP   = (*r64(A) + I12) << 2 ;
    }
  } break ;

  case 0x6B : { // j{b} r(a) + i
    if (0 != __cond_is_set(voo, B))
      rIP += (*r64(A) + __sext_16(I16)) << 2 ;
  } break ;

  case 0x6C :   // pop byte r(a), i
  case 0x6D :   // pop half r(a), i
  case 0x6E :   // pop word r(a), i
  case 0x6F : { // pop long r(a), i
    S(0x6C) ;

    for (u32_t i = 0 ; i < I20 ; ++i) {
      switch (size) {
      case 0 : r1 = __pop_8(voo,  (u8_t  *)&r0) ; break ;
      case 1 : r1 = __pop_16(voo, (u16_t *)&r0) ; break ;
      case 2 : r1 = __pop_32(voo, (u32_t *)&r0) ; break ;
      case 3 : r1 = __pop_64(voo, (u64_t *)&r0) ; break ;
      }

      if (VOO_N_IRQS != r1)
        return rIRQ ;
    }

    __reg_set(voo, A, size, r0) ;
    __set_flags_mov(voo, size, r0) ;
  } break ;

  case 0x70 :   // psh byte r(a), i
  case 0x71 :   // psh half r(a), i
  case 0x72 :   // psh word r(a), i
  case 0x73 : { // psh long r(a), i
    S(0x70) ;

    r0 = __reg_get(voo, A, size) ;

    for (u32_t i = 0 ; i < I20 ; ++i) {
      switch (size) {
      case 0 : r1 = __push_8(voo, (u8_t)r0)   ; break ;
      case 1 : r1 = __push_16(voo, (u16_t)r0) ; break ;
      case 2 : r1 = __push_32(voo, (u32_t)r0) ; break ;
      case 3 : r1 = __push_64(voo, (u64_t)r0) ; break ;
      }

      if (VOO_N_IRQS != r1)
        return rIRQ ;
    }
  } break ;

  case 0x74 :   // psh byte i
  case 0x75 :   // psh half i
  case 0x76 :   // psh word i
  case 0x77 : { // psh long i
    S(0x74) ;

    r0 = I24 ;

    switch (size) {
    case 0 : return __push_8(voo, (u8_t)r0)   ;
    case 1 : return __push_16(voo, (u16_t)r0) ;
    case 2 : return __push_32(voo, (u32_t)r0) ;
    case 3 : return __push_64(voo, (u64_t)r0) ;
    }

    if (VOO_N_IRQS != r1)
      return rIRQ ;
  } break ;

  case 0x78 :   // psh byte sext i
  case 0x79 :   // psh half sext i
  case 0x7A :   // psh word sext i
  case 0x7B : { // psh long sext i
    S(0x78) ;

    r0 = __sext_24(I24) ;

    switch (size) {
    case 0 : return __push_8(voo, (u8_t)r0)   ;
    case 1 : return __push_16(voo, (u16_t)r0) ;
    case 2 : return __push_32(voo, (u32_t)r0) ;
    case 3 : return __push_64(voo, (u64_t)r0) ;
    }
  } break ;

  case 0x7C : // pshf
    return __push_32(voo, rFLAGS) ;

  case 0x7D : // popf
    return __pop_32(voo, &rFLAGS) ;
  
  case 0x7E : { // psh r(16 + a)
    if (8 <= A)
      return voo_int(voo, VOO_IRQ_OUT_OF_MEMORY) ;

    return __push_64(voo, *r64(A + 16)) ;
  }

  case 0x7F : { // pop r(16 + a)
    if (8 <= A)
      return voo_int(voo, VOO_IRQ_OUT_OF_MEMORY) ;
    
    r0 = A + 16 ;

    if (VOO_REG_IP == A || VOO_REG_CLOCK == A)
      return voo_int(voo, VOO_IRQ_GENERAL_PROTECT) ;

    return __pop_64(voo, r64(r0)) ;
  }

  case 0x80 :   // mov byte r(a), i
  case 0x81 :   // mov half r(a), i
  case 0x82 :   // mov word r(a), i
  case 0x83 : { // mov long r(a), i
    S(0x80) ;

    r0 = I20 ;

    __reg_set(voo, A, size, r0) ;
    __set_flags_mov(voo, size, r0) ;
  } break ;

  case 0x84 :   // mov byte r(a), sext i
  case 0x85 :   // mov half r(a), sext i
  case 0x86 :   // mov word r(a), sext i
  case 0x87 : { // mov long r(a), sext i
    S(0x84) ;

    r0 = __sext_20(I20) ;

    __reg_set(voo, A, size, r0) ;
    __set_flags_mov(voo, size, r0) ;
  } break ;

  case 0x88 :   // sto byte [s(c):r(b) + i], r(a)
  case 0x89 :   // sto half [s(c):r(b) + i], r(a)
  case 0x8A :   // sto word [s(c):r(b) + i], r(a)
  case 0x8B : { // sto long [s(c):r(b) + i], r(a)
    S(0x88) ;

    if (VOO_REG_BP != B) {
      r1 = *r64(B) ;
      r2 = __sext_12(I12) ;
    } else {
      r1 = 0 ;
      r2 = I12 ;
    }

    r0 = __reg_get(voo, A, size) ;

    switch (size) {
    case 0 : r3 = voo_write_8(voo,  *s(C), r1 + r2, r0) ; break ;
    case 1 : r3 = voo_write_16(voo, *s(C), r1 + r2, r0) ; break ;
    case 2 : r3 = voo_write_32(voo, *s(C), r1 + r2, r0) ; break ;
    case 3 : r3 = voo_write_64(voo, *s(C), r1 + r2, r0) ; break ;
    }

    if (VOO_N_IRQS != r3)
      return rIRQ ;
  } break ;

  case 0x8C :   // lod byte r(a), [s(c):r(b) + i]
  case 0x8D :   // lod half r(a), [s(c):r(b) + i]
  case 0x8E :   // lod word r(a), [s(c):r(b) + i]
  case 0x8F : { // lod long r(a), [s(c):r(b) + i]
    S(0x8C) ;

    if (VOO_REG_BP != B) {
      r1 = *r64(B) ;
      r2 = __sext_12(I12) ;
    } else {
      r1 = 0 ;
      r2 = I12 ;
    }

    switch (size) {
    case 0 : r3 = voo_read_8(voo,  *s(C), r1 + r2, (u8_t  *)&r0) ; break ;
    case 1 : r3 = voo_read_16(voo, *s(C), r1 + r2, (u16_t *)&r0) ; break ;
    case 2 : r3 = voo_read_32(voo, *s(C), r1 + r2, (u32_t *)&r0) ; break ;
    case 3 : r3 = voo_read_64(voo, *s(C), r1 + r2, (u64_t *)&r0) ; break ;
    }

    if (VOO_N_IRQS != r3)
      return rIRQ ;

    __reg_set(voo, A, size, r0) ;
    __set_flags_mov(voo, size, r0) ;
  } break ;

  case 0x90 :   // mul byte r(2):r(0), r(a)
  case 0x91 :   // mul half r(2):r(0), r(a)
  case 0x92 :   // mul word r(2):r(0), r(a)
  case 0x93 : { // mul long r(2):r(0), r(a)
    S(0x90) ;

    r0 = __reg_get(voo, VOO_REG_AX, size) ;
    r1 = __reg_get(voo, A, size) ;

    r3 = r0 * r1 ;

    *r64(VOO_REG_DX) = r3 >> __c_bits[size] ;
    __reg_set(voo, VOO_REG_AX, size, r3) ;
    __set_flags_mul_div(voo, size, r0, r1, r3) ;
  } break ;

  case 0x94 :   // div byte r(2):r(0), r(a)
  case 0x95 :   // div half r(2):r(0), r(a)
  case 0x96 :   // div word r(2):r(0), r(a)
  case 0x97 : { // div long r(2):r(0), r(a)
    S(0x94) ;

    r0 = __reg_get(voo, VOO_REG_AX, size) ;
    r1 = __reg_get(voo, A, size) ;

    if (0 == r1)
      return voo_int(voo, VOO_IRQ_DIV_BY_ZERO) ;

    r3 = r0 / r1 ;
    r2 = r0 % r1 ;

    *r64(VOO_REG_DX) = r2 ;
    __reg_set(voo, VOO_REG_AX, size, r3) ;
    __set_flags_mul_div(voo, size, r0, r1, r3) ;
  } break ;

  case 0x98 :   // imul byte r(2):r(0), r(a)
  case 0x99 :   // imul half r(2):r(0), r(a)
  case 0x9A :   // imul word r(2):r(0), r(a)
  case 0x9B : { // imul long r(2):r(0), r(a)
    S(0x98) ;

    r0 = __sext(__reg_get(voo, VOO_REG_AX, size), (1 << size) - 1) ;
    r1 = __sext(__reg_get(voo, A, size), (1 << size) - 1) ;

    r3 = (i64_t)r0 * (i64_t)r1 ;

    *r64(VOO_REG_DX) = r3 >> __c_bits[size] ;
    __reg_set(voo, VOO_REG_AX, size, r3) ;
    __set_flags_imul_idiv(voo, size, (i64_t)r0, (i64_t)r1, (i64_t)r3) ;
  } break ;

  case 0x9C :   // idiv byte r(2):r(0), r(a)
  case 0x9D :   // idiv half r(2):r(0), r(a)
  case 0x9E :   // idiv word r(2):r(0), r(a)
  case 0x9F : { // idiv long r(2):r(0), r(a)
    S(0x9C) ;

    r0 = __sext(__reg_get(voo, VOO_REG_AX, size), (1 << size) - 1) ;
    r1 = __sext(__reg_get(voo, A, size), (1 << size) - 1) ;

    if (0 == r1)
      return voo_int(voo, VOO_IRQ_DIV_BY_ZERO) ;

    r3 = (i64_t)r0 / (i64_t)r1 ;
    r2 = (i64_t)r0 % (i64_t)r1 ;

    *r64(VOO_REG_DX) = r2 ;
    __reg_set(voo, VOO_REG_AX, size, r3) ;
    __set_flags_imul_idiv(voo, size, (i64_t)r0, (i64_t)r1, (i64_t)r3) ;
  } break ;

  case 0xA0 : case 0xA1 : case 0xA2 : case 0xA3 :
  case 0xA4 : case 0xA5 : case 0xA6 : case 0xA7 :
  case 0xA8 : case 0xA9 : case 0xAA : case 0xAB :
  case 0xAC : case 0xAD : case 0xAE : case 0xAF :
    return voo_int(voo, VOO_IRQ_UNDEF_INST) ;

  case 0xB0 : { // loop r(a) + i
    if (0 != *r64(VOO_REG_CX)) {
      rIP += (*r64(A) + __sext_20(I20)) << 2 ;
      --*r64(VOO_REG_CX) ;
    }
  } break ;

  case 0xB1 : { // loop{b} r(a) + i
    if (0 != *r64(VOO_REG_CX) && 0 != __cond_is_set(voo, B)) {
      rIP += (*r64(A) + __sext_16(I16)) << 2 ;
      --*r64(VOO_REG_CX) ;
    }
  } break ;

  case 0xB2 : { // enter r(a) + i
    if (VOO_N_IRQS != __push_64(voo, rBP))
      return rIRQ ;
    
    rBP = rSP ;
    rSP -= *r64(A) + __sext_20(I20) ;
  } break ;

  case 0xB3 : { // leave
    rSP = rBP ;

    if (VOO_N_IRQS != __pop_64(voo, &rBP))
      return rIRQ ;
  } break ;

  case 0xB4 : case 0xB5 : case 0xB6 : case 0xB7 :
  case 0xB8 : case 0xB9 : case 0xBA : case 0xBB :
  case 0xBC : case 0xBD : case 0xBE : case 0xBF :
    return voo_int(voo, VOO_IRQ_UNDEF_INST) ;

  case 0xC0 : case 0xC1 : case 0xC2 : case 0xC3 :
  case 0xC4 : case 0xC5 : case 0xC6 : case 0xC7 :
  case 0xC8 : case 0xC9 : case 0xCA : case 0xCB :
  case 0xCC : case 0xCD : case 0xCE : case 0xCF :
    return voo_int(voo, VOO_IRQ_UNDEF_INST) ;

  case 0xD0 : { // out byte i, r(2)
    voo->out_8(voo, I24, *r8(VOO_REG_DX)) ;
  } break ;

  case 0xD1 : { // out half i, r(2)
    voo->out_16(voo, I24, *r16(VOO_REG_DX)) ;
  } break ;

  case 0xD2 : { // out word i, r(2)
    voo->out_32(voo, I24, *r32(VOO_REG_DX)) ;
  } break ;

  case 0xD3 : { // out long i, r(2)
    voo->out_64(voo, I24, *r64(VOO_REG_DX)) ;
  } break ;

  case 0xD4 : { // out byte r(0), r(2)
    voo->out_8(voo, *r32(VOO_REG_AX), *r8(VOO_REG_DX)) ;
  } break ;

  case 0xD5 : { // out half r(0), r(2)
    voo->out_16(voo, *r32(VOO_REG_AX), *r16(VOO_REG_DX)) ;
  } break ;

  case 0xD6 : { // out word r(0), r(2)
    voo->out_32(voo, *r32(VOO_REG_AX), *r32(VOO_REG_DX)) ;
  } break ;

  case 0xD7 : { // out long r(0), r(2)
    voo->out_64(voo, *r32(VOO_REG_AX), *r64(VOO_REG_DX)) ;
  } break ;

  case 0xD8 : { // in byte i, r(2)
    voo->in_8(voo, I24, r8(VOO_REG_DX)) ;
  } break ;

  case 0xD9 : { // in half i, r(2)
    voo->in_16(voo, I24, r16(VOO_REG_DX)) ;
  } break ;

  case 0xDA : { // in word i, r(2)
    voo->in_32(voo, I24, r32(VOO_REG_DX)) ;
  } break ;

  case 0xDB : { // in long i, r(2)
    voo->in_64(voo, I24, r64(VOO_REG_DX)) ;
  } break ;

  case 0xDC : { // in byte r(0), r(2)
    voo->in_8(voo, *r32(VOO_REG_AX), r8(VOO_REG_DX)) ;
  } break ;

  case 0xDD : { // in half r(0), r(2)
    voo->in_16(voo, *r32(VOO_REG_AX), r16(VOO_REG_DX)) ;
  } break ;

  case 0xDE : { // in word r(0), r(2)
    voo->in_32(voo, *r32(VOO_REG_AX), r32(VOO_REG_DX)) ;
  } break ;

  case 0xDF : { // in long r(0), r(2)
    voo->in_64(voo, *r32(VOO_REG_AX), r64(VOO_REG_DX)) ;
  } break ;

  case 0xE0 : case 0xE1 : case 0xE2 : case 0xE3 :
  case 0xE4 : case 0xE5 : case 0xE6 : case 0xE7 :
  case 0xE8 : case 0xE9 : case 0xEA : case 0xEB :
  case 0xEC : case 0xED : case 0xEE : case 0xEF :
    return voo_int(voo, VOO_IRQ_UNDEF_INST) ;

  case 0xF0 : case 0xF1 : case 0xF2 : case 0xF3 :
  case 0xF4 : case 0xF5 : case 0xF6 : case 0xF7 :
  case 0xF8 : case 0xF9 : case 0xFA : case 0xFB :
  case 0xFC : case 0xFD : case 0xFE : case 0xFF :
    return voo_int(voo, VOO_IRQ_UNDEF_INST) ;

  default :
    return voo_int(voo, VOO_IRQ_GENERAL_PROTECT) ;
  }

  return VOO_N_IRQS ;
}

__VOO_API
u32_t voo_clock (
  voo_t * voo
)
{
#ifdef __VOO_HAS_OPTIONS
  if (voo->opt.cks <= rCLOCK) {
    rFLAGS &= ~VOO_FLAG_1 ;
    return VOO_N_IRQS ;
  }
#endif

  u32_t IRQ ;

  // initialize the operation

  voo->opr.seg  = rCODE ;
  voo->opr.addr = rIP   ;
  voo->opr.data = 0     ;

  // fetch, decode and execute the next instruction
  IRQ = __clock(voo) ;

  // increase the clock counter
  ++rCLOCK ;

#ifdef __VOO_HAS_OPTIONS
  if (0 != voo->opt.dump_STATE) {
    fprintf(
      stderr ,
      "\n"
      ".-----------------------------------------------------.\n"
      "| State                                               |\n"
      "|-----------------------------------------------------|\n"
      "| r%-2u : 0x%016" PRIX64 " | r%-2u : 0x%016" PRIX64 " |\n"
      "| r%-2u : 0x%016" PRIX64 " | r%-2u : 0x%016" PRIX64 " |\n"
      "| r%-2u : 0x%016" PRIX64 " | r%-2u : 0x%016" PRIX64 " |\n"
      "| r%-2u : 0x%016" PRIX64 " | r%-2u : 0x%016" PRIX64 " |\n"
      "| r%-2u : 0x%016" PRIX64 " | r%-2u : 0x%016" PRIX64 " |\n"
      "| r%-2u : 0x%016" PRIX64 " | r%-2u : 0x%016" PRIX64 " |\n"
      "| r%-2u : 0x%016" PRIX64 " | r%-2u : 0x%016" PRIX64 " |\n"
      "| r%-2u : 0x%016" PRIX64 " | r%-2u : 0x%016" PRIX64 " |\n"
      "|-----------------------------------------------------|\n"
      "| r%-2u : 0x%016" PRIX64 " | r%-2u : 0x%016" PRIX64 " |\n"
      "| r%-2u : 0x%016" PRIX64 " | r%-2u : 0x%016" PRIX64 " |\n"
      "| r%-2u : 0x%016" PRIX64 " | r%-2u : 0x%016" PRIX64 " |\n"
      "| r%-2u : 0x%016" PRIX64 " | r%-2u : 0x%016" PRIX64 " |\n"
      "|-----------------------------------------------------|\n"
      "| s%-2u : 0x%08" PRIX32 "         | s%-2u : 0x%08" PRIX32 "         |\n"
      "| s%-2u : 0x%08" PRIX32 "         | s%-2u : 0x%08" PRIX32 "         |\n"
      "| s%-2u : 0x%08" PRIX32 "         | s%-2u : 0x%08" PRIX32 "         |\n"
      "| s%-2u : 0x%08" PRIX32 "         | s%-2u : 0x%08" PRIX32 "         |\n"
      "| s%-2u : 0x%08" PRIX32 "         | s%-2u : 0x%08" PRIX32 "         |\n"
      "| s%-2u : 0x%08" PRIX32 "         | s%-2u : 0x%08" PRIX32 "         |\n"
      "| s%-2u : 0x%08" PRIX32 "         | s%-2u : 0x%08" PRIX32 "         |\n"
      "| s%-2u : 0x%08" PRIX32 "         | s%-2u : 0x%08" PRIX32 "         |\n"
      "|-----------------------------------------------------|\n"
      "| 0x%08" PRIX32 ":%016" PRIX64 " | 0x%08" PRIX32 "            |\n"
      "'-----------------------------------------------------'\n" ,
      0x00 , *r64(0x00) , 0x08 , *r64(0x08) ,
      0x01 , *r64(0x01) , 0x09 , *r64(0x09) ,
      0x02 , *r64(0x02) , 0x0A , *r64(0x0A) ,
      0x03 , *r64(0x03) , 0x0B , *r64(0x0B) ,
      0x04 , *r64(0x04) , 0x0C , *r64(0x0C) ,
      0x05 , *r64(0x05) , 0x0D , *r64(0x0D) ,
      0x06 , *r64(0x06) , 0x0E , *r64(0x0E) ,
      0x07 , *r64(0x07) , 0x0F , *r64(0x0F) ,
      0x10 , *r64(0x10) , 0x14 , *r64(0x14) ,
      0x11 , *r64(0x11) , 0x15 , *r64(0x15) ,
      0x12 , *r64(0x12) , 0x16 , *r64(0x16) ,
      0x13 , *r64(0x13) , 0x17 , *r64(0x17) ,
      0x00 , *s(0x00)   , 0x08 , *s(0x08)   ,
      0x01 , *s(0x01)   , 0x09 , *s(0x09)   ,
      0x02 , *s(0x02)   , 0x0A , *s(0x0A)   ,
      0x03 , *s(0x03)   , 0x0B , *s(0x0B)   ,
      0x04 , *s(0x04)   , 0x0C , *s(0x0C)   ,
      0x05 , *s(0x05)   , 0x0D , *s(0x0D)   ,
      0x06 , *s(0x06)   , 0x0E , *s(0x0E)   ,
      0x07 , *s(0x07)   , 0x0F , *s(0x0F)   ,
      voo->opr.seg  ,
      voo->opr.addr ,
      voo->opr.data
    ) ;
  }
#endif

  return IRQ ;
}

__VOO_API
u32_t voo_dump (
  FILE * file ,
  u64_t  addr ,
  u64_t  size ,
  void * data
)
{
  char * fmt_data [] = {
    "%02"  PRIX64 " |" ,
    "%04"  PRIX64 " |" ,
    "%08"  PRIX64 " |" ,
    "%016" PRIX64 " |"
  } ;

  u32_t fmt_size [] = {
    2  ,
    4  ,
    8  ,
    16
  } ;

  int idx = 3 ;

  if (addr + size <= 0xFF) {
    idx = 0 ;
  } else if (addr + size <= 0xFFFF) {
    idx = 1 ;
  } else if (addr + size <= 0xFFFFFFFF) {
    idx = 2 ;
  }

  for (u32_t i = 0 ; i < fmt_size[idx] + 2 ; ++i)
    fputc(' ', file) ;

  for (u32_t i = 0 ; i < 16 ; ++i)
    fprintf(file, " %02" PRIX32, i) ;

  fputc(' ', file) ;
  fputc(' ', file) ;
  fputc(' ', file) ;
  fputc(' ', file) ;

  for (u32_t i = 0 ; i < 16 ; ++i)
    fprintf(file, "%" PRIX32, i) ;

  fputc('\n', file) ;

  for (u32_t i = 0 ; i < fmt_size[idx] + 1 ; ++i)
    fputc(' ', file) ;

  fputc('.', file) ;

  for (u32_t i = 0 ; i < 16 * 3 + 4 + 16 ; ++i)
    fputc('-', file) ;

  fputc('\n', file) ;

  for (u64_t i = 0 ; i < size ;) {
    fprintf(file, fmt_data[idx], addr + i) ;

    u64_t lim = 16 ;

    if (size < i + lim)
      lim = size - i ;

    u64_t j ;

    for (j = 0 ; j < lim ; ++j)
      fprintf(file, " %02" PRIX8, ((u8_t *)data)[i + j]) ;

    for (j = 0 ; j < (16 - lim) + 1 ; ++j) {
      fputc(' ', file) ;
      fputc(' ', file) ;
      fputc(' ', file) ;
    }

    fputc(' ', file) ;

    for (j = 0 ; j < lim ; ++j) {
      if (' ' <= ((u8_t *)data)[i + j] && ((u8_t *)data)[i + j] <= '~')
        fputc(((u8_t *)data)[i + j], file) ;
      else
        fputc('.', file) ;
    }

    i += j ;

    fputc('\n', file) ;
  }

  return VOO_N_IRQS ;
}
