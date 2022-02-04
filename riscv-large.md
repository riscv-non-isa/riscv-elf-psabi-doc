# RISC-V Large Code Model

#### Evandro Menezes
#### evandro.menezes@sifive.com


## Introduction

Code models are based on the portion of the total address space that may be reached.  The native range of instructions usually determines the code models.  The range of instructions such as branches and loads may determine different code models to reach code and data in the address space.

When the code or data of a program is within the native range of instructions, it is deemed "small". When the code or data of a program is without the native range of instructions, it is deemed "large". Code models are sometimes defined as:

| Code models    | Small code | Large code
| ---            | ---        | ---
| **Small data** | Small      | Medium
| **Large data** | Compact    | Large


## RISC-V Data Models

| Data models | Address space | Code addressing range | Data addressing range
| ---         | ---           | ---                   | ---
| **ILP32**   | 4GiB          | 4GiB                  | 4GiB
| **LP64**    | 16EiB         | 2GiB                  | 4GiB

For ILP32, the whole address space is within the native range for both code and data. Therefore, the small code model is sufficient.

For LP64, only 2GiB of the address space is within the native range for code and 4GiB for data. The existing small code models, `medlow` & `medany`, are not sufficient.


## RISC-V LP64 Code Models

In order to access more than 2GiB of code and 4GiB of data, the native instructions are not sufficient:
- More than 2GiB of code and 4GiB of data can only be reached indirectly via a 64 bit register.
- 64 bit constants are not natively supported by the RV64I instructions.
- Forming 64 bit constants using several instructions is not optimal.


## RV64 Large Model

This proposal for a large code model includes the case when there is more than 2GiB of code and 2GiB of initialized data, allowing for only the former or the latter at one time or both at the same time.

Since the global pointer is the anchor to access global data, the `gp` register is thus required to be preserved across function calls.  The global data area contains all the initialized data and ancillary data structures, such as the GOT and the PLT.  Its total maximum size is 2GiB.

If the global variable has local scope, it is allocated in or adjacent to the global data area. The limit for data of local scope is 2GiB. It is accessed relative to the global pointer.

If the global variable has global scope, it may be allocated anywhere in the address space. There is no size limit for data of global scope. It is accessed indirectly through a GOT entry that contains its address, with a corresponding `R_RISCV_64` or `R_RISCV_RELATIVE` relocation for the entry. When the program is loaded, the dynamic linker should process this relocation. In the absence of a loader, the linker records the symbol value in the GOT entry. Note that then the GOT is used for both non PIC and PIC objects.

If the function has global scope, it may be allocated anywhere in the address space.  There is no size limit for code.  It is accessed through a stub in a PLT entry.  Note that then the PLT is used for both non PIC and PIC objects.

These new relocation functions are added to the assembler:
- `%gprel_hi(<symbol>)`, `%gprel_lo(<symbol>)`: offset from the global pointer to the symbol.
- `%got_gprel_hi(<symbol>)`, `%got_gprel_lo(<symbol>)`: offset from the global pointer to the GOT entry for the symbol.
- `%plt_gprel_hi(<symbol>)`, `%plt_gprel_lo(<symbol>)`: offset from the global pointer to the PLT entry for the symbol.
- `tls_gd_gprel_hi(<symbol>)`, `tls_gd_gprel_lo(<symbol>)`: offset from the global pointer to the GD for the symbol.
- `tls_ie_gprel_hi(<symbol>)`, `tls_ie_gprel_lo(<symbol>)`: offset from the global pointer to the TLS entry for the symbol.
- `%gprel(<symbol>)`, `%gprel_add(<symbol>)`, `%got_gprel(<symbol>)`, `%got_gprel_add(<symbol>)`, `%plt_gprel(<symbol>)`, `%plt_gprel_add(<symbol>)`, `tls_gd_gprel_add(<symbol>)`, `tls_ie_gprel_add(<symbol>)`: note instruction for relaxation purposes.

These new relocation types are added:

| Relocation type                | Calculation[^1] | Notes
| ---                            | ---             | ---
| `R_RISCV_GPREL_HI20`           | S + A - GP      | `%gprel_hi(<symbol>)`
| `R_RISCV_GPREL_LO12_I`[^2]     | S + A - GP      | `%gprel_lo(<symbol>)`
| `R_RISCV_GPREL_LO12_S`[^2]     | S + A - GP      | `%gprel_lo(<symbol>)`
| `R_RISCV_GPREL_ADD`            |                 | `%gprel_add(<symbol>)`[^3]
| `R_RISCV_GPREL_LOAD`           |                 | `%gprel(<symbol>)`[^3]
| `R_RISCV_GPREL_STORE`          |                 | `%gprel(<symbol>)`[^3]
| `R_RISCV_GOT_GPREL_HI20`       | G + A - GP      | `%got_gprel_hi(<symbol>)`
| `R_RISCV_GOT_GPREL_LO12_I`     | G + A - GP      | `%got_gprel_lo(<symbol>)`
| `R_RISCV_GOT_GPREL_ADD`        |                 | `%got_gprel_add(<symbol>)`[^3]
| `R_RISCV_GOT_GPREL_LOAD`       |                 | `%got_gprel(<symbol>)`[^3]
| `R_RISCV_GOT_GPREL_STORE`      |                 | `%got_gprel(<symbol>)`[^3]
| `R_RISCV_TLS_GOT_GPREL_HI20`   |                 | Macro `la.tls.ie.gprel`
| `R_RISCV_TLS_GOT_GPREL_LO12_I` |                 | Macro `la.tls.ie.gprel`
| `R_RISCV_TLS_GOT_GPREL_ADD`    |                 | `%tls_ie_gprel(<symbol>)`[^3]
| `R_RISCV_TLS_GD_GPREL_HI20`    |                 | Macro `la.tls.gd.gprel`
| `R_RISCV_TLS_GD_GPREL_LO12_I`  |                 | Macro `la.tls.gd.gprel`
| `R_RISCV_TLS_GD_GPREL_ADD`     |                 | `%tls_gd_gprel(<symbol>)`[^3]
| `R_RISCV_64_PCREL`             | S + A - P       |
| `R_RISCV_PLT_GPREL_HI20`       | L + A - GP      | `%plt_gprel_hi(<symbol>)`
| `R_RISCV_PLT_GPREL_LO12_I`     | L + A - GP      | `%plt_gprel_lo(<symbol>)`
| `R_RISCV_PLT_GPREL_ADD`        |                 | `%plt_gprel_add(<symbol>)`[^3]

[^1]: Legend for the relocation calculations:
  _A_: the addend used to compute the value of the relocatable field.
  _G_: the address of the GOT entry where the value of the symbol will reside.
  _GP_: represents the address of the global data area.
  _L_: the addres of the PLT entry where the stub of the symbol will reside.
  _P_: the place (section offset or address) of the relocation.
  _S_: the value of the symbol.
[^2]: The relocation types `R_RISCV_GPREL_LO12_I` and `R_RISCV_GPREL_LO12_S` are relaxed into `R_RISCV_GPREL_I` and `R_RISCV_GPREL_S` respectively.
[^3]: The first source operand (`rs1`) has the address of the global data area.


## Large Model Coding Examples

### Function Prologue

Functions in executable objects expect that the `gp` register was initialized to the global pointer at program start up and the `gp` may be used as the base register to access objects in the global data area.

Functions in shared objects that refer to the global pointer must setup an arbitrary register in its prologue instead which must be used as the base register to access objects in the global data area.  For example:

```assembly
  auipc	s0, %pcrel_hi(__global_pointer__)
  addi	s0, s0, %pcrel_lo(__global_pointer__)
  ld	t0, 0(s0)
  add	s0, s0, t0
  ...
  .section .text.__global_pointer__, "aMG", @progbits, 8, __global_pointer__, comdat
  .align 3
  .global __global_pointer__
  .hidden __global_pointer__
  .type	__global_pointer__, object
__global_pointer__:
  .quad	__global_pointer$ -.
```

The expression `__global_pointer$ - .` results in the relocation type `R_RISCV_64_PCREL`. This relocation is position independent, provided that the information available to the linker matches the run time environment, therefore it can be used in both executable and shared objects.

Note that the code example above places the literal for the global data area in a `comdat` section that is shared by all references to this literal in the object.


### Program Linkage Table

Each PLT entry follows the pattern below:
```assembly
  lui	t3, %hi(<function>@.got.plt - .got.plt)
  addi	t3, %lo(<function>@.got.plt - .got.plt)
  jal	t1, <stub>@.plt
  nop
```

Where:
- `<function>@.got.plt` is the address of the GOT entry for the respective <function>.
- `<stub>@.plt` is the address of a special stub, occupying three PLT entries, that contains the code below:
```assembly
1:
  auipc	t0, %hi_pcrel(2f)	# address of 2f
  addi	t0, %lo_pcrel(1b)
  ld	t2, (t0)		# difference between .got.plt - 2f
  add	t0, t0, t2		# address of .got.plt
  add	t0, t0, t3		# address of <function>@.got.plt
  ld	t3, (t0)		# address of <function>
  jr	t3
  nop
2:
  .quad	.got.plt - ., 0
```

The stub above may be repeated as needed in order to bring it within range of a referring PLT entry.


### Data Objects

The code examples below assume that the `gp` register, only used in executable objects, points to the global data area.

| Source			| Assembly				| Relocations
| --				| --					| --
| `extern int src;`		| `.extern src`				| `R_RISCV_64`
| `extern int dst;`		| `.extern dst`				| `R_RISCV_64`
| `extern void *ptr;`		| `.extern ptr`				| `R_RISCV_64`
| `static int lsrc;`		| `.comm .Llsrc, 4`			|
| `static int ldst;`		| `.comm .Lldst, 4`			|
| `static void foo(void);`	| `.local foo`				|
|				| `.text`				|
| `dst = src;`			| `lui	t0, %got_gprel_hi(src)`		| `R_RISCV_GOT_GPREL_HI20`
|				| `ld	t0, %got_gprel_lo(src)(t0)`	| `R_RISCV_GOT_GPREL_LO12_I`
|				| `add	t0, gp, t0, %got_gprel(src)`	| `R_RISCV_GOT_GPREL_ADD`
|				| `lw	t2, 0(t0), %got_gprel(src)`	| `R_RISCV_GOT_GPREL_LOAD`
|				| `lui	t1, %got_gprel_hi(dst)`		| `R_RISCV_GOT_GPREL_HI20`
|				| `add	t1, gp, t1, %got_gprel(dst)`	| `R_RISCV_GOT_GPREL_ADD`
|				| `ld	t1, %got_gprel_lo(dst)(t1)`	| `R_RISCV_GOT_GPREL_LO12_I`
|				| `sw	t2, 0(t1), %got_gprel(dst)`	| `R_RISCV_GOT_GPREL_STORE`
| `ptr = &src;`			| `lui	t0, %got_gprel_hi(src)`		| `R_RISCV_GOT_GPREL_HI20`
|				| `add	t0, gp, t0, %got_gprel(src)`	| `R_RISCV_GOT_GPREL_ADD`
|				| `ld	t0, %got_gprel_lo(src)(t0)`	| `R_RISCV_GOT_GPREL_LO12_I`
|				| `lui	t1, %got_gprel_hi(ptr)`		| `R_RISCV_GOT_GPREL_HI20`
|				| `add	t1, gp, t1, %got_gprel(ptr)`	| `R_RISCV_GOT_GPREL_ADD`
|				| `ld	t1, %got_gprel_lo(ptr)(t1)`	| `R_RISCV_GOT_GPREL_LO12_I`
|				| `sd	t0, 0(t1), %got_gprel(ptr)`	| `R_RISCV_GOT_GPREL_STORE`
| `ldst = &lsrc;`		| `lui	t0, %gprel_hi(.Llsrc)`		| `R_RISCV_GPREL_HI20`
|				| `add	t0, gp, t0, %gprel(.Llsrc)`	| `R_RISCV_GPREL_ADD`
|				| `addi	t0, t0, %gprel_lo(.Llsrc)`	| `R_RISCV_GPREL_LO12_I`
|				| `lui	t1, %gprel_hi(.Lldst)`		| `R_RISCV_GPREL_HI20`
|				| `add	t1, gp, t1, %gprel(.Lldst)`	| `R_RISCV_GPREL_ADD`
|				| `sd	t0, %gprel_lo(.Lldst)(t1)`	| `R_RISCV_GPREL_LO12_S`
| `ptr = foo;`			| `la	t0, foo`			| `R_RISCV_PCREL_HI20`
|				|					| `R_RISCV_PCREL_LO12_I`
|				| `lui	t1, %got_gprel_hi(ptr)`		| `R_RISCV_GOT_GPREL_HI20`
|				| `add	t1, gp, t1, %got_gprel(ptr)`	| `R_RISCV_GOT_GPREL_ADD`
|				| `ld	t1, %got_gprel_lo(ptr)(t1)`	| `R_RISCV_GOT_GPREL_LO12_I`
|				| `sd	t0, 0(t1), %got_gprel(ptr)`	| `R_RISCV_GOT_GPREL_STORE`


### Function calls

The code examples below assume that the `gp` register, only used in executable objects, points to the global data area.

| Source			| Assembly				| Relocations
| --				| --					| --
| `auto void (*ptr)();`		|					|
| `extern void foo(void);`	| `.global foo`				|
|				| `.text`				|
| `foo();`			| `lui	t0, %plt_gprel_hi(foo)`		| `R_RISCV_PLT_GPREL_HI20`
|				| `add	t0, gp, t0, %plt_gprel(foo)`	| `R_RISCV_PLT_GPREL_ADD`
|				| `jalr	ra, %plt_gprel_lo(foo)(t0)`	| `R_RISCV_PLT_GPREL_LO12_I`
| `ptr = &foo;`			| `lui	t0, %got_gprel_hi(foo)`		| `R_RISCV_GOT_GPREL_HI20`
|				| `add	t0, gp, t0, %got_gprel(foo)`	| `R_RISCV_GOT_GPREL_ADD`
|				| `ld	a0, %got_gprel_lo(foo)(t0)`	| `R_RISCV_GOT_GPREL_LO12_I`
| `(*ptr)();`			| `jalr	a0`				|


## Pseudo instructions

The cost of the compact code model can be quite significant, so it is important to minimize this cost when the conditions allow for it. This can be done at link time through the addition of special relocation types to allow it. Additionally, additional assembly macros simplify adding such special relocations.


### Address Literals

This macro is used to produce the literal for a local symbol:

```assembly
lla	<rd>, %gprel(<symbol>), <rt>
```

Where:
- `<rd>` is the destination register;
- `<rt>` is the register holding the address of `__global_pointer$`[^1], defaulting to `gp`, if omitted;
- `<symbol>` is the global symbol.
[^1]: `gp` for an executable object or an arbitrary register for a shared object.

Which expands to:

```assembly
lui	<rd>, %gprel_hi(<symbol>)			// R_RISCV_GPREL_HI20 (symbol)
add	<rd>, <rt>, <rd>, %gprel(<symbol>)		// R_RISCV_GPREL_ADD (symbol)
addi	<rd>, <rd>, %gprel_lo(<symbol>)			// R_RISCV_GPREL_LO12_I (symbol)
```

For a global symbol, this macro is used to produce its literal:

```assembly
la	<rd>, %got_gprel(<symbol>), <rt>
```

Where:
- `<rd>` is the destination register;
- `<rt>` is the register holding the address of `__global_pointer$`[^1], defaulting to `gp`, if omitted;
- `<symbol>` is the global symbol.
[^1]: `gp` for an executable object or an arbitrary register for a shared object.

Which expands to:

```assembly
lui	<rd>, %got_gprel_hi(<symbol>)			// R_RISCV_GOT_GPREL_HI20 (symbol)
add	<rd>, <rt>, <rd>, %got_gprel(<symbol>)		// R_RISCV_GOT_GPREL_ADD (symbol)
ld	<rd>, %got_gprel_lo(<symbol>)(<rd>)		// R_RISCV_GOT_GPREL_LO12_I (symbol)
```

If the global symbol is allocated within the global data area and is referenced in an executable object, then this macro is equivalent to:

```assembly
lla	<rd>, %gprel(<symbol>), <rt>
```


### Loads and Stores

In an executable object, for loading from or storing to a local symbol, these macros are used:

```assembly
l{b|h|w|d} <rd>, %gprel(<symbol>)
s{b|h|w|d} <rd>, %gprel(<symbol>), <rt>
fl{h|w|d|q} <rd>, %gprel(<symbol>), <rt>
fs{h|w|d|q} <rd>, %gprel(<symbol>), <rt>
```

Where:
- `<rd>` is the destination register;
- `<rt>` is a temporary register;
- `<symbol>` is the local symbol.

Which expand to:

```assembly
lui	<rd>, %gprel_hi(<symbol>)			// R_RISCV_GPREL_HI20 (symbol)
add	<rd>, gp, <rd>, %gprel(<symbol>)		// R_RISCV_GPREL_ADD (symbol)
l{b|h|w|d} <rd>, %gprel_lo(<symbol>)(<rd>)		// R_RISCV_GPREL_LO12_I (symbol)
```

And:

```assembly
lui	<rt>, %gprel_hi(<symbol>)			// R_RISCV_GPREL_HI20 (symbol)
add	<rt>, gp, <rt>, %gprel(<symbol>)		// R_RISCV_GPREL_ADD (symbol)
s{b|h|w|d} <rd>, %gprel_lo(<symbol>)(<rt>)		// R_RISCV_GPREL_LO12_S (symbol)
```

And:

```assembly
lui	<rt>, %gprel_hi(<symbol>)			// R_RISCV_GPREL_HI20 (symbol)
add	<rt>, gp, <rt>, %gprel(<symbol>)		// R_RISCV_GPREL_ADD (symbol)
fl{h|w|d|q} <rd>, %gprel_lo(<symbol>)(<rt>)		// R_RISCV_GPREL_LO12_I (symbol)
```

And:

```assembly
lui	<rt>, %gprel_hi(<symbol>)			// R_RISCV_GPREL_HI20 (symbol)
add	<rt>, gp, <rt>, %gprel(<symbol>)		// R_RISCV_GPREL_ADD (symbol)
fs{h|w|d|q} <rd>, %gprel_lo(<symbol>)(<rt>)		// R_RISCV_GPREL_LO12_S (symbol)
```


### TLS

For TLS global dynamic and local dynamic:

```assembly
la.tls.gd.gprel	<rd>, <symbol>, <rt>
```

Where:
- `<rd>` is the destination register;
- `<rt>` is the register holding the address of `__global_pointer$`[^1], defaulting to `gp`, if omitted;
- `<symbol>` is the symbol.
[^1]: `gp` for an executable object or an arbitrary register for a shared object.

Which expands to:

```assembly
lui	<rd>, %tls_gd_gprel_hi(<symbol>)		// R_RISCV_TLS_GD_GPREL_HI20 (symbol)
add	<rd>, <rt>, <rd>, %tls_gd_gprel(<symbol>)	// R_RISCV_TLS_GD_GPREL_ADD (symbol)
addi	<rd>, <rd>, %tls_gd_gprel_lo(<symbol>)		// R_RISCV_TLS_GD_GPREL_LO12_I (symbol)
```

For TLS initial executable:

```assembly
la.tls.ie.gprel	<rd>, <symbol>, <rt>
```

Where:
- `<rd>` is the destination register;
- `<rt>` is the register holding the address of `__global_pointer$`[^1], defaulting to `gp`, if omitted;
- `<symbol>` is the symbol.
[^1]: `gp` for an executable object or an arbitrary register for a shared object.

Which expands to:

```assembly
lui	<rd>, %tls_ie_gprel_hi(<symbol>)		// R_RISCV_TLS_GOT_GPREL_HI20 (symbol)
add	<rd>, <rt>, <rd>,  %tls_ie_gprel(<symbol>)	// R_RISCV_TLS_GOT_GPREL_ADD (symbol)
addi	<rd>, <rd>, %tls_ie_gprel_lo(<symbol>)		// R_RISCV_TLS_GOT_GPREL_LO12_I (symbol)
```


## Appendix

In the tables below, it is assumed that the register `gp` holds the address of `__global_pointer$` in the executable.

The table below demonstrates the results of relaxation when the global is allocated and referenced in the executable object, assuming that the register `a0` holds the address of `__global_pointer$`::

| Source			| Assembly				| Relocations			| Relaxed				| Relocations
| --				| --					| --				| --					| --
| `extern int src;`		| `.extern src`				|				|					|
| `extern int dst;`		| `.extern dst`				|				|					|
| `extern void *ptr;`		| `.extern ptr`				|				|					|
| `static void foo(void);`	| `.local foo`				|				|					|
|				| `.text`				|				|					|
| `dst = src;`			| `lui	t0, %got_gprel_hi(src)`		| `R_RISCV_GOT_GPREL_HI20`	| `lui	t0, %gprel_hi(src)`		| `R_RISCV_GPREL_HI20`
|				| `add	t0, a0, t0, %got_gprel(src)`	| `R_RISCV_GOT_GPREL_ADD`	| `add	t0, gp, t0`			|
|				| `ld	t0, %got_gprel_lo(src)(t0)`	| `R_RISCV_GOT_GPREL_LO12_I`	| `addi	t0, t0, %gprel_lo(src)`		| `R_RISCV_GPREL_LO12_I`
|				| `lw	t2, 0(t0), %got_gprel(src)`	| `R_RISCV_GOT_GPREL_LOAD`	| `lw	t2, 0(t0)`			|
|				| `lui	t1, %got_gprel_hi(dst)`		| `R_RISCV_GOT_GPREL_HI20`	| `lui	t1, %gprel_hi(dst)`		| `R_RISCV_GPREL_HI20`
|				| `add	t1, a0, t1, %got_gprel(dst)`	| `R_RISCV_GOT_GPREL_ADD`	| `add	t1, gp, t1`			|
|				| `ld	t1, %got_gprel_lo(dst)(t1)`	| `R_RISCV_GOT_GPREL_LO12_I`	| `addi	t1, t1, %gprel_lo(dst)`		| `R_RISCV_GPREL_LO12_I `
|				| `sw	t2, 0(t1), %got_gprel(dst)`	| `R_RISCV_GOT_GPREL_STORE`	| `sw	t2, 0(t1)`			|
| `ptr = &src;`			| `lui	t0, %got_gprel_hi(src)`		| `R_RISCV_GOT_GPREL_HI20`	| `lui	t0, %gprel_hi(src)`		| `R_RISCV_GPREL_HI20`
|				| `add	t0, a0, t0, %got_gprel(src)`	| `R_RISCV_GOT_GPREL_ADD`	| `add	t0, gp, t0`			|
|				| `ld	t0, %got_gprel_lo(src)(t0)`	| `R_RISCV_GOT_GPREL_LO12_I`	| `addi	t0, t0, %gprel_lo(src)`		| `R_RISCV_GPREL_LO12_I `
|				| `lui	t1, %got_gprel_hi(ptr)`		| `R_RISCV_GOT_GPREL_HI20`	| `lui	t1, %gprel_hi(ptr)`		| `R_RISCV_GPREL_HI20`
|				| `add	t1, a0, t1, %got_gprel(ptr)` 	| `R_RISCV_GOT_GPREL_ADD`	| `add	t1, gp, t1`			|
|				| `ld	t1, %got_gprel_lo(ptr)(t1)`	| `R_RISCV_GOT_GPREL_LO12_I`	| `addi	t1, t1, %gprel_lo(ptr)`		| `R_RISCV_GPREL_LO12_I `
|				| `sd	t0, 0(t1), %got_gprel(ptr)`	| `R_RISCV_GOT_GPREL_STORE`	| `sd	t0, 0(t1)`			|
| `ptr = foo;`			| `la	t0, foo`			| `R_RISCV_PCREL_HI20`		| `la	t0, foo` 			| `R_RISCV_PCREL_HI20`
|				|					| `R_RISCV_PCREL_LO12_I`	|					| `R_RISCV_PCREL_LO12_I`
|				| `lui	t1, %got_gprel_hi(ptr)`		| `R_RISCV_GOT_GPREL_HI20`	| `lui	t1, %gprel_hi(ptr)`		| `R_RISCV_GPREL_HI20`
|				| `add	t1, a0, t1, %got_gprel(ptr)`	| `R_RISCV_GOT_GPREL_ADD`	| `add	t1, gp, t1`			|
|				| `ld	t1, %got_gprel_lo(ptr)(t1)`	| `R_RISCV_GOT_GPREL_LO12_I`	| `addi	t1, t1, %gprel_lo(ptr)`	| `R_RISCV_PCREL_LO12_I`
|				| `sd	t0, 0(t1), %got_gprel(ptr)`	| `R_RISCV_GOT_GPREL_STORE`	| `sd	t0, 0(t1)`			|

The table below demonstrates the results of relaxation when the GOT entry for the global is in the vicinity of the global data area in the executable object, assuming that the register `s0` holds the address of `__global_pointer$`:

| Source			| Assembly				| Relocations			| Relaxed				| Relocations
| --				| --					| --				| -- 					| --
| `extern int src;`		| `.extern src`				|				|					|
| `extern int dst;`		| `.extern dst`				|				|					|
| `extern void *ptr;`		| `.extern ptr`				|				|					|
| `static void foo(void);`	| `.local foo`				|				|					|
|				| `.text`				|				|					|
| `dst = src;`			| `lui	t0, %got_gprel_hi(src)`		| `R_RISCV_GOT_GPREL_HI20`	|					|
|				| `add	t1, s0, t0, %got_gprel(src)`	| `R_RISCV_GOT_GPREL_ADD`	|					|
|				| `ld	t2, %got_gprel_lo(src)(t1)`	| `R_RISCV_GOT_GPREL_LO12_I`	| `ld t2, %got_gprel_lo(src)(gp)`	| `R_RISCV_GOT_GPREL_LO12_I`
|				| `lw	t3, 0(t2), %got_gprel(src)`	| `R_RISCV_GOT_GPREL_LOAD`	| `lw t3, 0(t2)`			|
|				| `lui	t4, %got_gprel_hi(dst)`		| `R_RISCV_GOT_GPREL_HI20`	|					|
|				| `add	t5, s0, t4, %got_gprel(dst)`	| `R_RISCV_GOT_GPREL_ADD`	|					|
|				| `ld	t6, %got_gprel_lo(dst)(t5)`	| `R_RISCV_GOT_GPREL_LO12_I`	| `ld t6, %got_gprel_lo(dst)(gp)`	| `R_RISCV_GOT_GPREL_LO12_I`
|				| `sw	t3, 0(t6), %got_gprel(dst)`	| `R_RISCV_GOT_GPREL_STORE`	| `sw t3, 0(t6)`			|
| `ptr = &src;`			| `lui	t0, %got_gprel_hi(src)`		| `R_RISCV_GOT_GPREL_HI20`	|					|
|				| `add 	t1, s0, t0, %got_gprel(src)`	| `R_RISCV_GOT_GPREL_ADD`	|					|
|				| `ld	t2, %got_gprel_lo(src)(t1)`	| `R_RISCV_GOT_GPREL_LO12_I`	| `ld t2, %got_gprel_lo(src)(gp)`	| `R_RISCV_GOT_GPREL_LO12_I`
|				| `lui 	t3, %got_gprel_hi(ptr)`		| `R_RISCV_GOT_GPREL_HI20`	|					|
|				| `add 	t4, s0, t3,  %got_gprel(ptr)`	| `R_RISCV_GOT_GPREL_ADD`	|					|
|				| `ld 	t5, %got_gprel_lo(ptr)(t4)`	| `R_RISCV_GOT_GPREL_LO12_I`	| `ld t5, %got_gprel_lo(ptr)(gp)`	| `R_RISCV_GOT_GPREL_LO12_I`
|				| `sd 	t2, 0(t5), %got_gprel(ptr)`	| `R_RISCV_GOT_GPREL_STORE`	| `sd t2, 0(t5)`			|
| `ptr = foo;`			| `la	t0, foo`			| `R_RISCV_PCREL_HI20`		| `la t0, foo`				| `R_RISCV_PCREL_HI20`
|				|					| `R_RISCV_PCREL_LO12_I`	|					| `R_RISCV_PCREL_LO12_I`
|				| `lui	t1, %got_gprel_hi(ptr)`		| `R_RISCV_GOT_GPREL_HI20`	|					|
|				| `add	t2, s0, t1, %got_gprel(ptr)`	| `R_RISCV_GOT_GPREL_ADD`	|					|
|				| `ld	t3, %got_gprel_lo(ptr)(t2)`	| `R_RISCV_GOT_GPREL_LO12`	| `ld t3, %got_gprel_lo(ptr)(gp)`	| `R_RISCV_GOT_GPREL_LO12_I`
|				| `sd	t0, 0(t3), %got_gprel(ptr)`	| `R_RISCV_GOT_GPREL_STORE`	| `sd t0, 0(t3)`			|

The table below demonstrates the results of relaxation when the PLT entry for the function is in the vicinity of the global data area in the executable object, assuming that the register `a1` holds the address of `__global_pointer$`:

| Source			| Assembly				| Relocations			| Relaxed				| Relocations
| --				| --					| --				| --					| --
| `extern void foo(void);`	| `.global foo`				|				|					|
|				| `.text`				|				|					|
| `foo();`			| `lui	t0, %plt_gprel_hi(foo)`		| `R_RISCV_PLT_GPREL_HI20`	|					|
|				| `add	t0, a1, t0, %plt_gprel(foo)`	| `R_RISCV_PLT_GPREL_ADD`	|					|
|				| `jalr	ra, %plt_gprel_lo(foo)(t0)`	| `R_RISCV_PLT_GPREL_LO12_I`	| `jalr	ra, %plt_gprel_lo(foo)(gp)`	| `R_RISCV_PLT_GPREL_LO12_I`

The table below demonstrates the results of relaxation when the global is allocated in the global data area and referenced in the executable object, assuming that the register `s1` holds the address of `__global_pointer$`:

| Source			| Assembly				| Relocations			| Relaxed				| Relocations
| --				| --					| --				| --					| --
| `extern int src;`		| `.extern src`				|				|					|
| `extern int dst;`		| `.extern dst`				|				|					|
| `static int lsrc;`		|					|				|					|
| `static int ldst;`		|					|				|					|
| `extern void *ptr;`		| `.extern ptr`				|				|					|
| `static void foo(void);`	| `.local foo`				|				|					|
|				| `.text`				|				|					|
| `dst = src;`			| `lui	t0, %got_gprel_hi(src)`		| `R_RISCV_GOT_GPREL_HI20`	|					|
|				| `add	t0, s1, t0, %got_gprel(src)`	| `R_RISCV_GOT_GPREL_ADD`	|					|
|				| `ld	t0, %got_gprel_lo(src)(t0)`	| `R_RISCV_GOT_GPREL_LO12`	| `addi	t0, gp, %gprel_lo(src)`		| `R_RISCV_GPREL_I`
|				| `lw	t2, 0(t0), %got_gprel(src)`	| `R_RISCV_GOT_GPREL_LOAD`	| `lw	t2, %gprel_lo(src)(gp)`		| `R_RISCV_GPREL_I`
|				| `lui	t1, %got_gprel_hi(dst)`		| `R_RISCV_GOT_GPREL_HI20`	|					|
|				| `add	t1, s1, t1, %got_gprel(dst)`	| `R_RISCV_GOT_GPREL_ADD`	|					|
|				| `ld	t1, %got_gprel_lo(dst)(t1)`	| `R_RISCV_GOT_GPREL_LO12`	| `addi	t1, gp, %gprel_lo(dst)`		| `R_RISCV_GPREL_I`
|				| `sw	t2, 0(t1), %got_gprel(dst)`	| `R_RISCV_GOT_GPREL_STOR`	| `sw	t2, %gprel_lo(dst)(gp)`		| `R_RISCV_GPREL_S`
| `ptr = &src;`			| `lui	t0, %got_gprel_hi(src)`		| `R_RISCV_GOT_GPREL_HI20`	|					|
|				| `add	t0, s1, t0, %got_gprel(src)`	| `R_RISCV_GOT_GPREL_ADD`	|					|
|				| `ld	t0, %got_gprel_lo(src)(t0)`	| `R_RISCV_GOT_GPREL_LO12_I`	| `addi	t0, gp, %gprel_lo(src)`		| `R_RISCV_GPREL_I`
|				| `lui	t1, %got_gprel_hi(ptr)`		| `R_RISCV_GOT_GPREL_HI20`	|					|
|				| `add	t1, s1, t1, %got_gprel(ptr)`	| `R_RISCV_GOT_GPREL_ADD`	|					|
|				| `ld	t1, %got_gprel_lo(ptr)(t1)`	| `R_RISCV_GOT_GPREL_LO12_I`	| `addi	t1, gp, %gprel_lo(ptr)`		| `R_RISCV_GPREL_I`
|				| `sd	t0, 0(t1), %got_gprel(ptr)`	| `R_RISCV_GOT_GPREL_STORE`	| `sd	t0, %gprel_lo(ptr)(gp)`		| `R_RISCV_GPREL_S`
| `ldst = &lsrc;`		| `lui	t0, %gprel_hi(.Llsrc)`		| `R_RISCV_GPREL_HI20`		|					|
|				| `add	t0, s1, t0, %gprel(.Llsrc)`	| `R_RISCV_GPREL_ADD`		|					|
|				| `addi	t0, t0, %gprel_lo(.Llsrc)`	| `R_RISCV_GPREL_LO12_I`	| `addi	t0, gp, %gprel_lo(.Llsrc)`	| `R_RISCV_GPREL_I`
|				| `lui	t1, %gprel_hi(.Lldst)`		| `R_RISCV_GPREL_HI20`		|					|
|				| `add	t1, s1, t1, %gprel(.Lldst)`	| `R_RISCV_GPREL_ADD`		| `addi	t1, gp, %gprel_lo(.Lldst)`	| `R_RISCV_GPREL_I`
|				| `sd	t0, %gprel_lo(.Lldst)(t1)`	| `R_RISCV_GPREL_LO12_S`	| `sd	t0, %gprel_lo(.Lldst)(gp)`	| `R_RISCV_GPREL_S`
| `ptr = foo;`			| `la	t0, foo`			| `R_RISCV_PCREL_HI20`		| `la	t0, foo`			| `R_RISCV_PCREL_HI20`
|				|					| `R_RISCV_PCREL_LO12_I`	|					| `R_RISCV_PCREL_LO12_I`
|				| `lui	t1, %got_gprel_hi(ptr)	`	| `R_RISCV_GOT_GPREL_HI20`	|					|
|				| `add	t1, s1, t1, %got_gprel(ptr)`	| `R_RISCV_GOT_GPREL_ADD`	|					|
|				| `ld	t1, %got_gprel_lo(ptr)(t1)`	| `R_RISCV_GOT_GPREL_LO12`	| `addi	t1, gp, %gprel_lo(ptr)`		| `R_RISCV_GPREL_I`
|				| `sd	t0, 0(t1), %got_gprel(ptr)`	| `R_RISCV_GOT_GPREL_STORE`	| `sd	t0, %gprel_lo(ptr)(gp)`		| `R_RISCV_GPREL_S`
