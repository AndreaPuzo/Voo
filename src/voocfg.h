#ifndef __VOOCFG_H
# define __VOOCFG_H

# define __VOO_STATIC static

# define __VOO_API_EXPORT
# define __VOO_API_IMPORT
# define __VOO_API

# ifndef __VOO_HOST_ENDIAN
// https://sourceforge.net/p/predef/wiki/Architectures
#  if defined(___amd64__) || defined(__amd64)  ||                  \
      defined(__x86_64__) || defined(__x86_64) ||                  \
      defined(_M_AMD64)   || defined(_M_X64)   ||                  \
      defined(__i386__)   || defined(__i386)   || defined(i386) || \
      defined(__IA32__)   || defined(_M_IX86)  ||                  \
      defined(__X86__)    || defined(_X86_)
#   define __VOO_HOST_ENDIAN 0x00010203
#  else
#   define __VOO_HOST_ENDIAN 0x03020100
#  endif
# endif

# ifndef __VOO_ENDIAN
#  ifdef __VOO_HAS_LITTLE_ENDIAN
#   define __VOO_ENDIAN 0x00010203
#  elif defined(__VOO_HAS_BIG_ENDIAN)
#   define __VOO_ENDIAN 0x03020100
#  else
#   define __VOO_ENDIAN __VOO_HOST_ENDIAN
#  endif
# endif

# define __VOO_MAX_PATH     1024
# define __VOO_MAX_FILENAME 256
# define __VOO_MEMORY_SIZE  0x2000

#endif