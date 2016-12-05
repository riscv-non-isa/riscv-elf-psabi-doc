# Overview of an ELF file

# Data Representation

# File Header

* e_ident
  * EI_CLASS: Specifies the base ISA, either RV32 ond RV64.  We don't let users
    link RV32 and RV64 code together.
    * ELFCLASS64: ELF-64 Object File
    * ELFCLASS32: ELF-32 Object File

* e_type: Nothing RISC-V specific.

* e_machine: Identifies the machine this ELF file targets.  Always contains
  EM_RISCV (243) for RISC-V ELF files.  We only support RISC-V v2 family ISAs,
  this support is implicit.

* e_flags: Describes the format of this ELF file.  These flags are used by the
  linker to disallow linking ELF files with incompatible ABIs together.

   Bit 0 | Bit  1 - 2 | Bit 3 | Bit 4 - 31
  -------|------------|-------|------------
   RVC   | Float ABI  |  RVE  | UNUSED


  * EF_RISCV_RVC (0x0001): This bit is set when the binary targets the C ABI,
    which allows instructions to be aligned to 16-bit boundries (the base RV32
    and RV64 ISAs only allow 32-bit instruction alignment).  When linking
    objects which specify EF_RISCV_RVC, the linker is permitted to use RVC
    instructions such as C.JAL in the relaxation process.
  * EF_RISCV_FLOAT_ABI_SINGLE (0x0002)
  * EF_RISCV_FLOAT_ABI_DOUBLE (0x0004)
  * EF_RISCV_FLOAT_ABI_QUAD (0x0006): These three flags identify the floating
    point ABI in use for this ELF file.  They store the largest floating-point
    type that ends up in registers as part of the ABI (but do not control if
    code generation is allowed to use floating-point internally).  The rule is
    that if you have a floating-point type in a register, then you also have
    all smaller floating-point types in registers.  For example _DOUBLE would
    store "float" and "double" values in F registers, but would not store "long
    double" values in F registers.  If none of the float ABI flags are set, the
    object is taken to use the soft-float ABI.
  * EF_RISCV_RVE (0x0008): This bit is set when the binary targets the E ABI.

# Register Map

Integer Registers
--------------------------------
Name    | ABI Mnemonic | Meaning
--------|--------------|--------
x0      | zero         | Zero
x1      | ra           | Return address
x2      | sp           | Stack pointer
x3      | gp           | Global pointer
x4      | tp           | Thread pointer
x5-x7   | t0-t2        | Temporary registers
x8-x9   | s0-s1        | Callee-saved registers
x10-x17 | a0-a7        | Argument registers
x18-x27 | s2-s11       | Callee-saved registers
x28-x31 | t3-t6        | Temporary registers

Floating-point Registers
--------------------------------
Name    | ABI Mnemonic | Meaning
--------|--------------|--------
f0-f7   | ft0-ft7      | Temporary registers
f8-f9   | fs0-fs1      | Callee-saved registers
f10-f17 | fa0-fa7      | Argument registers
f18-f27 | fs2-fs11     | Callee-saved registers
f28-f31 | ft8-ft11     | Temporary registers

# Procedure Calling Convention

The base integer calling convention provides eight argument registers,
a0-a7, the first two of which are also used to return values.

Scalars that are at most XLEN bits wide are passed in a single argument
register, or on the stack by value if none is available.  When passed in
registers, scalars narrower than XLEN bits are widened according to the sign
of their type up to 32 bits, then sign-extended to XLEN bits.

Scalars that are 2✕XLEN bits wide are passed in an aligned pair of argument
registers (i.e., the first register in the pair is even-numbered), or on the
stack by value if none is available.  Wider scalars are passed by reference
and are replaced in the argument list with the address.

Aggregates whose total size is no more than XLEN bits are passed in
a register, with the fields laid out as though they were passed in memory.
Aggregates whose total size is no more than 2✕XLEN bits are passed in a pair
of registers; if only one register is available, the first half is passed in
a register and the second half is passed on the stack.  Bits unused due to
padding, and bits past the end of an aggregate whose size in bits is not
divisible by XLEN, are undefined.

Aggregates larger than 2✕XLEN bits are passed by reference and are replaced in
the argument list with the address, as are C++ aggregates with nontrivial copy
constructors, destructors, or vtables.

Arguments passed by reference may be modified by the callee.

If an aggregate no larger than 2✕XLEN bits has 2✕XLEN-bit alignment, it is
passed in an aligned pair of argument registers, or on the stack by value if
none is available.

Floating-point reals are passed the same way as integers of the same size, and
complex floating-point numbers are passed the same way as a struct containing
two floating-point reals.

In the base integer calling convention, variadic arguments are passed in the
same manner as named arguments.

Values are returned in the same manner as a first named argument of the same
type would be passed.  If such an argument would have been passed by
reference, the caller allocates memory for the return value, and passes the
address as an implicit first parameter.

## Hardware floating-point calling convention

The hardware floating-point calling convention adds eight floating-point
argument registers, fa0-fa7, the first two of which are also used to return
values.  Values are passed in floating-point registers whenever possible, even
if all integer registers have been exhausted.  The remainder of this section
applies only to named arguments.  Variadic arguments are passed according to
the integer calling convention.

For the purposes of this section, FLEN refers to the width of a
floating-point register in the ABI.  The ISA might have wider
floating-point registers than the ABI.

For the purposes of this section, "struct" refers to a C struct with its
hierarchy flattened, including any array fields.  That is, struct { struct
{ float f[1]; } g[2]; int h; } and struct { float f; float g; int h; } are
treated the same.

A real floating-point argument is passed in a floating-point argument
register if it is no more than FLEN bits wide and at least one floating-point
argument register is available.  Otherwise, it is passed the same way as an
integral argument of the same size.

A struct containing just one floating-point real is passed as though it were
a standalone floating-point real.

A struct containing two floating-point reals is passed in two floating-point
registers, if neither is more than FLEN bits wide and at least two floating-point
argument registers are available.  (The registers need not be an aligned pair.)
Otherwise, it is passed according to the integer calling convention.

A complex floating-point number, or a struct containing just one complex
floating-point number, is passed as though it were a struct containing two
floating-point reals.

A struct containing one floating-point real and one integer, in either
order, is passed in a floating-point register and an integer register,
provided the floating-point real is no more than FLEN bits wide and the
integer is no more than XLEN bits wide, and at least one floating-point
argument register and at least one integer argument register is available.
Otherwise, it is passed according to the integer calling convention.

Values are returned in the same manner as a first named argument of the same
type would be passed.

# Sections

# String Tables

# Symbal Table

# Relocations

# Program Header Table

# Note Sections

# Dynamic Table

# Hash Table
