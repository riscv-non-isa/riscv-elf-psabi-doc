# Policy of Merge Pull Request

Each type of modification require different policy, the policy is based on
following rules:

- Changes require linker change.
  - Require an open source PoC implmenation for `binutils` or `LLD`
    - Available on either mailing list, phabricator or github repo is acceptable.
  - Require at least one binutils developer **_AND_** one LLD developer to
    approve.

- Changes require compiler change.
  - Require an open source PoC implmenation for `GCC` or `LLVM`
    - Available on either mailing list, phabricator or github repo is acceptable.
  - Require at least one GCC developer **_AND_** one LLVM developer to approve.

- Clarification on current implmenation behavior.
  - Require approve from a developer of corresponding component.

- General document improvement and clarification.
  - One of the psABI TG chair or co-chair.

- Do **_NOT_** make incompatible change.
  - Incompatible are generally unacceptable.
  - In case, it's a ABI bug fix or a very conner case, it require all
    psABI TG chair or co-chair to approve.

# FAQ

- Could I leave comment, LGTM or approve the PR even if I am not a toolchain developer or chair/co-chair?
  - Don't hesitate to leave your comment, we are encourage anyone who intend
    to contribute to the RISC-V community.

- When do I need to modify compiler and/or linker?
  - Change or add new stuffs to the ELF format require linker
    change, e.g. new relocation type, new flags for e_flags field.
  - Change or add new ABI, new calling convention or new code model require compiler
    change.

- Who is psABI TG chair/co-chair
  - psABI TG chair and co-chair are election by RISC-V international,
    current chair is Kito Cheng ([@kito-cheng](https://github.com/kito-cheng)) and
    co-chair is Jessica Clarke ([@jrtc27](https://github.com/jrtc27)).

- Where can I find a RISC-V GCC/LLVM/binutils/LLD developers to review my PR?
  - psABI TG chair/co-chair will contact right people to review generally, but in
    case you want to reach out those developer you could found an incomplete list
    from [RISC-V International's wiki page](https://wiki.riscv.org/display/TECH/Toolchain+Projects).
