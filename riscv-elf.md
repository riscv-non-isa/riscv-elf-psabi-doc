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
argument register is available.  Otherwise, it is passed according to the
integer calling convention.

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

The following table provides details of the RISC-V ELF relocations:

ELF Reloc Type       | Description                | Assembler           | Details
:------------------  | :---------------           | :-------------      | :-----------
R_RISCV_NONE         | None                       |                     |
R_RISCV_32           | Runtime relocation         |                     | word32 = S + A
R_RISCV_64           | Runtime relocation         |                     | word64 = S + A
R_RISCV_RELATIVE     | Runtime relocation         |                     | word32,64 = B + A
R_RISCV_COPY         | Runtime relocation         |                     | must be in executable. not allowed in shared library
R_RISCV_JUMP_SLOT    | Runtime relocation         |                     | word32,64 = S ;handled by PLT unless LD_BIND_NOW
R_RISCV_TLS_DTPMOD32 | TLS relocation             |                     | word32 = S->TLSINDEX
R_RISCV_TLS_DTPMOD64 | TLS relocation             |                     | word64 = S->TLSINDEX
R_RISCV_TLS_DTPREL32 | TLS relocation             |                     | word32 = TLS + S + A - TLS_TP_OFFSET
R_RISCV_TLS_DTPREL64 | TLS relocation             |                     | word64 = TLS + S + A - TLS_TP_OFFSET
R_RISCV_TLS_TPREL32  | TLS relocation             |                     | word32 = TLS + S + A + S_TLS_OFFSET - TLS_DTV_OFFSET
R_RISCV_TLS_TPREL64  | TLS relocation             |                     | word64 = TLS + S + A + S_TLS_OFFSET - TLS_DTV_OFFSET
R_RISCV_BRANCH       | PC-relative branch offset  |                     | SB-Type (beq,bne,blt,bge,bltu,bgeu)
R_RISCV_JAL          | PC-relative jump offset    |                     | UJ-Type (jal)
R_RISCV_CALL         | PC-relative function call  |                     | MACRO call,tail (auipc+jalr)
R_RISCV_CALL_PLT     | PC-relative function call  |                     | MACRO call,tail (auipc+jalr)
R_RISCV_GOT_HI20     | PC-relative GOT offset     |                     | MACRO la
R_RISCV_TLS_GOT_HI20 | PC-relative TLS IE GOT ref | %tls_ie_pcrel_hi(x) | MACRO la.tls.ie
R_RISCV_TLS_GD_HI20  | PC-relative TLS GD GOT ref | %tls_gd_pcrel_hi(x) | MACRO la.tls.gd
R_RISCV_PCREL_HI20   | PC-relative reference      | %pcrel_hi(symbol)   | U-Type (auipc)
R_RISCV_PCREL_LO12_I | PC-relative reference      | %pcrel_lo(label)    | I-Type (lb,lbu,lh,lhu,lw,lwu,flw,fld,addi,addiw)
R_RISCV_PCREL_LO12_S | PC-relative reference      | %pcrel_lo(label)    | S-Type (sb,sh,sw,fsw,fsd)
R_RISCV_HI20         | Absolute address           | %hi(symbol)         | U-Type (lui,auipc)
R_RISCV_LO12_I       | Absolute address           | %lo(symbol)         | I-Type (lb,lbu,lh,lhu,lw,lwu,flw,fld,addi,addiw)
R_RISCV_LO12_S       | Absolute address           | %lo(symbol)         | S-Type (sb,sh,sw,fsw,fsd)
R_RISCV_TPREL_HI20   | TLS LE thread offset       | %tprel_hi(symbol)   | U-Type (auipc)
R_RISCV_TPREL_LO12_I | TLS LE thread offset       | %tprel_lo(label)    | I-Type (lb,lbu,lh,lhu,lw,lwu,flw,fld,addi,addiw)
R_RISCV_TPREL_LO12_S | TLS LE thread offset       | %tprel_lo(label)    | S-Type (sb,sh,sw,fsw,fsd)
R_RISCV_TPREL_ADD    | TLS LE thread usage        | %tprel_add(label)   | internal assembler expansion
R_RISCV_ADD8         | 8-bit label addition       |                     | word8 = S + A
R_RISCV_ADD16        | 16-bit label addition      |                     | word16 = S + A
R_RISCV_ADD32        | 32-bit label addition      |                     | word32 = S + A
R_RISCV_ADD64        | 64-bit label addition      |                     | word64 = S + A
R_RISCV_SUB8         | 8-bit label subtraction    |                     | word8 = S - A
R_RISCV_SUB16        | 16-bit label subtraction   |                     | word16 = S - A
R_RISCV_SUB32        | 32-bit label subtraction   |                     | word32 = S - A
R_RISCV_SUB64        | 64-bit label subtraction   |                     | word64 = S - A
R_RISCV_GNU_VTINHERIT| GNU C++ vtable hierarchy   |                     |
R_RISCV_GNU_VTENTRY  | GNU C++ vtable member usage|                     |
R_RISCV_ALIGN        | Alignment statement        |                     |
R_RISCV_RVC_BRANCH   | PC-relative branch offset  |                     | CB-Type (c.beqz,c.bnez)
R_RISCV_RVC_JUMP     | PC-relative jump offset    |                     | CJ-Type (c.j)
R_RISCV_RVC_LUI      | Absolute address           |                     | CI-Type (c.lui)
R_RISCV_GPREL_I      | PC-relative reference      | %gprel(symbol)      | I-Type (lb,lbu,lh,lhu,lw,lwu,flw,fld,addi,addiw)
R_RISCV_GPREL_S      | PC-relative reference      | %gprel(symbol)      | S-Type (sb,sh,sw,fsw,fsd)

The following table provides details on the variables used in address calculation:

Variable       | Description
:------------- | :----------------
A              | addend field in the relocation entry associated with the symbol
B              | base address of a shared object loaded into memory
G              | offset of the symbol into the GOT (Global Offset Table)
S              | the value of the symbol in the symbol table

# Program Header Table

# Note Sections

# Dynamic Table

# Hash Table

# Thread Local Storage

The following table lists the thread local storage models:

Mnemonic | Model          | Compiler flags
:------- | :---------     | :-------------------
TLS LE   | Local Exec     | `-ftls-model=local-exec`
TLS IE   | Initial Exec   | `-ftls-model=initial-exec`
TLS GD   | Global Dynamic | `-ftls-model=global-dynamic`
