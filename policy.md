# Policy for Merging Pull Requests

Each type of modification has a different policy, based on the following rules:

- Changes requiring linker changes
  - Require an open source PoC implementation for binutils or LLD, either as a
    patch on the mailing list/Phabricator as appropriate or in a GitHub fork
  - Require at least one binutils developer **_AND_** one LLD developer to
    approve

- Changes requiring compiler changes
  - Require an open source PoC implementation for GCC or LLVM, either as a
    patch on the mailing list/Phabricator as appropriate or in a GitHub fork
  - Require at least one GCC developer **_AND_** one LLVM developer to approve

- Clarifications for currently-implemented behaviour
  - Require approval from a developer of the corresponding component
    (binutils/LLD or GCC/LLVM)

- General improvements and clarification
  - One of the psABI TG chair or co-chair.

- Do **_NOT_** make incompatible changes
  - Changes that break compatibility are generally not acceptable
  - In the rare case there is a bug in the ABI that needs fixing and that
    cannot be done in a backwards-compatible way, or possibly for some
    edge-case behaviour that is not currently relied upon, breaking the ABI can
    be considered, but will require both the psABI TG chair and co-chair to
    approve, and is subject to the above requirements as appropriate

# FAQ

- Can I leave a comment, LGTM or approve the PR even if I am not a toolchain
  developer or chair/co-chair?
  - Don't hesitate to leave your comment, we encourage anyone who intends
    to contribute to the RISC-V community to participate in discussion.

- When do I need to modify the compiler and/or linker?
  - Changes and additions to the ELF format itself generally require linker
    changes, e.g. new relocation types, new flags in the `e_flags` field, new
    sections and new symbol flags.
  - Changes and additions to calling conventions and code models generally
    require compiler changes.

- Who are the psABI TG chair and co-chair?
  - The current chair is Kito Cheng ([@kito-cheng]) and the current co-chair is
    Jessica Clarke ([@jrtc27]).

- Where can I find a RISC-V GCC/LLVM/binutils/LLD developer to review my PR?
  - The psABI TG chair or co-chair will generally contact the right people as
    needed for reviews, but in case you want to reach out yourself you can find
    an incomplete list from [RISC-V International's wiki page].

# Reserving Encodings

- Relocations within the standard encoding space, or program header types etc.,
  may be reserved for in-development standard extensions
  - Such extensions must be under development by an existing TG and be well on
    the way to ratification (such as, but not strictly required, be in the
    stable state)
  - A proof-of-concept toolchain ideally should exist
  - A specification for the ABI additions ideally should exist
  - The nature of extensions can vary, and so the policy here is intentionally
    vague and incomplete; each request will be evaluated on a case-by-case
    basis by the psABI community
  - Encoding reservations are voluntary and non-binding; whilst the psABI
    community intends to honour reasonable requests, it reserves the right to
    stop reserving such encodings for any reason

[@kito-cheng]: https://github.com/kito-cheng
[@jrtc27]: https://github.com/jrtc27
[RISC-V International's wiki page]: https://wiki.riscv.org/display/TECH/Toolchain+Projects
