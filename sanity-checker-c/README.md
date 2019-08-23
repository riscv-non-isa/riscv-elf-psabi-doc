# RISC-V ELF psABI Sanity Checker (for C compilers)

This directory contains a set of C files to check a C compiler complies with
the basic parts of the [RISC-V ELF psABI specification](../riscv-elf.md).

To check your compiler, run:

```
$ ./sanity-checker-c [path/to/cc]
```

If you do not specify a path to a C compiler, `sanity-checker-c` will
automatically use your `CC` environment variable, or `/usr/bin/cc` if `CC` is
empty.

If `sanity-checker-c` detects the C compiler is clang, it will automatically add
a `--target` argument of either `riscv32-unknown-linux-gnu` or
`riscv64-unknown-linux-gnu`.

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
