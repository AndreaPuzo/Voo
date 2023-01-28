#ifndef __VOOAPI_H
# define __VOOAPI_H

# include <stdio.h>
# include <byteswap.h>

# define voo_bswap_16(__x) bswap_16(__x)
# define voo_bswap_32(__x) bswap_32(__x)
# define voo_bswap_64(__x) bswap_64(__x)

__VOO_API
u32_t voo_boot (
  voo_t * voo
) ;

__VOO_API
u32_t voo_unboot (
  voo_t * voo
) ;

__VOO_API
u32_t voo_int (
  voo_t * voo ,
  u32_t   IRQ
) ;

__VOO_API
u32_t voo_iret (
  voo_t * voo
) ;

__VOO_API
u32_t voo_write (
  voo_t * voo  ,
  u32_t   seg  ,
  u64_t   addr ,
  u64_t   size ,
  void *  data
) ;

__VOO_API
u32_t voo_write_8 (
  voo_t * voo  ,
  u32_t   seg  ,
  u64_t   addr ,
  u8_t    data
) ;

__VOO_API
u32_t voo_write_16 (
  voo_t * voo  ,
  u32_t   seg  ,
  u64_t   addr ,
  u16_t   data
) ;

__VOO_API
u32_t voo_write_32 (
  voo_t * voo  ,
  u32_t   seg  ,
  u64_t   addr ,
  u32_t   data
) ;

__VOO_API
u32_t voo_write_64 (
  voo_t * voo  ,
  u32_t   seg  ,
  u64_t   addr ,
  u64_t   data
) ;

__VOO_API
u32_t voo_read (
  voo_t * voo  ,
  u32_t   seg  ,
  u64_t   addr ,
  u64_t   size ,
  void *  data
) ;

__VOO_API
u32_t voo_read_8 (
  voo_t * voo  ,
  u32_t   seg  ,
  u64_t   addr ,
  u8_t *  data
) ;

__VOO_API
u32_t voo_read_16 (
  voo_t * voo  ,
  u32_t   seg  ,
  u64_t   addr ,
  u16_t * data
) ;

__VOO_API
u32_t voo_read_32 (
  voo_t * voo  ,
  u32_t   seg  ,
  u64_t   addr ,
  u32_t * data
) ;

__VOO_API
u32_t voo_read_64 (
  voo_t * voo  ,
  u32_t   seg  ,
  u64_t   addr ,
  u64_t * data
) ;

__VOO_API
u32_t voo_clock (
  voo_t * voo
) ;

__VOO_API
u32_t voo_dump (
  FILE * file ,
  u64_t  addr ,
  u64_t  size ,
  void * data
) ;

#endif