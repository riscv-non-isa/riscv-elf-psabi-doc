// RISC-V rv64imafdc Sanity Checks    -*- C -*-

#include "utils.h"

// Architecture-Specific Preprocessor Definitions:
CHECK_DEFINED_EQ(__riscv_xlen, 64);
CHECK_DEFINED_EQ(__riscv_flen, 64);
