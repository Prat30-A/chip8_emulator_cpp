#include <cstdint>
#pragma once
#include <string>
using namespace std;

class Chip8{
    public:
    //constructor
        Chip8();
    // 4KB memory (CHIP-8 has 4096 bytes of RAM)
        uint8_t memory[4096]{};
    // 16 general-purpose 8-bit registers: V0 to VF
        uint8_t V[16]{};
    // Index register 16 bit used for memory address
        uint16_t index_register;
    // Program Counter (PC) - 16-bit, starts at 0x200
        uint16_t PC{};
    // Stack to store return addresses for function calls 
        uint16_t stack[16]{};
    // Stack Pointer - points to top of the stack
        uint8_t SP{};
     // Delay timer - decrements at 60Hz until zero
        uint8_t delay_timer{};
    // Sound timer - beeps when non-zero, decrements at 60Hz
        uint8_t sound_timer{};
    // Monochrome display: 64 pixels wide × 32 pixels high
        uint8_t display[64*32]{};
    // Each element is 0 (not pressed) or 1 (pressed)
        uint8_t keypad[16]{};
    //loadrom function to open a .ch8 file
        bool load_ROM(const string& filename);
    //fetch the opcode
        uint16_t fetchOpcode();
    //clear screen OPCODE
        uint16_t OP_00E0();
        
};