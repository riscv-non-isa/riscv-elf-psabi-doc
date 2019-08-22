// RISC-V 32-bit ILP32F Compliance Checks    -*- C -*-

// Based off ILP32 Checks
#include "ilp32.h"

// ABI-Specific Preprocessor Defines:
#ifdef __riscv_float_abi_soft
_Static_assert(0, "__riscv_float_abi_soft (preprocessor define) is defined and "
                  "should not be");
#endif

#ifndef __riscv_float_abi_single
_Static_assert(0,
               "__riscv_float_abi_single (preprocessor define) is not defined");
#endif

#ifdef __riscv_float_abi_double
_Static_assert(0, "__riscv_float_abi_double (preprocessor define) is defined "
                  "and should not be");
#endif

#ifdef __riscv_abi_rve
_Static_assert(
    0, "__riscv_abi_rve (preprocessor define) is defined and should not be")
#endif
