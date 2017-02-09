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

RISC-V is a classical RISC architecture that has densely packed non-word
sized instruction immediate values. While the linker can make relocations on
arbitrary memory locations, many of the RISC-V relocations are designed for
use with specific instructions or instruction sequences. RISC-V has several
instruction specific encodings for PC-Relative address loading, jumps,
branches and the RVC compressed instruction set.

The purpose of this section is to describe the RISC-V specific instruction
sequences with their associated relocations in addition to the general purpose
machine word sized relocations that are used for symbol addresses in the
Global Offset Table or DWARF meta data.

The following table provides details of the RISC-V ELF relocations (instruction
specific relocations show the instruction type in the Details column):

Enum | ELF Reloc Type       | Description                     | Details
:--- | :------------------  | :---------------                | :-----------
0    | R_RISCV_NONE         | None                            |
1    | R_RISCV_32           | Runtime relocation              | word32 = S + A
2    | R_RISCV_64           | Runtime relocation              | word64 = S + A
3    | R_RISCV_RELATIVE     | Runtime relocation              | word32,64 = B + A
4    | R_RISCV_COPY         | Runtime relocation              | must be in executable. not allowed in shared library
5    | R_RISCV_JUMP_SLOT    | Runtime relocation              | word32,64 = S ;handled by PLT unless LD_BIND_NOW
6    | R_RISCV_TLS_DTPMOD32 | TLS relocation                  | word32 = S->TLSINDEX
7    | R_RISCV_TLS_DTPMOD64 | TLS relocation                  | word64 = S->TLSINDEX
8    | R_RISCV_TLS_DTPREL32 | TLS relocation                  | word32 = TLS + S + A - TLS_TP_OFFSET
9    | R_RISCV_TLS_DTPREL64 | TLS relocation                  | word64 = TLS + S + A - TLS_TP_OFFSET
10   | R_RISCV_TLS_TPREL32  | TLS relocation                  | word32 = TLS + S + A + S_TLS_OFFSET - TLS_DTV_OFFSET
11   | R_RISCV_TLS_TPREL64  | TLS relocation                  | word64 = TLS + S + A + S_TLS_OFFSET - TLS_DTV_OFFSET
16   | R_RISCV_BRANCH       | PC-relative branch              | (SB-Type)
17   | R_RISCV_JAL          | PC-relative jump                | (UJ-Type)
18   | R_RISCV_CALL         | PC-relative call                | MACRO call,tail (auipc+jalr pair)
19   | R_RISCV_CALL_PLT     | PC-relative call (PLT)          | MACRO call,tail (auipc+jalr pair) PIC
20   | R_RISCV_GOT_HI20     | PC-relative GOT reference       | MACRO la
21   | R_RISCV_TLS_GOT_HI20 | PC-relative TLS IE GOT offset   | MACRO la.tls.ie
22   | R_RISCV_TLS_GD_HI20  | PC-relative TLS GD reference    | MACRO la.tls.gd
23   | R_RISCV_PCREL_HI20   | PC-relative reference           | %pcrel_hi(symbol) (U-Type)
24   | R_RISCV_PCREL_LO12_I | PC-relative reference           | %pcrel_lo(symbol) (I-Type)
25   | R_RISCV_PCREL_LO12_S | PC-relative reference           | %pcrel_lo(symbol) (S-Type)
26   | R_RISCV_HI20         | Absolute address                | %hi(symbol) (U-Type)
27   | R_RISCV_LO12_I       | Absolute address                | %lo(symbol) (I-Type)
28   | R_RISCV_LO12_S       | Absolute address                | %lo(symbol) (S-Type)
29   | R_RISCV_TPREL_HI20   | TLS LE thread offset            | %tprel_hi(symbol) (U-Type)
30   | R_RISCV_TPREL_LO12_I | TLS LE thread offset            | %tprel_lo(symbol) (I-Type)
31   | R_RISCV_TPREL_LO12_S | TLS LE thread offset            | %tprel_lo(symbol) (S-Type)
32   | R_RISCV_TPREL_ADD    | TLS LE thread usage             | %tprel_add(symbol)
33   | R_RISCV_ADD8         | 8-bit label addition            | word8 = S + A
34   | R_RISCV_ADD16        | 16-bit label addition           | word16 = S + A
35   | R_RISCV_ADD32        | 32-bit label addition           | word32 = S + A
36   | R_RISCV_ADD64        | 64-bit label addition           | word64 = S + A
37   | R_RISCV_SUB8         | 8-bit label subtraction         | word8 = S - A
38   | R_RISCV_SUB16        | 16-bit label subtraction        | word16 = S - A
39   | R_RISCV_SUB32        | 32-bit label subtraction        | word32 = S - A
40   | R_RISCV_SUB64        | 64-bit label subtraction        | word64 = S - A
41   | R_RISCV_GNU_VTINHERIT| GNU C++ vtable hierarchy        |
42   | R_RISCV_GNU_VTENTRY  | GNU C++ vtable member usage     |
43   | R_RISCV_ALIGN        | Alignment statement             |
44   | R_RISCV_RVC_BRANCH   | PC-relative branch offset       | (CB-Type)
45   | R_RISCV_RVC_JUMP     | PC-relative jump offset         | (CJ-Type)
46   | R_RISCV_RVC_LUI      | Absolute address                | (CI-Type)
47   | R_RISCV_GPREL_I      | GP-relative reference           | (I-Type)
48   | R_RISCV_GPREL_S      | GP-relative reference           | (S-Type)
49   | R_RISCV_TPREL_I      | TP-relative TLS LE load         | (I-Type)
50   | R_RISCV_TPREL_S      | TP-relative TLS LE store        | (S-Type)
51   | R_RISCV_RELAX        | Instruction pair can be relaxed |
52   | R_RISCV_SUB6         | Local label subtraction         |
53   | R_RISCV_SET6         | Local label subtraction         |
54   | R_RISCV_SET8         | Local label subtraction         |
55   | R_RISCV_SET16        | Local label subtraction         |
56   | R_RISCV_SET32        | Local label subtraction         |


### Assembler Relocation Functions

The following table lists assembler functions for emitting relocatable symbol references:

Assembler Notation       | Description                          | Instruction / Macro
:----------------------  | :---------------                     | :-------------------
%hi(symbol)              | Absolute (HI20)                      | lui
%lo(symbol)              | Absolute (LO12)                      | load, store, add
%pcrel_hi(symbol)        | PC-relative (HI20)                   | auipc
%pcrel_lo(label)         | PC-relative (LO12)                   | load, store, add
%tls_ie_pcrel_hi(symbol) | PC-relative TLS IE GOT reference     | la.tls.ie (auipc+{ld,lw})
%tls_gd_pcrel_hi(symbol) | PC-relative TLS GD reference         | la.tls.gd (auipc+addi)
%tprel_hi(symbol)        | TLS LE thread offset (U-Type)        | auipc
%tprel_lo(symbol)        | TLS LE thread offset (I-Type,S-Type) | load, store
%tprel_add(offset)       | TLS LE thread offset (Add)           | add


### Address Calculation Symbols

The following table provides details on the variables used in address calculation:

Variable       | Description
:------------- | :----------------
A              | Addend field in the relocation entry associated with the symbol
B              | Base address of a shared object loaded into memory
G              | Offset of the symbol into the GOT (Global Offset Table)
S              | Value of the symbol in the symbol table
GP             | Global Pointer register (x3)


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

The following assembly and relocations show loading an absolute address:

```
     lui  a0,%hi(symbol)     # R_RISCV_HI20 (symbol)
     addi a0,a0,%lo(symbol)  # R_RISCV_LO12 (symbol)
```


### Global Offset Table

For position independent code in dynamically linked objects, each shared
object contains a GOT (Global Offset Table) which contains addresses of
global symbols (objects and functions) referred to by the dynamically
linked shared object. The GOT in each shared library is filled in by the
dynamic linker during program loading, or on the first call to extern functions.

To avoid runtime relocations within the text segment of position independent
code the GOT is used for indirection. Instead of code loading virtual addresses
directly, as can be done in static code, addresses are loaded from the GOT.
The allows runtime binding to external objects and functions at the expense of a
slightly higher runtime overhead for access to extern objects and functions.


### Program Linkage Table

The PLT (Program Linkage Table) exists to allow function calls between
dynamically linked shared objects. Each dynamic object has its own
GOT (Global Offset Table) and PLT (Program Linkage Table).

The first entry of a shared object PLT is a special entry that calls
`_dl_runtime_resolve` to resolve the GOT offset for the called function.
The `_dl_runtime_resolve` function in the dynamic loader resolves the
GOT offsets lazily on the first call to any function, except when
`LD_BIND_NOW` is set in which case the GOT entries are populated by the
dynamic linker before the executable is started. Lazy resolution of GOT
entries is intended to speed up program loading by deferring symbol
resolution to the first time the function is called. The first entry
in the PLT occupies two 16 byte entries:

```
1:   auipc  t2, %pcrel_hi(.got.plt)
     sub    t1, t1, t3               # shifted .got.plt offset + hdr size + 12
     l[w|d] t3, %pcrel_lo(1b)(t2)    # _dl_runtime_resolve
     addi   t1, t1, -(hdr size + 12) # shifted .got.plt offset
     addi   t0, t2, %lo(.got.plt)    # &.got.plt
     srli   t1, t1, log2(16/PTRSIZE) # .got.plt offset
     l[w|d] t0, PTRSIZE(t0)          # link map
     jr     t3
```

Subsequent function entry stubs in the PLT take up 16 bytes and load a
function pointer from the GOT. On the first call to a function, the
entry redirects to the first PLT entry which calls `_dl_runtime_resolve`
and fills in the GOT entry for subsequent calls to the function:

```
1:  auipc   t3, %pcrel_hi(function@.got.plt)
    l[w|d]  t3, %pcrel_lo(1b)(t3)
    jalr    t1, t3
    nop
```


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

The pseudo instruction:

```
    call symbol
```

expands to the following assembly and relocation:

```
    auipc t0, 0      # R_RISCV_CALL (symbol), R_RISCV_RELAX (symbol)
    jalr  ra, t0, 0
```

and when `-fpic` is enabled it expands to:

```
    auipc t0, 0      # R_RISCV_CALL_PLT (symbol), R_RISCV_RELAX (symbol)
    jalr  ra, t0, 0
```


### PC-Relative Jumps and Branches

Unconditional jump (UJ-Type) instructions have a `R_RISCV_JAL` relocation
that can represent an even signed 21-bit offset (-2MiB to +2MiB-1).

Branch (SB-Type) instructions have a `R_RISCV_BRANCH` relocation that
can represent an even signed 13-bit offset (-4096 to +4095).


### PC-Relative Symbol Addresses

32-bit PC-relative relocations for symbol addresses on sequences of
instructions such as the `AUIPC+ADDI` instruction pair expanded from
the `la` pseudo-instruction, in position independent code typically
have an associated pair of relocations: `R_RISCV_PCREL_HI20` plus
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

Note the compiler emitted instructions for PC-relative symbol addresses are
not necessarily sequential or in pairs. There is a constraint is that the
instruction with the `RISCV_R_PCREL_LO12_I` or `RISCV_R_PCREL_LO12_S`
relocation label points to a valid HI20 PC-relative relocation pointing to
the symbol. e.g.

```
label:
   auipc t0, %pcrel_hi(sym)      # t0 := label + (sym - label + 0x800)[31:12]
   lui t1, 1
   lw t2, t0, %pcrel_lo(label)   # t0 := t0 + (sym - label)[11:0]
   add t2, t2, t1
   sw t2, t0, %pcrel_lo(label)   # t0 := t0 + (sym - label)[11:0]
```

## Thread Local Storage

RISC-V adopts the ELF Thread Local Storage Model in which ELF objects define
`.tbss` and `.tdata` sections and `PT_TLS` program headers that contain the
TLS "initialization images" for new threads. The `.tbss` and `.tdata` sections
are not referenced directly like regular segments, rather they are copied or
allocated to the thread local storage space of newly created threads.
See [https://www.akkadia.org/drepper/tls.pdf](https://www.akkadia.org/drepper/tls.pdf).

In The ELF Thread Local Storage Model, TLS offsets are used instead of pointers.
The ELF TLS sections are initialization images for the thread local variables of
each new thread. A TLS offset defines an offset into the dynamic thread vector
which is pointed to by the TCB (Thread Control Block) held in the `tp` register.

There are various thread local storage models for statically allocated or
dynamically allocated thread local storage. The following table lists the
thread local storage models:

Mnemonic | Model          | Compiler flags
:------- | :---------     | :-------------------
TLS LE   | Local Exec     | `-ftls-model=local-exec`
TLS IE   | Initial Exec   | `-ftls-model=initial-exec`
TLS LD   | Local Dynamic  | `-ftls-model=local-dynamic`
TLS GD   | Global Dynamic | `-ftls-model=global-dynamic`

The program linker in the case of static TLS or the dynamic linker in the case
of dynamic TLS allocate TLS offsets for storage of thread local variables.


### Local Exec

Local exec is a form of static thread local storage. This model is used
when static linking as the TLS offsets are resolved during program linking.

- Compiler flag `-ftls-model=local-exec`
- Variable attribute: `__thread int i __attribute__((tls_model("local-exec")));`

Example assembler load and store of a thread local variable `i` using the
`%tprel_hi`, `%tprel_add` and `%tprel_lo` assembler functions. The emitted
relocations are in comments.

```
  lui  a5,%tprel_hi(i)         # R_RISCV_TPREL_HI20 (symbol)
  add  a5,a5,tp,%tprel_add(i)  # R_RISCV_TPREL_ADD (symbol)
  lw   t0,%tprel_lo(i)(a5)     # R_RISCV_TPREL_LO12_I (symbol)
  addi t0,a0,1
  sw   t0,%tprel_lo(i)(a5)     # R_RISCV_TPREL_LO12_S (symbol)
```

The `%tprel_add` assembler function does not return a value and is used purely
to associate the `R_RISCV_TPREL_ADD` relocation with the `add` instruction.


### Initial Exec

Initial exec is is a form of static thread local storage that can be used in
shared libraries that use thread local storage. TLS relocations are performed
at load time. `dlopen` calls to libraries that use thread local storage may fail
when using the initial exec thread local storage model as TLS offsets must all
be resolved at load time. This model uses the GOT to resolve TLS offsets.

- Compiler flag `-ftls-model=initial-exec`
- Variable attribute: `__thread int i __attribute__((tls_model("initial-exec")));`
- ELF flags: DF_STATIC_TLS

Example assembler load and store of a thread local variable `i` using the
`la.tls.ie` psuedo-instruction, with the emitted TLS relocations in comments:

```
   la.tls.ie a5,i
   add  a5,a5,tp
   lw   t0,0(a5)
   addi t0,a0,1
   sw   t0,0(a5)
```

The assembler pseudo instruction:

```
   la.tls.ie a5,symbol
```

expands to the following assembly instructions and relocations:

```
label:
   auipc a5, 0       # R_RISCV_TLS_GOT_HI20 (symbol)
   {ld,lw} a5, 0(a5) # R_RISCV_PCREL_LO12_I (label)
```


### Global Dynamic

RISC-V local dynamic and global dynamic TLS models generate equivalent object code.
The Global dynamic thread local storage model is used for PIC Shared libraries and
handles the case where more than one library uses thread local variables, and
additionally allows libraries may be loaded and unloaded at runtime using `dlopen`.
In the global dynamic model, application code calls the dynamic linker function
`__tls_get_addr` to locate TLS offsets into the dynamic thread vector at runtime.

- Compiler flag `-ftls-model=global-dynamic`
- Variable attribute: `__thread int i __attribute__((tls_model("global-dynamic")));`

Example assembler load and store of a thread local variable `i` using the
`la.tls.gd` pseudo-instruction, with the emitted TLS relocations in comments:

```
  la.tls.gd a0,i
  call  __tls_get_addr@plt
  mv   a5,a0
  lw   t0,0(a5)
  addi t0,a0,1
  sw   t0,0(a5)
```

The assembler pseudo instruction:

```
   la.tls.gd a0,symbol
```

expands to the following assembly instructions and relocations:

```
label:
   auipc a0,0        # R_RISCV_TLS_GD_HI20 (symbol)
   addi  a0,a0,0     # R_RISCV_PCREL_LO12_I (label)
```

In the Global Dynamic model, the runtime library provides the `__tls_get_addr` function:

```
extern void *__tls_get_addr (tls_index *ti);
```

where the type tls index are defined as:

```
typedef struct
{
  unsigned long int ti_module;
  unsigned long int ti_offset;
} tls_index;
```


# Program Header Table

# Note Sections

# Dynamic Table

# Hash Table

