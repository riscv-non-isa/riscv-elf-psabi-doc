// RISC-V 64-bit LP64D Compliance Checks    -*- C -*-

// Based off LP64 Checks
#include "lp64.h"

// ABI-Specific Preprocessor Defines:
#ifdef __riscv_float_abi_soft
_Static_assert(0, "__riscv_float_abi_soft (preprocessor define) is defined and "
                  "should not be");
#endif

#ifdef __riscv_float_abi_single
_Static_assert(0, "__riscv_float_abi_single (preprocessor define) is defined "
                  "and should not be");
#endif

#ifndef __riscv_float_abi_double
_Static_assert(0,
               "__riscv_float_abi_double (preprocessor define) is not defined");
#endif
