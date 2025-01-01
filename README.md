# CHIP-8 Emulator

This is a simple CHIP-8 emulator build with the help of the high-level guide written by Tobias [V. Langhoff](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/).

## Specifications

- Memory: 4 kilobytes of RAM.
- Display: 64 x 32 pixels monochrome.
- Program counter: `PC` pointer.
- Index Register: 16-bit register named `I`.
- Stack: It should support 16-bit addresses.
- An 8-bit delay timer with a rate of 60 Hz.
- An 8-bit sound timer.
- 16 8-bit general-purpose registers. (_V0_-_VF_).
