// RISC-V rv32imafdc Sanity Checks    -*- C -*-

#include "utils.h"

// Architecture-Specific Preprocessor Definitions:
CHECK_DEFINED_EQ(__riscv_xlen, 32);
CHECK_DEFINED_EQ(__riscv_flen, 64);
