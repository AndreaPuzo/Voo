#ifndef __VOODEF_H
# define __VOODEF_H

# include <stdint.h>

# define KiB (UINT64_C(1024))
# define MiB (UINT64_C(1024) * KiB)
# define GiB (UINT64_C(1024) * MiB)
# define TiB (UINT64_C(1024) * GiB)

typedef uint8_t  u8_t  ;
typedef uint16_t u16_t ;
typedef uint32_t u32_t ;
typedef uint64_t u64_t ;
typedef int8_t   i8_t  ;
typedef int16_t  i16_t ;
typedef int32_t  i32_t ;
typedef int64_t  i64_t ;

typedef struct voo_ker_s voo_ker_t ;
typedef struct voo_opr_s voo_opr_t ;
typedef struct voo_mem_s voo_mem_t ;
#ifdef __VOO_HAS_OPTIONS
typedef struct voo_opt_s voo_opt_t ;
#endif
typedef struct voo_s     voo_t     ;

struct voo_ker_s {
  u32_t seg [VOO_N_SEGS] ;
  u64_t reg [VOO_N_REGS] ;
} ;

struct voo_opr_s {
  u32_t seg   ;
  u64_t addr  ;
  u32_t data  ;
  u32_t stack ;
} ;

struct voo_mem_s {
  u64_t  size ;
  u8_t * data ;
} ;

#ifdef __VOO_HAS_OPTIONS
struct voo_opt_s {
  u16_t verbose    : 1 ; // print information
  u16_t dump_STATE : 1 ; // dump the registers after `clock cycle`
  u16_t dump_CODE  : 1 ; // dump the instruction during `fetch`
  u16_t dump_DATA  : 1 ; // dump the data during `write` and `read`
  u16_t dump_STACK : 1 ; // dump the stack during `push` and `pop`
  u16_t halt_IRQ   : 1 ; // stop the machine when an IRQ is raised
  u16_t force_IRQ  : 1 ; // force the machine to execute the IRQ
  u16_t save_STATE : 1 ; // save the machine state during the unbooting
  u64_t cks            ; // limit for clock cycles
} ;
#endif

struct voo_s {
  voo_ker_t ker ;
  voo_opr_t opr ;
  voo_mem_t mem ;
#ifdef __VOO_HAS_OPTIONS
  voo_opt_t opt ;
#endif
  void *    ptr ;

  u32_t (* out) (
    voo_t * ,
    u32_t   ,
    u64_t   ,
    void *
  ) ;

  u32_t (* out_8) (
    voo_t * ,
    u32_t   ,
    u8_t
  ) ;

  u32_t (* out_16) (
    voo_t * ,
    u32_t   ,
    u16_t
  ) ;

  u32_t (* out_32) (
    voo_t * ,
    u32_t   ,
    u32_t
  ) ;

  u32_t (* out_64) (
    voo_t * ,
    u32_t   ,
    u64_t
  ) ;

  u32_t (* in) (
    voo_t * ,
    u32_t   ,
    u64_t   ,
    void *
  ) ;

  u32_t (* in_8) (
    voo_t * ,
    u32_t   ,
    u8_t *
  ) ;

  u32_t (* in_16) (
    voo_t * ,
    u32_t   ,
    u16_t *
  ) ;

  u32_t (* in_32) (
    voo_t * ,
    u32_t   ,
    u32_t *
  ) ;

  u32_t (* in_64) (
    voo_t * ,
    u32_t   ,
    u64_t *
  ) ;
} ;

#endif