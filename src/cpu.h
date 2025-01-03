#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <string.h>
#include <SDL2/SDL.h>

#define MEM_SIZE 4096
#define STACK_DEPTH 16
#define NUM_REGS 16
#define FRAMEBUFFER_SIZE 64*32
#define START_FONT_MEM 0x50
#define END_FONT_MEM 0xF9
#define START_PROGRAM_MEM 0x200
#define NUM_KEYS 16

typedef struct {
    uint8_t memory[MEM_SIZE];               // Create 4Kb or 4096 bytes of RAM
    uint16_t PC;                            // Program counter
    uint16_t I;                             // Index register
    int8_t SP;                              // Stack pointer
    uint16_t stack[STACK_DEPTH];            // Stack for 16 bit addresses with 16-levels of depth
    uint8_t v[NUM_REGS];                    // 16 one byte general purpose registers (V0 - VF)
    uint8_t delay_timer;                    // 60 Hz
    uint8_t sound_timer;                    // Something something sound
    uint8_t framebuffer[FRAMEBUFFER_SIZE];  // 64 * 32 pixels display
    uint8_t keypad[NUM_KEYS];               // Array to represent the 16 keys available
    uint8_t original_mode;                  // Flag to determine whether we use a CHIP-8 or SUPER-CHIP
} CPU;

static const uint8_t keymap[NUM_KEYS] = {
    SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
    SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
    SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V
};

int initialize_cpu(CPU* cpu);
void handle_input(CPU* cpu, SDL_KeyboardEvent event);

/*
 * Returns a 16-bit opcode combining two consecutive bytes from memory.
 * Since CHIP-8 has a simple set of instructions,
 * we don't need a separte decode function.
 */
uint16_t fetch(CPU* cpu);


/*
 * Decodes and executes the given opcode.
 * Extracts:
 * X (second nibble),
 * Y (third nibble),
 * N (fourth nibble),
 * NN (third and fourth nibbles),
 * and NNN (second, third, and fourth nibbles)
 */
void execute(CPU* cpu, uint16_t opcode);

int load_rom(CPU* cpu, const char* filename);

#endif
