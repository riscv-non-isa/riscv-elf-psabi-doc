# RISC-V ELF psABI Compliance Checker

This directory contains a set of C files to ensure a compiler complies with the
[RISC-V ELF psABI specification](../riscv-elf.md).

To check your compiler, run:

```
$ ./checker [path/to/cc]
```

If you do not specify a path to a C compiler, `checker` will automatically use
your `CC` environment variable, or `/usr/bin/cc` if `CC` is empty.

## Copyright and license information

The RISC-V ELF psABI Compliance Checker in this directory is:

 &copy; 2019 lowRISC CIC

It is licensed under the Apache 2.0 licence, available in LICENSE.txt.
