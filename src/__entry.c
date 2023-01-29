#include "voo.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#define r(__type, __x) (__type *)(voo.ker.reg + (__x))

#define r8(__x)  r(u8_t , (__x))
#define r16(__x) r(u16_t, (__x))
#define r32(__x) r(u32_t, (__x))
#define r64(__x) r(u64_t, (__x))

#define rSP    *r64(VOO_REG_SP)
#define rBP    *r64(VOO_REG_BP)
#define rFLAGS *r32(VOO_REG_FLAGS)
#define rIP    *r64(VOO_REG_IP)
#define rCLOCK *r64(VOO_REG_CLOCK)
#define rIRQ   *(u32_t *)(voo->ker.reg + VOO_REG_IRQ)

u32_t port_out (
  voo_t * voo  ,
  u32_t   port ,
  u64_t   size ,
  void *  data
) ;

u32_t port_out_8 (
  voo_t * voo  ,
  u32_t   port ,
  u8_t    data
) ;

u32_t port_out_16 (
  voo_t * voo  ,
  u32_t   port ,
  u16_t   data
) ;

u32_t port_out_32 (
  voo_t * voo  ,
  u32_t   port ,
  u32_t   data
) ;

u32_t port_out_64 (
  voo_t * voo  ,
  u32_t   port ,
  u64_t   data
) ;

u32_t port_in (
  voo_t * voo  ,
  u32_t   port ,
  u64_t   size ,
  void *  data
) ;

u32_t port_in_8 (
  voo_t * voo  ,
  u32_t   port ,
  u8_t *  data
) ;

u32_t port_in_16 (
  voo_t * voo  ,
  u32_t   port ,
  u16_t * data
) ;

u32_t port_in_32 (
  voo_t * voo  ,
  u32_t   port ,
  u32_t * data
) ;

u32_t port_in_64 (
  voo_t * voo  ,
  u32_t   port ,
  u64_t * data
) ;

#define N_DSKS 2

int main (int argc, char ** argv)
{
  if (1 == argc) {
    fprintf(
      stderr                         ,
      "usage: %s [<opt>...] <img>\n"
      "usage: %s doo [<opt>...]\n"   ,
      argv[0]                        ,
      argv[0]
    ) ;

    exit(EXIT_SUCCESS) ;
  }

  __VOO_STATIC
  voo_t voo ;

#ifdef __VOO_HAS_OPTIONS
  voo.opt.verbose    =  0 ;
  voo.opt.dump_STATE =  0 ;
  voo.opt.dump_CODE  =  0 ;
  voo.opt.dump_DATA  =  0 ;
  voo.opt.dump_STACK =  0 ;
  voo.opt.halt_IRQ   =  0 ;
  voo.opt.force_IRQ  =  0 ;
  voo.opt.save_STATE =  0 ;
  voo.opt.cks        = -1 ;
#endif

  struct {
    u32_t  doo       : 1 ;
    u32_t  clear_log : 1 ;
    u32_t  clear_dsk : 1 ;
    u32_t  config    : 1 ;
    u64_t  mem           ;
    char * img           ;
  } opt ;

  opt.doo       = 0 ;
  opt.clear_log = 0 ;
  opt.clear_dsk = 0 ;
  opt.config    = 0 ;
  opt.mem       = __VOO_MEMORY_SIZE ;
  opt.img       = NULL ;

  if (0 == strcmp(argv[1], "--help") || 0 == strcmp(argv[1], "-h")) {
    fprintf(
      stderr ,
#ifdef __VOO_HAS_OPTIONS
      "usage: %s [<opt>...] <img>\n"
      "usage: %s doo [<opt>...]\n"
#else
      "usage: %s <img>\n"
      "usage: %s doo\n"
#endif
      "\n"
      "options:\n"
      "  -h , --help      --- Print this help page.\n"
      "       --help=docs|repo\n"
      "       --version   --- Print the version.\n"
      "  -e , --endian    --- Print the endian.\n"
      "       --clear-log --- Clear the file `.voolog`.\n"
      "       --clear-dsk --- Clear the file `.voodsk`.\n"
      "       --config    --- Set the default configuration.\n"
      "  -m , --mem <num> --- Set the configuration memory.\n"
#ifdef __VOO_HAS_OPTIONS
      "  -v , --verbose   --- Print misc information.\n"
      "  -c , --cks <num> --- Set the clock limit.\n"
      "  -d , --dump      --- Turn on all the dump options.\n"
      "       --dump=state|code|data|stack\n"
      "  -i               --- Halt if the IRQ is raised.\n"
      "  -I               --- Force the raise of IRQ.\n"
      "  -s , --save      --- Save the state onto `.voodmp`\n"
#endif
      , argv[0] , argv[0]
    ) ;

    exit(EXIT_SUCCESS) ;
  }

  if (0 == strncmp(argv[1], "--help=", 7)) {
    if (0 == strcmp(argv[1] + 7, "docs")) {
      fprintf(
        stderr ,
        "You can found the documentation here:\n"
        "  https://github.com/AndreaPuzo/Voo/blob/main/Docs.md\n"
      ) ;
    } else if (0 == strcmp(argv[1] + 7, "repo")) {
      fprintf(
        stderr ,
        "You can found the repository here:\n"
        "  https://github.com/AndreaPuzo/Voo\n"
      ) ;
    } else {
      fprintf(stderr, "error: invalid option `%s`\n", argv[1]) ;
      exit(EXIT_FAILURE) ;
    }

    exit(EXIT_SUCCESS) ;
  }

  if (0 == strcmp(argv[1], "--version")) {
    fprintf(
      stderr              ,
      "Voo %d.%d.%d%s\n"  ,
      __VOO_VERSION_MAJOR ,
      __VOO_VERSION_MINOR ,
      __VOO_VERSION_PATCH ,
#if __VOO_VERSION_EXTRA == 1
      "-alpha"
#elif __VOO_VERSION_EXTRA == 2
      "-beta"
#else
      ""
#endif
    ) ;

    exit(EXIT_SUCCESS) ;
  }

  if (0 == strcmp(argv[1], "--endian") || 0 == strcmp(argv[1], "-e")) {
    fprintf(
      stderr ,
#if __VOO_HOST_ENDIAN == 0x00010203
      "host: little endian\n"
#else
      "host: big endian\n"
#endif

#if __VOO_ENDIAN == 0x00010203
      "vm: little endian\n"
#else
      "vm: big endian\n"
#endif
    ) ;

    exit(EXIT_SUCCESS) ;
  }

  int i = 1 ;

  if (0 == strcmp(argv[i], "doo")) {
    opt.doo = 1 ;
    ++i ;
  }

  for (; i < argc ; ++i) {
    if (0 == strcmp(argv[i], "--clear-log")) {
      opt.clear_log = 1 ;
    } else if (0 == strcmp(argv[i], "--clear-dsk")) {
      opt.clear_dsk = 1 ;
    } else if (0 == strcmp(argv[i], "--config")) {
      opt.config = 1 ;
    } else if (0 == strcmp(argv[i], "--mem") || 0 == strcmp(argv[i], "-m")) {
      if (argc <= i + 1) {
        fprintf(stderr, "error: `%s` requires <num>\n", argv[i]) ;
        exit(EXIT_FAILURE) ;
      }

      opt.mem = strtoull(argv[++i], NULL, 0) ;
    }
#ifdef __VOO_HAS_OPTIONS
    else if (0 == strcmp(argv[i], "--verbose") || 0 == strcmp(argv[i], "-v")) {
      voo.opt.verbose = 1 ;
    } else if (0 == strcmp(argv[i], "--cks") || 0 == strcmp(argv[i], "-c")) {
      if (argc <= i + 1) {
        fprintf(stderr, "error: `%s` requires <num>\n", argv[i]) ;
        exit(EXIT_FAILURE) ;
      }

      voo.opt.cks = strtoull(argv[++i], NULL, 0) ;
    } else if (0 == strcmp(argv[i], "--dump") || 0 == strcmp(argv[i], "-d")) {
      voo.opt.dump_STATE = 1 ;
      voo.opt.dump_CODE  = 1 ;
      voo.opt.dump_DATA  = 1 ;
      voo.opt.dump_STACK = 1 ;
    } else if (0 == strncmp(argv[i], "--dump=", 7)) {
      char * str = argv[i] + 7 ;

      if (0 == strcmp(str, "state")) {
        voo.opt.dump_STATE = 1 ;
      } else if (0 == strcmp(str, "code")) {
        voo.opt.dump_CODE = 1 ;
      } else if (0 == strcmp(str, "data")) {
        voo.opt.dump_DATA = 1 ;
      } else if (0 == strcmp(str, "stack")) {
        voo.opt.dump_STACK = 1 ;
      } else {
        fprintf(stderr, "error: invalid option `%s`\n", argv[i]) ;
        exit(EXIT_FAILURE) ;
      }
    } else if (0 == strcmp(argv[i], "-i")) {
      voo.opt.halt_IRQ = 1 ;
    } else if (0 == strcmp(argv[i], "-I")) {
      voo.opt.force_IRQ = 1 ;
    } else if (0 == strcmp(argv[i], "--save") || 0 == strcmp(argv[i], "-s")) {
      voo.opt.save_STATE = 1 ;
    }
#endif
    else {
      opt.img = argv[i] ;
    }
  }

  if (0 != opt.clear_log)
    remove(".voolog") ;

  if (0 != opt.clear_dsk)
    remove(".voodsk") ;

  voo.out    = port_out    ;
  voo.out_8  = port_out_8  ;
  voo.out_16 = port_out_16 ;
  voo.out_32 = port_out_32 ;
  voo.out_64 = port_out_64 ;
  voo.in     = port_in     ;
  voo.in_8   = port_in_8   ;
  voo.in_16  = port_in_16  ;
  voo.in_32  = port_in_32  ;
  voo.in_64  = port_in_64  ;

  FILE * dsk [N_DSKS] = {
    NULL ,
    NULL
  } ;

  const char * dsk_name [N_DSKS] = {
    ".voolog" ,
    ".voodsk"
  } ;

  const char * dsk_mode [N_DSKS] = {
    "ab+" ,
    "ab+"
  } ;

  for (int i = 0 ; i < N_DSKS ; ++i) {
    dsk[i] = fopen(dsk_name[i], dsk_mode[i]) ;

    if (NULL == dsk[i]) {
      fprintf(stderr, "error: cannot open `%s`\n", dsk_name[i]) ;
      exit(EXIT_FAILURE) ;
    }
  }

  fseek(dsk[1], 0, SEEK_SET) ;

  voo.ptr = dsk ;

  if (0 == opt.doo) {
    if (0 == opt.config && NULL == opt.img) {
      fprintf(stderr, "error: expected <img>\n") ;
      exit(EXIT_FAILURE) ;
    }

    FILE * fp = fopen(".voocfg", "w") ;

    if (NULL == fp) {
      fprintf(stderr, "error: cannot create/override `.voocfg`\n") ;
      exit(EXIT_FAILURE) ;
    }

    if (0 != opt.config) {
      fprintf(fp, "MEM 8 KiB\nIMG sos/sos.o") ;
      fclose(fp) ;
      exit(EXIT_SUCCESS) ;
    }

    char mul ;

    if (opt.mem <= MiB) {
      mul = 'K' ;
      opt.mem >>= 10 ;
    } else if (opt.mem <= GiB) {
      mul = 'M' ;
      opt.mem >>= 20 ;
    } else {
      mul = 'G' ;
      opt.mem >>= 30 ;
    }

    fprintf(
      fp                       ,
      "MEM %" PRIu64 " %ciB\n"
      "IMG %s"                 ,
      opt.mem                  ,
      mul                      ,
      opt.img
    ) ;

    fclose(fp) ;
  }

  if (VOO_N_IRQS != voo_boot(&voo)) {
    voo_unboot(&voo) ;
    exit(EXIT_FAILURE) ;
  }

  // fprintf(stderr, "Memory:\n") ;
  // voo_dump(stderr, 0, voo.mem.size, voo.mem.data) ;

  rSP = rBP = rIP =
    (sizeof(u32_t) + sizeof(u64_t)) * VOO_N_IRQS +
    0x100                                        +
    0x300
  ;

  voo.out_8(&voo, 0x03, 0x45) ;
  voo.out_8(&voo, 0x03, 0x70) ;
  voo.out_8(&voo, 0x03, 0xBE) ;
  voo.out_8(&voo, 0x03, 0xEF) ;
  voo.out_32(&voo, 0x03, 0x00010203) ; // to check the endian
  voo.out(&voo, 0x03, 6, "START") ;

  rFLAGS |= VOO_FLAG_1 ;

  while (0 != (rFLAGS & VOO_FLAG_1))
    voo_clock(&voo) ;

  voo.out(&voo, 0x03, 6, "CLOCK") ;
  voo.out_64(&voo, 0x03, rCLOCK) ;

  if (VOO_N_IRQS != voo_unboot(&voo))
    exit(EXIT_FAILURE) ;

  for (int i = 0 ; i < N_DSKS ; ++i)
    fclose(dsk[i]) ;

  exit(EXIT_SUCCESS) ;
}

u32_t port_out (
  voo_t * voo  ,
  u32_t   port ,
  u64_t   size ,
  void *  data
)
{
  switch (port) {
  case 0x00 :
    return voo_int(voo, VOO_IRQ_GENERAL_PROTECT) ;

  case 0x01 : {
    for (u64_t i = 0 ; i < size ; ++i)
      fputc(((u8_t *)data)[i], stdout) ;
  } break ;

  case 0x02 : {
    for (u64_t i = 0 ; i < size ; ++i)
      fputc(((u8_t *)data)[i], stderr) ;
  } break ;

  case 0x03 :
  case 0x04 : {
    for (u64_t i = 0 ; i < size ; ++i)
      fputc(((u8_t *)data)[i], ((FILE **)voo->ptr)[port - 0x03]) ;
  } break ;

  default :
    return voo_int(voo, VOO_IRQ_UNDEF_INST) ;
  }

  return VOO_N_IRQS ;
}

u32_t port_out_8 (
  voo_t * voo  ,
  u32_t   port ,
  u8_t    data
)
{
  return port_out(
    voo          ,
    port         ,
    sizeof(data) ,
    &data
  ) ;
}

u32_t port_out_16 (
  voo_t * voo  ,
  u32_t   port ,
  u16_t   data
)
{
#if __VOO_ENDIAN != __VOO_HOST_ENDIAN
  data = voo_bswap_16(data) ;
#endif

  return port_out(
    voo          ,
    port         ,
    sizeof(data) ,
    &data
  ) ;
}

u32_t port_out_32 (
  voo_t * voo  ,
  u32_t   port ,
  u32_t   data
)
{
#if __VOO_ENDIAN != __VOO_HOST_ENDIAN
  data = voo_bswap_32(data) ;
#endif

  return port_out(
    voo          ,
    port         ,
    sizeof(data) ,
    &data
  ) ;
}

u32_t port_out_64 (
  voo_t * voo  ,
  u32_t   port ,
  u64_t   data
)
{
#if __VOO_ENDIAN != __VOO_HOST_ENDIAN
  data = voo_bswap_64(data) ;
#endif

  return port_out(
    voo          ,
    port         ,
    sizeof(data) ,
    &data
  ) ;
}

u32_t port_in (
  voo_t * voo  ,
  u32_t   port ,
  u64_t   size ,
  void *  data
)
{
  switch (port) {
  case 0x00 : {
    for (u64_t i = 0 ; i < size ; ++i)
      ((u8_t *)data)[i] = fgetc(stdin) ;
  } break ;

  case 0x01 :
  case 0x02 :
    return voo_int(voo, VOO_IRQ_GENERAL_PROTECT) ;

  case 0x03 :
  case 0x04 : {
    for (u64_t i = 0 ; i < size ; ++i)
      ((u8_t *)data)[i] = fgetc(((FILE **)voo->ptr)[port - 0x03]) ;
  } break ;

  default :
    return voo_int(voo, VOO_IRQ_UNDEF_INST) ;
  }

  return VOO_N_IRQS ;
}

u32_t port_in_8 (
  voo_t * voo  ,
  u32_t   port ,
  u8_t *  data
)
{
  return port_in(
    voo           ,
    port          ,
    sizeof(*data) ,
    data
  ) ;
}

u32_t port_in_16 (
  voo_t * voo  ,
  u32_t   port ,
  u16_t * data
)
{
  if (
    VOO_N_IRQS != port_in(
      voo           ,
      port          ,
      sizeof(*data) ,
      data
    )
  ) return rIRQ ;

#if __VOO_ENDIAN != __VOO_HOST_ENDIAN
  *data = voo_bswap_16(*data) ;
#endif

  return VOO_N_IRQS ;
}

u32_t port_in_32 (
  voo_t * voo  ,
  u32_t   port ,
  u32_t * data
)
{
  if (
    VOO_N_IRQS != port_in(
      voo           ,
      port          ,
      sizeof(*data) ,
      data
    )
  ) return rIRQ ;

#if __VOO_ENDIAN != __VOO_HOST_ENDIAN
  *data = voo_bswap_32(*data) ;
#endif

  return VOO_N_IRQS ;
}

u32_t port_in_64 (
  voo_t * voo  ,
  u32_t   port ,
  u64_t * data
)
{
  if (
    VOO_N_IRQS != port_in(
      voo           ,
      port          ,
      sizeof(*data) ,
      data
    )
  ) return rIRQ ;

#if __VOO_ENDIAN != __VOO_HOST_ENDIAN
  *data = voo_bswap_64(*data) ;
#endif

  return VOO_N_IRQS ;
}
