#include <iostream>
#include "chip8.hpp"
#include <cstring>
#include <fstream>
#include <vector>
#include <cstdlib>

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
    opcode = (memory[PC] << 8) | memory[PC+1];

    //Move to the next instruction as each instruction is 2 bytes 
    PC += 2;
    return opcode;
}

//opcode for clearing display
void Chip8::OP_00E0(){
    memset(display,0,sizeof(display));
}

void Chip8::OP_00EE(){
    //decrement stack ptr
    SP--;
    //set PC to the new SP
    PC = stack[SP];
}

//opcode to jump to the NNN address
void Chip8::OP_1NNN(){
    //get the lower 12 bits 
    uint16_t address = opcode & 0x0FFF;
    //Set the PC to the jump address
    PC = address;
}

//opcode to call subroutine at NNN
void Chip8::OP_2NNN(){
    //save the return address on the stack
    stack[SP] = PC;
    //move to a next stack position
    SP++;
    //set PC as the address of NNN
    PC = opcode & 0x0FFF;
}

void Chip8::OP_3XNN(){
    //Extract NN
    uint8_t NN = (opcode & 0x00FF);
    //Extract X from opcode, shift right 
    uint8_t X = (opcode & 0x0F00) >> 8;
    if (V[X] == NN){
        PC+=2; //skip the next instruction
    }
}

void Chip8::OP_4XNN(){
    //Extract NN
    uint8_t NN = (opcode & 0x00FF);

    //Extract X from opcode, shift right
    uint8_t X = (opcode & 0x0F00) >> 8;
    if (V[X] != NN){
        PC+=2; //skip the next instruction
    }
}

void Chip8::OP_5XY0(){
    //Extract X from opcode and shift right 8 bits
    uint8_t X = (opcode & 0x0F00) >> 8;
    //Extract Y from opcode and shift right 4 bits
    uint8_t Y = (opcode & 0x00F0) >> 4;

    if(V[X] == V[Y]){
        PC+=2;
    }
}

void Chip8::OP_6XNN(){
    //Extract X
    uint8_t X = (opcode & 0x0F00) >> 8;
    //Extract NN
    uint8_t NN = opcode & 0x00FF;
    //set V[x] to NN
    V[X] = NN;
}

void Chip8::OP_7XNN(){
    //Extract X
    uint8_t X = (opcode & 0x0F00) >> 8;
    //Extract NN
    uint8_t NN = opcode & 0x00FF;
    //Add V[X] to NN
    V[X] += NN;
}

void Chip8::OP_8XY0(){
    //Extract X
    uint8_t X = (opcode & 0x0F00) >> 8;
    //Extract Y 
    uint8_t Y = (opcode & 0x00F0) >> 4;
    //set Vx to Vy
    V[X] = V[Y];
}

void Chip8::OP_8XY1(){
    //Extract X
    uint8_t X = (opcode & 0x0F00) >> 8;
    //Extract Y
    uint8_t Y = (opcode & 0x00F0) >> 4;
    //set Vx to either Vx to Vy
    V[X] = V[X] | V[Y];
}

void Chip8::OP_8XY2(){
    //Extract X
    uint8_t X = (opcode & 0x0F00) >> 8;
    //Extract Y
    uint8_t Y = (opcode & 0x00F0) >> 4;
    //set Vx and Vy
    V[X] = V[X] & V[Y];
}

void Chip8::OP_8XY3(){
    //Extract X
    uint8_t X = (opcode & 0x0F00) >> 8;
    //Extract Y
    uint8_t Y = (opcode & 0x00F0) >> 4;
    //set Vx to Vx XOR Vy
    V[X] = V[X] ^ V[Y];
}

void Chip8::OP_8XY4(){
    //Extract X
    uint8_t X = (opcode & 0x0F00) >> 8;
    
    uint8_t Y = (opcode & 0x00F0) >> 4;
    //add Vx to Vy and check for overflow
    uint16_t sum = V[X] + V[Y];
    if(sum > 255){
        V[0xF] = 1;
    }
    else{
        V[0xF] = 0;
    }
    //after setting V[F] do the addition
    V[X] = sum & 0xFF;
}

void Chip8::OP_8XY5(){
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0x00F0) >> 4;
    if(V[X] >= V[Y]){
        V[0xF] = 1;
    }
    else{
        V[0xF] = 0;
    }
    V[X] = V[X] - V[Y];
}

void Chip8::OP_8XY6(){
    uint8_t X = (opcode & 0x0F00) >> 8;
    //Find the LSB
    V[0xF] = V[X] & 0x1;
    //shift Vx by 1 bit
    V[X]= V[X] >> 1;
}

void Chip8::OP_8XY7(){
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0x00F0) >> 4;

    //check Vy greater than equal to Vx
    if(V[Y] >= V[X]){
        //no undeflow
        V[0xF] = 1;
    }
    else{
        V[0xF] = 0;
    }

    V[X] = V[Y] - V[X];
}   

void Chip8::OP_8XYE(){
    uint8_t X = (opcode & 0x0F00) >> 8;
    //set VF to VX MSB prior to shift
    V[0xF] = (V[X] & 0x80) >> 7;
    //shift VX by 1 bit
    V[X] = V[X] << 1;
}

void Chip8::OP_9XY0(){
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0x00F0) >> 4;

    if (V[X] != V[Y]){
        PC+=2;
    }
}

void Chip8::OP_ANNN(){
    uint16_t NNN = (opcode & 0x0FFF);
    index_register = NNN;
}

void Chip8::OP_BNNN(){
    uint16_t NNN = (opcode & 0x0FFF);
    PC = V[0] + NNN;
}

void Chip8::OP_CXNN(){
    uint8_t NN = opcode & 0x00FF;
    uint8_t X = (opcode & 0x0F00) >> 8;
    V[X] = (rand()%256) & NN;
}

void Chip8::OP_DXYN() {
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0x00F0) >> 4;
    uint8_t N = opcode & 0x000F;

    uint8_t xPos = V[X] % DISPLAY_WIDTH;
    uint8_t yPos = V[Y] % DISPLAY_HEIGHT;

    V[0xF] = 0;

    for (int row = 0; row < N; row++) {
        uint8_t spriteByte = memory[index_register + row];

        for (int col = 0; col < 8; col++) {
            if ((spriteByte & (0x80 >> col)) == 0) continue;

            uint8_t x = (xPos + col) % DISPLAY_WIDTH;
            uint8_t y = (yPos + row) % DISPLAY_HEIGHT;
            uint16_t index = y * DISPLAY_WIDTH + x;

            if (index >= DISPLAY_WIDTH * DISPLAY_HEIGHT) continue;  // Optional safety

            if (display[index] == 1) {
                V[0xF] = 1;  // Collision
            }

            display[index] ^= 1;  // Toggle pixel (black & white only)
        }
    }
}


void Chip8::OP_EX9E(){
    uint8_t X = (opcode & 0x0F00) >> 8;
    //if keypad is pressed skip the next instruction
    if (keypad[V[X]]){
        PC+=2;
    }
}

void Chip8::OP_EXA1(){
    uint8_t X = (opcode & 0x0F00) >> 8;
    if (!keypad[V[X]]){
        PC+=2;
    }
}

void Chip8::OP_FX07(){
    uint8_t X = (opcode & 0x0F00) >> 8;
    V[X] = delay_timer;
}

void Chip8::OP_FX0A() {
    uint8_t X = (opcode & 0x0F00) >> 8;

    for (uint8_t key = 0; key < 16; key++) {
        if (keypad[key]) {
            V[X] = key;  // Store the key code
            return;      // Exit once a key is pressed
        }
    }

    PC -= 2;  // Roll back the PC so the same opcode is run again
}

void Chip8::OP_FX15(){
    uint8_t X = (opcode & 0x0F00) >> 8;

    delay_timer = V[X];
}

void Chip8::OP_FX18(){
    uint8_t X = (opcode & 0x0F00) >> 8;

    sound_timer = V[X];
}

void Chip8::OP_FX1E(){
    uint8_t X = (opcode & 0x0F00) >> 8;

    index_register += V[X];
}

void Chip8::OP_FX29(){
    uint8_t X = (opcode & 0x0F00) >> 8;
    //each font character is 5 bytes each
    index_register = 0x50 + (V[X]*5);
}

void Chip8::OP_FX33() {
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t value = V[X];  // Make a copy so we don't modify V[X]

    // Store the hundreds digit
    memory[index_register] = value / 100;

    // Store the tens digit
    memory[index_register + 1] = (value / 10) % 10;

    // Store the ones digit
    memory[index_register + 2] = value % 10;
}

void Chip8::OP_FX55(){
    uint8_t X = (opcode & 0x0F00) >> 8;
    for(uint8_t i = 0; i <= X; i++){
        memory[index_register+i] = V[i];
    }
}

void Chip8::OP_FX65(){
    uint8_t X = (opcode & 0x0F00) >> 8;
    for(uint8_t i = 0; i <= X; i++){
        V[i] = memory[index_register+i];
    }
}

//implement switch stamenets for each particular opcode case
void Chip8::decodeAndExecute(){
    switch (opcode & 0xF000){
        case 0x0000:
            switch(opcode & 0x00FF){
                case 0x00E0: OP_00E0(); break;
                case 0x00EE: OP_00EE(); break;
            }
            break;
        case 0x1000: OP_1NNN(); break;
        case 0x2000: OP_2NNN(); break;
        case 0x3000: OP_3XNN(); break;
        case 0x4000: OP_4XNN(); break;
        case 0x5000: OP_5XY0(); break;
        case 0x6000: OP_6XNN(); break;
        case 0x7000: OP_7XNN(); break;
        case 0x8000:
            switch(opcode & 0x000F){
                case 0x0000: OP_8XY0(); break;
                case 0x0001: OP_8XY1(); break;
                case 0x0002: OP_8XY2(); break;
                case 0x0003: OP_8XY3(); break;
                case 0x0004: OP_8XY4(); break;
                case 0x0005: OP_8XY5(); break;
                case 0x0006: OP_8XY6(); break;
                case 0x0007: OP_8XY7(); break;
                case 0x000E: OP_8XYE(); break;
            }
            break;
        case 0x9000: OP_9XY0(); break;
        case 0xA000: OP_ANNN(); break;
        case 0xB000: OP_BNNN(); break;
        case 0xC000: OP_CXNN(); break;
        case 0xD000: OP_DXYN(); break;
        case 0xE000: 
            switch(opcode & 0x00FF){
                case 0x009E: OP_EX9E(); break;
                case 0x00A1: OP_EXA1(); break;
            }
            break;
        case 0xF000:
            switch(opcode & 0x00FF){
                case 0x0007: OP_FX07(); break;
                case 0x000A: OP_FX0A(); break;
                case 0x0015: OP_FX15(); break;
                case 0x0018: OP_FX18(); break;
                case 0x001E: OP_FX1E(); break;
                case 0x0029: OP_FX29(); break;
                case 0x0033: OP_FX33(); break;
                case 0x0055: OP_FX55(); break;
                case 0x0065: OP_FX65(); break;
            }
            break;
        default:
            cerr <<"Unknow OPCODE" << hex<< opcode<<endl;
            break;
    }
}

    void Chip8::updatetimers(){
        if (delay_timer > 0) {
            --delay_timer;
        }
        if (sound_timer > 0) {
            --sound_timer;
        }

    }

void Chip8::setKeyState(uint8_t key, bool isPressed) {
    keypad[key] = isPressed;
}
