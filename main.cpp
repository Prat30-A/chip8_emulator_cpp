#include <SDL2/SDL.h>
#include <iostream>
#include <cstring>
#include <chrono>
#include "chip8.hpp"

const int VIDEO_SCALE = 10;
const int WINDOW_WIDTH = DISPLAY_WIDTH * VIDEO_SCALE;
const int WINDOW_HEIGHT = DISPLAY_HEIGHT * VIDEO_SCALE;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: chip8 <ROM file>" << std::endl;
        return 1;
    }

    Chip8 chip8;
    if (!chip8.load_ROM(argv[1])) {
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL failed to initialize: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("CHIP-8 Emulator",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                             SDL_TEXTUREACCESS_STREAMING, DISPLAY_WIDTH, DISPLAY_HEIGHT);

    bool quit = false;
    SDL_Event event;

    const int FPS = 60;
    const int CYCLES_PER_FRAME = 10;
    uint32_t frameDelay = 1000 / FPS;

    auto lastTimerUpdate = std::chrono::high_resolution_clock::now();

    while (!quit) {
        uint32_t frameStart = SDL_GetTicks();

        // Process Input
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }

            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                bool isPressed = event.type == SDL_KEYDOWN;
                switch (event.key.keysym.sym) {
                    case SDLK_x: chip8.setKeyState(0x0, isPressed); break;
                    case SDLK_1: chip8.setKeyState(0x1, isPressed); break;
                    case SDLK_2: chip8.setKeyState(0x2, isPressed); break;
                    case SDLK_3: chip8.setKeyState(0x3, isPressed); break;
                    case SDLK_q: chip8.setKeyState(0x4, isPressed); break;
                    case SDLK_w: chip8.setKeyState(0x5, isPressed); break;
                    case SDLK_e: chip8.setKeyState(0x6, isPressed); break;
                    case SDLK_a: chip8.setKeyState(0x7, isPressed); break;
                    case SDLK_s: chip8.setKeyState(0x8, isPressed); break;
                    case SDLK_d: chip8.setKeyState(0x9, isPressed); break;
                    case SDLK_z: chip8.setKeyState(0xA, isPressed); break;
                    case SDLK_c: chip8.setKeyState(0xB, isPressed); break;
                    case SDLK_4: chip8.setKeyState(0xC, isPressed); break;
                    case SDLK_r: chip8.setKeyState(0xD, isPressed); break;
                    case SDLK_f: chip8.setKeyState(0xE, isPressed); break;
                    case SDLK_v: chip8.setKeyState(0xF, isPressed); break;
                }
            }
        }

        // Emulate multiple cycles per frame (important for game logic)
        for (int i = 0; i < CYCLES_PER_FRAME; ++i) {
            chip8.fetchOpcode();
            chip8.decodeAndExecute();
        }

        // Update timers at 60Hz
        auto now = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTimerUpdate).count() >= 16) {
            chip8.updatetimers();
            lastTimerUpdate = now;
        }

        // Render
        void* pixels;
        int pitch;
        SDL_LockTexture(texture, nullptr, &pixels, &pitch);
        uint32_t* pixelPtr = static_cast<uint32_t*>(pixels);
        for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; ++i) {
            pixelPtr[i] = (chip8.display[i] ? 0xFFFFFFFF : 0x00000000);
        }

        SDL_UnlockTexture(texture);

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        // Frame limiting
        uint32_t frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    std::cout << "PC: " << std::hex << chip8.PC << " Opcode: " << chip8.opcode << std::endl;

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
