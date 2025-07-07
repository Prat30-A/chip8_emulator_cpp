#include <cstdint>
#pragma once
#include <string>
using namespace std;

// Constants for display resolution
constexpr int DISPLAY_WIDTH = 64;
constexpr int DISPLAY_HEIGHT = 32;

class Chip8{
    public:
    //constructor
        Chip8();
    //declare a variable for the OpCode
        uint16_t opcode ;
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
        uint32_t display[64*32]{};
    // Each element is 0 (not pressed) or 1 (pressed)
        uint8_t keypad[16]{};
    //loadrom function to open a .ch8 file
        bool load_ROM(const string& filename);
    //fetch the opcode
        uint16_t fetchOpcode();
    //clear screen OPCODE
        void OP_00E0();
    //return from a subroutine
        void OP_00EE();
    //Jumps to address NNN
        void OP_1NNN();
    //Call subroutine at NNN
        void OP_2NNN();
    //skip instruction if V[x] = NN
        void OP_3XNN();
    //skips instruction ifv[x] != NN
        void OP_4XNN();
    //skips instructon if v[x] == v[y]
        void OP_5XY0();
    //set V[x] ==NN
        void OP_6XNN();
    //Add NN to VX 
        void OP_7XNN();
    //set Vx to Vy
        void OP_8XY0();
    //set Vx to Vx or Vy
        void OP_8XY1();
    //set Vx to Vx and Vy
        void OP_8XY2();
    //set Vx to Vx XOR vy
        void OP_8XY3();
    //Check for overflow in addition
        void OP_8XY4();
    //Check for underflow in subtraction
        void OP_8XY5();
    //shift Vx and store it in VF
        void OP_8XY6();
    //check for underflow in subtraction Vx = Vy - Vx
        void OP_8XY7();
    //shift Vx to the left by 1
        void OP_8XYE();
    //check if VX != Vy and skips
        void OP_9XY0();
    //set Index to value of NNN
        void OP_ANNN();
    //jump to address NNN + V0
        void OP_BNNN();
    //set Vx to rand() num and NN
        void OP_CXNN();
    //display
        void OP_DXYN();
    //skip instruction if key is pressed
        void OP_EX9E();
    //skip instruction if key is not pressed
        void OP_EXA1();
    //vx == delay timer
        void OP_FX07();
    //Vx == pressed key
        void OP_FX0A();
    //set delay timer to Vx
        void OP_FX15();
    //set sound timer to Vx
        void OP_FX18();
    //add Vx to I
        void OP_FX1E();
    //Sets I to the location of the sprite for the character
        void OP_FX29();
    //Store BCD representation of Vx in memory locations I, I+1, and I+2.
        void OP_FX33();
    //Stores from V0 to VX (including VX) in memory
        void OP_FX55();
    //Fills from V0 to VX (including VX) with values from memory
        void OP_FX65();
};