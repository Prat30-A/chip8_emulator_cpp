#include <iostream>
#include "chip8.hpp"
#include <cstring>
#include <fstream>
#include <vector>

using namespace std;

const uint8_t chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0 in binary for the upper bits
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
//define a constructor
Chip8::Chip8() {
    //set pc to 0x200
    PC = 0x200;

    //clear memory, registers, stack, display, keypad
    memset(memory, 0, sizeof(memory));
    memset(V, 0, sizeof(V));
    memset(stack, 0, sizeof(stack));
    memset(display, 0, sizeof(display));
    memset(keypad, 0, sizeof(keypad));

    index_register = 0;
    SP = 0;
    delay_timer = 0;
    sound_timer = 0;

    //load fontset into memory starting from 0x50
    for(int i = 0; i<80; i++){
        memory[i+0x50] = chip8_fontset[i];
    }
}

bool Chip8::load_ROM(const string& filename){
    //open the file as a binary
    ifstream input_stream(filename, ios::binary);
    if(!input_stream){
        //throw an error if the file cant be opened
        cerr << "Failed to open ROM file: " << filename << endl;
        return false;
    }

    //find the number of bytes at the start
    streampos begin = input_stream.tellg();
    //go to the end of the file
    input_stream.seekg(0, ios::end);
    //find the number of bytes at the end
    streampos end = input_stream.tellg();
    //get total bytes 
    int bytes = end - begin;
    //go back to the start
    input_stream.seekg(0, ios::beg);

    //get the size of the rom
    if(bytes > (4096 - 0x200)){
        cerr << "ROM too large to fit in the memory.\n";
        return false;
    }
    //create a buffer to store the memory 
    vector<char> buffer(bytes);
    //read the file bytes into the buffer
    input_stream.read(buffer.data(), bytes);
    //copy the ROM into memory at 0x200
    memcpy(&memory[0x200], buffer.data(), bytes);
    //close the file
    input_stream.close();
    return true;
}

uint16_t Chip8::fetchOpcode(){
    // Shift the first byte to the high 8 bits and OR with the next byte
    uint16_t opcode = (memory[PC] << 8) | memory[PC+1];

    //Move to the next instruction as each instruction is 2 bytes 
    PC += 2;
    return opcode;
}

