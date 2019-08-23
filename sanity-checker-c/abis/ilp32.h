// RISC-V 32-bit ILP32 Sanity Checks    -*- C -*-

// For wchar_t, wint_t
#include <wchar.h>
// For max_align_t
#include <stddef.h>

// Defines CHECK_* Macros
#include "../utils.h"

// Checks for all ILP32* ABIs.

// Standard C Types: Implementation Defined Sizes & Alignments
CHECK_SIZE_ALIGN(_Bool, 1, 1);
CHECK_SIZE_ALIGN(char, 1, 1);
CHECK_SIZE_ALIGN(short, 2, 2);
CHECK_SIZE_ALIGN(int, 4, 4);
CHECK_SIZE_ALIGN(wchar_t, 4, 4);
CHECK_SIZE_ALIGN(wint_t, 4, 4);
CHECK_SIZE_ALIGN(long, 4, 4);
CHECK_SIZE_ALIGN(long long, 8, 8);
CHECK_SIZE_ALIGN(void *, 4, 4);
CHECK_SIZE_ALIGN(float, 4, 4);
CHECK_SIZE_ALIGN(double, 8, 8);
CHECK_SIZE_ALIGN(long double, 16, 16);

// Standard C Complex Types: Implementation Defined Sizes & Alignments
CHECK_SIZE_ALIGN(_Complex float, 8, 4);
CHECK_SIZE_ALIGN(_Complex double, 16, 8);
CHECK_SIZE_ALIGN(_Complex long double, 32, 16);

// Standard C Atomic Types: Implementation Defined Sizes & Alignment
CHECK_SIZE_ALIGN(_Atomic(_Bool), 1, 1);
CHECK_SIZE_ALIGN(_Atomic(char), 1, 1);
CHECK_SIZE_ALIGN(_Atomic(short), 2, 2);
CHECK_SIZE_ALIGN(_Atomic(int), 4, 4);
CHECK_SIZE_ALIGN(_Atomic(wchar_t), 4, 4);
CHECK_SIZE_ALIGN(_Atomic(wint_t), 4, 4);
CHECK_SIZE_ALIGN(_Atomic(long), 4, 4);
CHECK_SIZE_ALIGN(_Atomic(long long), 8, 8);
CHECK_SIZE_ALIGN(_Atomic(void *), 4, 4);
CHECK_SIZE_ALIGN(_Atomic(float), 4, 4);
CHECK_SIZE_ALIGN(_Atomic(double), 8, 8);
CHECK_SIZE_ALIGN(_Atomic(long double), 16, 16);

// Standard C Atomic Complex Types: Implementation Defined Sizes & Alignments
CHECK_SIZE_ALIGN(_Atomic(_Complex float), 8, 8);
CHECK_SIZE_ALIGN(_Atomic(_Complex double), 16, 16);
CHECK_SIZE_ALIGN(_Atomic(_Complex long double), 32, 16);

// Standard C Types: Implementation Defined Alignment
CHECK_EQ("_Alignof(max_align_t)", 16, _Alignof(max_align_t));

// Standard C Types: Implementation Defined Signedness
#ifndef __CHAR_UNSIGNED__
_Static_assert(0, "char should be an unsigned type");
#endif

#ifdef __WCHAR_UNSIGNED__
_Static_assert(0, "wchar_t should be a signed type")
#endif

// ABI-Specific Preprocessor Defines:
#ifndef __ELF__
    _Static_assert(0, "__ELF__ (preprocessor define) not defined");
#endif

#ifndef __riscv
_Static_assert(0, "__riscv (preprocessor define) not defined");
#endif

// Checks for ILP32 ABI Only
#if !defined(ilp32f) && !defined(ilp32d) && !defined(ilp32e)

// ABI-Specific Preprocessor Defines:
#  ifndef __riscv_float_abi_soft
_Static_assert(0, "__riscv_float_abi_soft (preprocessor define) not defined");
#  endif

#  ifdef __riscv_float_abi_single
_Static_assert(0, "__riscv_float_abi_float (preprocessor define) is defined "
                  "and should not be");
#  endif

#  ifdef __riscv_float_abi_double
_Static_assert(0, "__riscv_float_abi_double (preprocessor define) is defined "
                  "and should not be")
#  endif

#  ifdef __riscv_abi_rve
    _Static_assert(
        0, "__riscv_abi_rve (preprocessor define) is defined and should not be")
#  endif

#endif
