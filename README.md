cat << 'EOF' > README.md
# CHIP-8 Emulator (C++ & SDL2)

This is a simple CHIP-8 emulator written in C++ with SDL2 for graphics. I built this project to better understand how emulators work, including memory management, opcode decoding, and real-time input handling.

---

## Features

- Full support for standard CHIP-8 opcodes
- 64x32 pixel monochrome display (scaled up)
- Basic keyboard input mapped to CHIP-8 keys
- ROM loading from command line
- Delay and sound timer implementation

---

## Getting Started

### Requirements

- g++ with C++17 support
- SDL2
- Make

### Build

Clone the repo and run:

    make

This should build an executable named `chip8`.

---

## Usage

To run a ROM:

    ./chip8 path/to/ROM.ch8

**Note:** ROMs are not included in this repo. 

---

## File Structure

    chip8_emulator_cpp/
    ├── chip8.hpp / chip8.cpp   // Core emulator implementation
    ├── main.cpp                // SDL setup, main loop
    ├── fontset.hpp             // Built-in font sprites
    ├── Makefile
    └── README.md

---

## Notes

- I used SDL2 for graphics and input. You can swap it out for other backends if needed.
- This emulator runs best with simpler ROMs designed for the original CHIP-8 spec.

---

## License

MIT — feel free to fork and modify.
EOF
