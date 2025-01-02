#include <SDL2/SDL.h>
#include "cpu.h"
#include "font.h"

int initialize_cpu(CPU* cpu) {

    // Check for null pointers
    if (cpu == NULL) {
        return -1;
    }

    // Clear memory
    if (memset(cpu->memory, 0, MEM_SIZE) == NULL) {
        return -1;
    }

    // Originally, 512 bytes were used to store its own code
    // Therefore, we need to leave those 512 bytes empty (except for the fonts).
    // According to the guide I was reading: "For some reason, it’s become popular to put it at 050–09F"
    for (int i = START_FONT_MEM; i <= END_FONT_MEM; i++) {
        cpu->memory[i] = fontset[i - START_FONT_MEM];
    }

    cpu->PC = START_PROGRAM_MEM;  // Initialize it to address 0x200 for retro-compatibility
    cpu->SP = -1;
    cpu->I = 0;
    cpu->delay_timer = 0;
    cpu->sound_timer = 0;

    // Clear framebuffer
    if (memset(cpu->framebuffer, 0, FRAMEBUFFER_SIZE) == NULL) {
        return -1;
    }

    // Clear stack
    if (memset(cpu->stack, 0, sizeof(cpu->stack)) == NULL) {
        return -1;
    }

    // Clear registers
    if (memset(cpu->v, 0, sizeof(cpu->v)) == NULL) {
        return -1;
    }

    return 0;
}

void print_keypad(CPU* cpu) {
    printf("\n");
    printf("1: %d | 2: %d | 3: %d | C: %d\n", cpu->keypad[0], cpu->keypad[1], cpu->keypad[2], cpu->keypad[3]);
    printf("4: %d | 5: %d | 6: %d | D: %d\n", cpu->keypad[4], cpu->keypad[5], cpu->keypad[6], cpu->keypad[7]);
    printf("7: %d | 8: %d | 9: %d | E: %d\n", cpu->keypad[8], cpu->keypad[9], cpu->keypad[10], cpu->keypad[11]);
    printf("A: %d | 0: %d | B: %d | F: %d\n", cpu->keypad[12], cpu->keypad[13], cpu->keypad[14], cpu->keypad[15]);
}

void handle_input(CPU* cpu, SDL_KeyboardEvent key) {
    for (int i = 0; i < NUM_KEYS; i++) {
        if (key.keysym.scancode == keymap[i]) {
            cpu->keypad[i] = (key.type == SDL_KEYDOWN) ? 1 : 0;
            break;
        }
    }

    // Only for debugging
    /*print_keypad(cpu);*/
}

uint16_t fetch(CPU* cpu) {
    /* An instruction is two bytes,
    * so we need to read two instructions
    * and combine them
    */

    uint8_t first_half = cpu->memory[cpu->PC];
    uint8_t second_half = cpu->memory[cpu->PC + 1];

    // Increment to get the next two bytes
    cpu->PC += 2;

    // 11011011 | 10001000
    // 11011011 << 8 = 1101101100000000
    // 1101101110001000
    uint16_t opcode = (first_half << 8) | second_half;

    return opcode;
}

void execute(CPU* cpu, uint16_t opcode) {
    // There are like 35 instructions to consider

    uint8_t first = (opcode & 0xF000) >> 12;    // first nibble
    uint8_t x = (opcode & 0x0F00) >> 8;         // second nibble
    uint8_t y = (opcode & 0x00F0) >> 4;         // third nibble
    uint8_t n = (opcode & 0x000F);              // fourth nibble
    uint8_t nn = (opcode & 0x00FF);             // second byte
    uint16_t nnn = (opcode & 0x0FFF);           // second, third, and fourth nibbles

    switch(first) {
        case 0x0:
            switch(nn) {
                case 0xE0:
                    // clear screen
                    memset(cpu->framebuffer, 0, FRAMEBUFFER_SIZE);
                    break;
                case 0xEE:
                    // return
                    cpu->PC = cpu->stack[cpu->SP]; // program pointer points to the return address
                    cpu->SP--; // decrement the stack pointer
                    break;
            }
            break;
        case 0x1:
            // There is only one instruction whose first nibble is 1
            // jump
            cpu->PC = nnn; // set the program counter to nnn mem address
            break;
        case 0x2:
            // There is only one instruction whose first nibble is 2
            // call (subroutine)
            cpu->SP++;
            cpu->stack[cpu->SP] = cpu->PC;
            cpu->PC = nnn;
            break;
        case 0x3:
            // There is only one instruction whose first nibble is 3
            // skip if Vx == nn
            if (cpu->v[x] == nn) {
                // skip one instruction
                cpu->PC += 2;
            }
            break;
        case 0x4:
            // There is only one instruction whose first nibble is 4
            // skip if Vx != nn
            if (cpu->v[x] != nn) {
                // skip one instruction
                cpu->PC += 2;
            }
            break;
        case 0x5:
            // There is only one instruction whose first nibble is 5
            // skip if Vx == Vy
            if (cpu->v[x] == cpu->v[y]) {
                // skip one instruction
                cpu->PC += 2;
            }
            break;
        case 0x6:
            // set vx to nn
            cpu->v[x] = nn;
            break;
        case 0x7:
            // add nn to vx
            cpu->v[x] += nn;
            break;
        case 0x8:
            // This set of instructions is decided based on the last nibble of the opcode
            switch (n) {
                case 0x0:
                    // set vx to vy
                    cpu->v[x] = cpu->v[y];
                    break;
                case 0x1:
                    // binary OR
                    cpu->v[x] = cpu->v[x] | cpu->v[y];
                    break;
                case 0x2:
                    // binary AND
                    cpu->v[x] = cpu->v[x] & cpu->v[y];
                    break;
                case 0x3:
                    // binary XOR
                    cpu->v[x] = cpu->v[x] ^ cpu->v[y];
                    break;
                case 0x4: {
                    // Add
                    int16_t tmp = cpu->v[x] + cpu->v[y];    // Check for overflow
                    cpu->v[0xF] = (tmp > 255) ? 1 : 0;      // Assign the carry flag
                    cpu->v[x] = tmp & 0xFF;                 // Assign the lower 8 bits of tmp
                    break;
                }
                case 0x5:
                    // Subtract vx - vy
                    cpu->v[0xF] = cpu->v[x] > cpu->v[y] ? 1 : 0;    // Check for underflow
                    cpu->v[x] = cpu->v[x] - cpu->v[y];              // Check for overflow
                    break;
                case 0x6:
                    // TODO: Add the ability to configure the behavior
                    // shift one bit to the right
                    cpu->v[0xF] = cpu->v[x] & 0b1;
                    cpu->v[x] = cpu->v[x] >> 1;
                    break;
                case 0x7:
                    // Subtract vy - vx
                    cpu->v[0xF] = cpu->v[y] > cpu->v[x] ? 1 : 0;    // Check for underflow
                    cpu->v[x] = cpu->v[y] - cpu->v[x];              // Check for overflow
                    break;
                case 0xE:
                    // TODO: Add the ability to configure the behavior
                    // shift one bit to the left
                    cpu->v[0xF] = (cpu->v[x] >> 7) & 0b1;
                    cpu->v[x] = cpu->v[x] << 1;
                    break;
            }
            break;
        case 0x9:
            // There is only one instruction whose first nibble is 9
            // skip if Vx != Vy
            if (cpu->v[x] != cpu->v[y]) {
                // skip one instruction
                cpu->PC += 2;
            }
            break;
        case 0xA:
            // do something
            break;
        case 0xB:
            // do something
            break;
        case 0xC:
            // do something
            break;
        case 0xD:
            // do something
            break;
        case 0xE:
            // do something
            break;
        case 0xF:
            // do something
            break;
        default:
            // Don't do anything
            break;
    }
}
