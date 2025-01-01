#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <string.h>

#define MEM_SIZE 4096
#define STACK_DEPTH 16
#define NUM_REGS 16
#define FRAMEBUFFER_SIZE 64*32
#define START_FONT_MEM 0x50
#define END_FONT_MEM 0xF9
#define START_PROGRAM_MEM 0x200

typedef struct {
    uint8_t memory[MEM_SIZE];               // Create 4Kb or 4096 bytes of RAM
    uint16_t PC;                            // Program counter
    uint16_t I;                             // Index register
    uint8_t SP;                             // Stack pointer
    uint16_t stack[STACK_DEPTH];            // Stack for 16 bit addresses with 16-levels of depth
    uint8_t v[NUM_REGS];                    // 16 one byte general purpose registers (V0 - VF)
    uint8_t delay_timer;                    // 60 Hz
    uint8_t sound_timer;                    // Something something sound
    uint8_t framebuffer[FRAMEBUFFER_SIZE];  // 64 * 32 pixels display
} CPU;

int initialize_cpu(CPU* cpu);

#endif
