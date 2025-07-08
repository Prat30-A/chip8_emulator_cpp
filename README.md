# CHIP-8 Emulator (C++ & SDL2)

A simple CHIP-8 emulator built using C++ and SDL2. This project helped me learn about system-level programming, emulation, and graphics rendering using a minimal virtual machine.

## Features

- Full support for standard CHIP-8 opcodes
- 64×32 monochrome display rendered using SDL2
- Keyboard input mapped to CHIP-8 hex keys
- Load ROMs via command-line arguments
- Implements delay and sound timers at 60Hz
- Compatible with classic CHIP-8 games

## Requirements

- A C++17-compatible compiler (e.g. `g++`)
- [SDL2](https://github.com/libsdl-org/SDL/releases) development libraries
- (Optional) `make` if you prefer using a Makefile for building

> This project was developed and tested on Windows using MinGW-w64 and SDL2 version 2.32.8.

## Build Instructions

### On Windows (MinGW + SDL2)
1. Download the [SDL2 development library for MinGW-w64](https://github.com/libsdl-org/SDL/release) and extract it.
2. Make sure your `g++` compiler is installed and added to your system PATH.
3. Compile using the following command (adjust paths if needed):
## Setup & Usage

1. **Fork & Clone**  
   Fork this repo, then clone your fork locally:
   ```bash
   git clone https://github.com/<your-username>/chip8_emulator_cpp.git
   cd chip8_emulator_cpp
    ```
2. **Build**
    ```bash
    g++ main.cpp chip8.cpp -o chip8.exe -IC:\SDL2-2.32.8\x86_64-w64-mingw32\include -LC:\SDL2-2.32.8\x86_64-w64-mingw32\lib -lmingw32 -lSDL2main -lSDL2
    ```
3. **Copy the SDL2 Runtime DLL**
    After building, copy SDL2.dll next to your chip8.exe

4. **Place a ROM in roms/**
    Make sure you have a ROM in roms/:
    
5. **Run with a Relative Path**
    From the project root (where chip8.exe is), launch:
    ```bash
    .\chip8.exe .\roms\"NameofRom".ch8
    ```

## Keyboard Mapping:
```
Original  | Keyboard
---------------------
1 2 3 C   | 1 2 3 4
4 5 6 D   | Q W E R
7 8 9 E   | A S D F
A 0 B F   | Z X C V
```

## File Structure

    chip8_emulator_cpp/
    ├── roms              #store roms in this file
    ├── chip8.hpp         # Core emulator implementation header
    ├── chip8.cpp         # Core emulator implementation source
    ├── main.cpp          # SDL2 setup and main loop
    ├── SDL2.dll          # Windows runtime dependency
    └── README.md         # Project documentation


### Notes
This emulator is intended for learning and runs best with simple ROMs using the original CHIP-8 spec.
You can replace SDL2 with other rendering/input libraries if you prefer.
> Tested with WinLibs standalone MinGW-w64 GCC 14.1.0 on Windows.

