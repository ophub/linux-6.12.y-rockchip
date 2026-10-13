#ifndef _KE_CONFIG_H_
#define _KE_CONFIG_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <linux/kernel.h>
#include <linux/string.h>

/*
 * CONSTANT DEFINITIONS
 ****************************************************************************************
 */

/// ARM is little endian
#define CPU_LE          1

/** @name Kernel configuration
 * @{
 * Defines if Kernel heap management is compiled or not
 */
#if (__SIZEOF_POINTER__ == 4)
#define CPU_WORD_SIZE   4
#elif (__SIZEOF_POINTER__ == 8)
#define CPU_WORD_SIZE   8
#else
#define CPU_WORD_SIZE   4
#endif


#define KE_MEM_NX       1
#define KE_MEM_LINUX    0
#define KE_MEM_LIBC     0

#define KE_PROFILING    0

/**
 * @brief Null Type definition
 */
#ifndef NULL
#define NULL 0
#endif

/*
 * The fixed width integer types below are already provided by the kernel
 * (<linux/types.h>, which is pulled in by <linux/kernel.h> above) using the
 * kernel's own typedef chain, e.g.
 *
 *	kernel : uint8_t -> u8 -> __u8 -> unsigned char
 *	here   : uint8_t -> unsigned char
 *
 * Re-declaring them is legal C (the underlying types are identical, that is
 * why it compiles) but it leaves a second, differently encoded copy of these
 * typedefs in the DWARF.  pahole cannot deduplicate such types, so every
 * kernel type whose members refer to them ends up twice in the BTF of the
 * object, which makes tools/bpf/resolve_btfids complain:
 *
 *	WARN: multiple IDs found for 'struct module': 146, 58170 - using 146
 *
 * Keep the kernel's types for kernel builds and only declare them for the
 * other (userspace) users of this header.
 */
#ifndef __KERNEL__
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;
typedef signed char         int8_t;
typedef signed char         s8;
typedef short               s16;
typedef short               int16_t;
typedef int                 s32;
typedef int                 int32_t;
typedef long long           s64;
#endif /* !__KERNEL__ */

#ifndef __INLINE
#define __INLINE static __attribute__((__always_inline__)) inline
#endif


#endif // _KE_CONFIG_H_
