# CHIP-8 Emulator

This is a simple CHIP-8 emulator build with the help of the language-agnostic high-level guide written by Tobias [V. Langhoff](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/).

## Features

- Full CHIP-8 instruction set implementation.
- Support for both CHIP-8 and SUPER-CHIP.
- Configurable CPU clock speed.
- SDL-based display.

## Specifications

- Memory: 4KB (4096 bytes) of RAM.
- Display: 64 x 32 pixel monochrome display.
- Registers:
    - 16 8-bit general-purpose registers (**V0**-**VF**).
    - A 16-bit index register (**I**).
    - A 16-bit program counter (**PC**).
- Stack:
    - Support for 16-bit addresses.
    - 16 levels for subroutine calls.
- Timers:
    - An 8-bit delay timer (60 Hz).
    - An 8-bit sound timer (60 Hz).
- Input:
    - 16-key hexadecimal keypad.

## Default Key Mapping

```bash
Original   ->  Keyboard
1 2 3 C    ->  1 2 3 4
4 5 6 D    ->  Q W E R
7 8 9 E    ->  A S D F
A 0 B F    ->  Z X C V
```

## Building

### Prerequisites
- C compiler (gcc/clang)
- SDL2 library
- Make

### Compilation
```bash
make
```

## Usage

```bash
./chip8 -r ROM_FILE [options]

# Options:
#   -c SPEED     Set CPU clock speed (instructions per second)
#   -o           Enable original CHIP-8 behavior
```

## Acknowledgements

- [Tobias V. Langhoff](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/) for the excellent CHIP-8 guide.
- [Austin Morlan](https://austinmorlan.com/posts/chip8_emulator/#chip-8-description) for the detailed instructions on making this emulator using C++.
- [Ivan Garcia](https://github.com/Klairm) for his CHIP-8 repo.
