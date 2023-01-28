#ifndef __VOOCON_H
# define __VOOCON_H

# include <stdint.h>

# define KiB (UINT64_C(1024))
# define MiB (UINT64_C(1024) * KiB)
# define GiB (UINT64_C(1024) * MiB)
# define TiB (UINT64_C(1024) * GiB)

enum {
  VOO_FLAG_C    = 1 <<  0 ,
  VOO_FLAG_1    = 1 <<  1 ,
  VOO_FLAG_P    = 1 <<  2 ,
  VOO_FLAG_Z    = 1 <<  6 ,
  VOO_FLAG_S    = 1 <<  7 ,
  VOO_FLAG_T    = 1 <<  8 ,
  VOO_FLAG_I    = 1 <<  9 ,
  VOO_FLAG_V    = 1 << 10 ,
  VOO_FLAG_O    = 1 << 11 ,
  VOO_FLAG_IOPL = 3 << 12
} ;

enum {
  VOO_IRQ_DIV_BY_ZERO     ,
  VOO_IRQ_SINGLE_STEP     ,
  VOO_IRQ_NON_MASKABLE    ,
  VOO_IRQ_BREAKPOINT      ,
  VOO_IRQ_GENERAL_PROTECT ,
  VOO_IRQ_OUT_OF_MEMORY   ,
  VOO_IRQ_SEGMENT_PRESENT ,
  VOO_IRQ_SEGMENT_ACCESS  ,
  VOO_IRQ_SEGMENT_FAULT   ,
  VOO_IRQ_STACK_OVERFLOW  ,
  VOO_IRQ_STACK_UNDERFLOW ,
  VOO_IRQ_UNDEF_INTERRUPT ,
  VOO_IRQ_UNDEF_INST      ,
  VOO_IRQ_RESERVED_0      ,
  VOO_IRQ_RESERVED_1      ,
  VOO_IRQ_RESERVED_2      ,

  VOO_N_IRQS = 256
} ;

enum {
  VOO_REG_0  , VOO_REG_1  , VOO_REG_2  , VOO_REG_3  ,
  VOO_REG_4  , VOO_REG_5  , VOO_REG_6  , VOO_REG_7  ,
  VOO_REG_8  , VOO_REG_9  , VOO_REG_10 , VOO_REG_11 ,
  VOO_REG_12 , VOO_REG_13 , VOO_REG_14 , VOO_REG_15 ,
  VOO_REG_16 , VOO_REG_17 , VOO_REG_18 , VOO_REG_19 ,
  VOO_REG_20 , VOO_REG_21 , VOO_REG_22 , VOO_REG_23 ,
  
  VOO_N_REGS
} ;

# define VOO_REG_AX    VOO_REG_0
# define VOO_REG_CX    VOO_REG_1
# define VOO_REG_DX    VOO_REG_2
# define VOO_REG_BX    VOO_REG_3
# define VOO_REG_SP    VOO_REG_14
# define VOO_REG_BP    VOO_REG_15
# define VOO_REG_FLAGS VOO_REG_16
# define VOO_REG_IP    VOO_REG_17
# define VOO_REG_CLOCK VOO_REG_18
# define VOO_REG_IVT   VOO_REG_19
# define VOO_REG_IRQ   VOO_REG_20
# define VOO_REG_ISR   VOO_REG_21

enum {
  VOO_SEG_0  , VOO_SEG_1  , VOO_SEG_2  , VOO_SEG_3  ,
  VOO_SEG_4  , VOO_SEG_5  , VOO_SEG_6  , VOO_SEG_7  ,
  VOO_SEG_8  , VOO_SEG_9  , VOO_SEG_10 , VOO_SEG_11 ,
  VOO_SEG_12 , VOO_SEG_13 , VOO_SEG_14 , VOO_SEG_15 ,

  VOO_N_SEGS
} ;

# define VOO_SEG_CODE  VOO_REG_0
# define VOO_SEG_DATA  VOO_REG_1
# define VOO_SEG_STACK VOO_REG_2
# define VOO_SEG_IVT   VOO_REG_14
# define VOO_SEG_ISR   VOO_REG_15

#endif