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

Enum | ELF Reloc Type       | Description                         | Details
:--- | :------------------  | :---------------                    | :-----------
0    | R_RISCV_NONE         | None                                |
1    | R_RISCV_32           | Runtime relocation                  | word32 = S + A
2    | R_RISCV_64           | Runtime relocation                  | word64 = S + A
3    | R_RISCV_RELATIVE     | Runtime relocation                  | word32,64 = B + A
4    | R_RISCV_COPY         | Runtime relocation                  | must be in executable. not allowed in shared library
5    | R_RISCV_JUMP_SLOT    | Runtime relocation                  | word32,64 = S ;handled by PLT unless LD_BIND_NOW
6    | R_RISCV_TLS_DTPMOD32 | TLS relocation                      | word32 = S->TLSINDEX
7    | R_RISCV_TLS_DTPMOD64 | TLS relocation                      | word64 = S->TLSINDEX
8    | R_RISCV_TLS_DTPREL32 | TLS relocation                      | word32 = TLS + S + A - TLS_TP_OFFSET
9    | R_RISCV_TLS_DTPREL64 | TLS relocation                      | word64 = TLS + S + A - TLS_TP_OFFSET
10   | R_RISCV_TLS_TPREL32  | TLS relocation                      | word32 = TLS + S + A + S_TLS_OFFSET - TLS_DTV_OFFSET
11   | R_RISCV_TLS_TPREL64  | TLS relocation                      | word64 = TLS + S + A + S_TLS_OFFSET - TLS_DTV_OFFSET
16   | R_RISCV_BRANCH       | PC-relative branch offset           | SB-Type
17   | R_RISCV_JAL          | PC-relative jump offset             | UJ-Type
18   | R_RISCV_CALL         | PC-relative function call           | MACRO call,tail (auipc+jalr pair)
19   | R_RISCV_CALL_PLT     | PC-relative function call           | MACRO call,tail (auipc+jalr pair) PIC
20   | R_RISCV_GOT_HI20     | PC-relative GOT offset              | MACRO la
21   | R_RISCV_TLS_GOT_HI20 | PC-relative TLS IE GOT reference    | MACRO la.tls.ie %tls_ie_pcrel_hi(x)
22   | R_RISCV_TLS_GD_HI20  | PC-relative TLS GD GOT reference    | MACRO la.tls.gd %tls_gd_pcrel_hi(x)
23   | R_RISCV_PCREL_HI20   | PC-relative reference  (U-Type)     | %pcrel_hi(symbol)
24   | R_RISCV_PCREL_LO12_I | PC-relative reference (I-Type)      | %pcrel_lo(symbol)
25   | R_RISCV_PCREL_LO12_S | PC-relative reference (S-Type)      | %pcrel_lo(symbol)
26   | R_RISCV_HI20         | Absolute address (U-Type)           | %hi(symbol)
27   | R_RISCV_LO12_I       | Absolute address (I-Type)           | %lo(symbol)
28   | R_RISCV_LO12_S       | Absolute address (S-Type)           | %lo(symbol)
29   | R_RISCV_TPREL_HI20   | TLS LE thread offset (U-Type)       | %tprel_hi(symbol)
30   | R_RISCV_TPREL_LO12_I | TLS LE thread offset (I-Type)       | %tprel_lo(symbol)
31   | R_RISCV_TPREL_LO12_S | TLS LE thread offset (S-Type)       | %tprel_lo(symbol)
32   | R_RISCV_TPREL_ADD    | TLS LE thread usage                 | %tprel_add(symbol)
33   | R_RISCV_ADD8         | 8-bit label addition                | word8 = S + A
34   | R_RISCV_ADD16        | 16-bit label addition               | word16 = S + A
35   | R_RISCV_ADD32        | 32-bit label addition               | word32 = S + A
36   | R_RISCV_ADD64        | 64-bit label addition               | word64 = S + A
37   | R_RISCV_SUB8         | 8-bit label subtraction             | word8 = S - A
38   | R_RISCV_SUB16        | 16-bit label subtraction            | word16 = S - A
39   | R_RISCV_SUB32        | 32-bit label subtraction            | word32 = S - A
40   | R_RISCV_SUB64        | 64-bit label subtraction            | word64 = S - A
41   | R_RISCV_GNU_VTINHERIT| GNU C++ vtable hierarchy            |
42   | R_RISCV_GNU_VTENTRY  | GNU C++ vtable member usage         |
43   | R_RISCV_ALIGN        | Alignment statement                 |
44   | R_RISCV_RVC_BRANCH   | PC-relative branch offset (CB-Type) |
45   | R_RISCV_RVC_JUMP     | PC-relative jump offset (CJ-Type)   |
46   | R_RISCV_RVC_LUI      | Absolute address (CI-Type)          |
47   | R_RISCV_GPREL_I      | GP-relative reference (I-Type)      |
48   | R_RISCV_GPREL_S      | GP-relative reference (S-Type)      |
49   | R_RISCV_TPREL_I      | TP-relative TLS LE load (I-Type)    |
50   | R_RISCV_TPREL_S      | TP-relative TLS LE store (S-Type)   |
51   | R_RISCV_RELAX        | Instruction pair can be relaxed     | (auipc+jalr pair)
52   | R_RISCV_SUB6         | Local label subtraction             |
53   | R_RISCV_SET6         | Local label subtraction             |
54   | R_RISCV_SET8         | Local label subtraction             |
55   | R_RISCV_SET16        | Local label subtraction             |
56   | R_RISCV_SET32        | Local label subtraction             |

The following table provides details on the variables used in address calculation:

Variable       | Description
:------------- | :----------------
A              | Addend field in the relocation entry associated with the symbol
B              | Base address of a shared object loaded into memory
G              | Offset of the symbol into the GOT (Global Offset Table)
S              | Value of the symbol in the symbol table
GP             | Global Pointer register (x3)

While the linker can make relocations on arbitrary memory locations, many
relocations are designed for use with specific instructions or instruction
sequences. For clarity, the description of those relocations assumes they
are used in the intended context.

### Absolute Addresses

32-bit absolute addresses in position dependent code are loaded with a pair
of instructions which have an associated pair of relocations:
`R_RISCV_HI20` plus `R_RISCV_LO12_I` or `R_RISCV_LO12_S`.

The `R_RISCV_HI20` refers to an `LUI` instruction containing the high
20-bits to be relocated to an absolute symbol address. The `LUI` instruction
is followed by an I-Type instruction (add immediate or load) with an
`R_RISCV_LO12_I` relocation or an S-Type instruction (store) and an
`R_RISCV_LO12_S` relocation. The addresses for pair of relocations are
calculated like this:

 - `hi20 = ((symbol_address + 0x800) >> 12);`
 - `lo12 = symbol_address - hi20;`

### Procedure Calls

`R_RISCV_CALL` or `R_RISCV_CALL_PLT` and `R_RISCV_RELAX` relocations are
associated with pairs of instructions (`AUIPC+JALR`) generated by the `CALL`
or `TAIL` pseudo instructions.

In position dependent code (`-fno-pic`) the `AUIPC` instruction in the
`AUIPC+JALR` pair has both a `R_RISCV_CALL` relocation and a `R_RISCV_RELAX`
relocation indicating the instruction sequence can be relaxed during linking.

In position independent code (`-fPIC`, `-fpic` or `-fpie`) the `AUIPC`
instruction in the `AUIPC+JALR` pair has both a `R_RISCV_CALL_PLT` relocation
and a `R_RISCV_RELAX` relocation indicating the instruction sequence can be
relaxed during linking.

Procedure call linker relaxation allows the `AUIPC+JALR` pair to be relaxed
to the `JAL` instruction when the prodecure or PLT entry is within (-2MiB to
+2MiB-1) of the instruction pair.

### PC-Relative Jumps and Branches

Unconditional jump (UJ-Type) instructions have a `R_RISCV_JAL` relocation
that can represent an even signed 21-bit offset (-2MiB to +2MiB-1).

Branch (SB-Type) instructions have a `R_RISCV_BRANCH` relocation that
can represent an even signed 13-bit offset (-4096 to +4095).

### PC-Relative Symbol Addresses

32-bit PC-relative relocations for symbol addresses on pairs of instructions
such as the `AUIPC+ADDI` instruction pair expanded from the `la`
pseudo-instruction, in position independent code typically have an
associated pair of relocations: `R_RISCV_PCREL_HI20` plus
`R_RISCV_PCREL_LO12_I` or `R_RISCV_PCREL_LO12_S`.

The `R_RISCV_PCREL_HI20` relocation refers to an `AUIPC` instruction
containing the high 20-bits to be relocated to a symbol relative to the
program counter address of the `AUIPC` instruction. The `AUIPC`
instruction is followed by an I-Type instruction (add immediate or load)
with an `R_RISCV_PCREL_LO12_I` relocation or an S-Type instruction (store)
and an `R_RISCV_PCREL_LO12_S` relocation.

The `R_RISCV_PCREL_LO12_I` or `R_RISCV_PCREL_LO12_S` relocations contain
a label pointing to an instruction with a `R_RISCV_PCREL_HI20` relocation
entry that points to the target symbol:

 - At label: `R_RISCV_PCREL_HI20` relocation entry ⟶ symbol
 - `R_RISCV_PCREL_LO12_I` relocation entry ⟶ label

To get the symbol address to perform the calculation to fill the 12-bit
immediate on the add, load or store instruction the linker finds the
`R_RISCV_PCREL_HI20` relocation entry associated with the `AUIPC`
instruction. The addresses for pair of relocations are calculated like this:

 - `hi20 = ((symbol_address - hi20_reloc_offset + 0x800) >> 12);`
 - `lo12 = symbol_address - hi20_reloc_offset - hi20;`

The successive instruction has a signed 12-bit immediate so the value of the
preceding high 20-bit relocation may have 1 added to it.

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
