#ifndef __VOOVER_H
# define __VOOVER_H

# define __VOO_VERSION_MAJOR 0
# define __VOO_VERSION_MINOR 0
# define __VOO_VERSION_PATCH 0
# define __VOO_VERSION_EXTRA 1

# define __VOO_VERSION            \
  (                               \
    (__VOO_VERSION_MAJOR << 24) | \
    (__VOO_VERSION_MINOR << 16) | \
    (__VOO_VERSION_PATCH <<  8) | \
    (__VOO_VERSION_EXTRA <<  0)   \
  )

#endif