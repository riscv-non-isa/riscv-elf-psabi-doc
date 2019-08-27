# RISC-V ELF psABI Sanity Checker (for C compilers)

This directory contains a set of C files to check a C compiler complies with
the basic parts of the [RISC-V ELF psABI specification](../riscv-elf.md).

To check your compiler, run:

```
$ ./sanity-checker-c [target] [cc] [path/to/sysroot]
```

`[target]` should be one of:
  - `riscv32-unknown-elf`
  - `riscv64-unknown-elf`
  - `riscv32-unknown-linux-gnu`
  - `riscv64-unknown-linux-gnu`

`[cc]` should be a C compiler to check

`[path/to/sysroot]` should be the path to a valid multilib sysroot for the given
target. This is optional if your compiler will choose the right directory.

## RISC-V ABI- and Architecture-specific C Preprocessor Definitions

The RISC-V ELF psABI is not language-specific, and so does not mandate the
definition of specific C preprocessor definitions.

However, there is value in ensuring that C compilers for the RISC-V psABI match
in terms of ABI- and architecture-specific preprocessor defines. These tests
ensure that compilers match on the preprocessor definitions.

The C preprocessor definition checks are based on both
[riscv-toolchain-conventions](https://github.com/riscv/riscv-toolchain-conventions#cc-preprocessor-definitions)
and
[riscv-c-api](https://github.com/riscv/riscv-c-api-doc/blob/master/riscv-c-api.md#preprocessor-definitions).

## Copyright and license information

The RISC-V ELF psABI Sanity Checker (for C compilers) in this directory is:

 &copy; 2019 lowRISC CIC

It is licensed under the Apache 2.0 licence, available in LICENSE.txt.
