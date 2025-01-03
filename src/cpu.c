#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>
#include "cpu.h"
#include "font.h"
#include "error.h"

int initialize_cpu(CPU* cpu) {
    // Set a seed so we get random numbers each time
    srand(time(NULL));

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
    cpu->original_mode = 0;

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
            // There is only one instruction whose first nibble is 6
            // set vx to nn
            cpu->v[x] = nn;
            break;
        case 0x7:
            // There is only one instruction whose first nibble is 7
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
                    if (cpu->original_mode == 1) {
                        cpu->v[x] = cpu->v[y];

                    }
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
                    if (cpu->original_mode == 1) {
                        cpu->v[x] = cpu->v[y];
                    }
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
            // There is only one instruction whose first nibble is A
            // Set the index register to the value nnn
            cpu->I = nnn;
            break;
        case 0xB:
            // There is only one instruction whose first nibble is B
            // But it's ambiguous :/
            if (cpu->original_mode == 1) {
                cpu->PC = cpu->v[0] + nnn;

            } else {
                cpu->PC = cpu->v[x] + nnn;

            }
            break;
        case 0xC: {
            // There is only one instruction whose first nibble is C
            // generate a random number and binary ANDs it with nn
            uint8_t random = rand() % 256;
            random = random & nn;
            // put the result in vx
            cpu->v[x] = random;
            break;
        }
        case 0xD: {
            // There is only one instruction whose first nibble is D,
            // and it's the most convoluted to write
            uint8_t x_start = cpu->v[x] % 64;
            uint8_t y_start = cpu->v[y] % 32;
            cpu->v[0xF] = 0;

            // Iterate over the rows
            for (size_t row = 0; row < n; row++) {
                if (y_start + row >= 32) {
                    break;
                }

                // Get the nth byte of sprite data
                uint8_t nth = cpu->memory[cpu->I + row];

                // Iterate over the columns
                for (size_t col = 0; col < 8; col++) {
                    if (x_start + col >= 64) {
                        break;
                    }

                    uint8_t nth_pixel = (nth & (0x80 >> col)) ? 1 : 0;
                    uint16_t pixel_index = x_start + col + ((y_start + row) * 64);

                    // Check collision
                    uint8_t current_pixel = cpu->framebuffer[pixel_index];
                    if (nth_pixel & current_pixel) {
                        cpu->v[0xF] = 1;
                    }

                    cpu->framebuffer[pixel_index] ^= nth_pixel;
                }
            }
            break;
        }
        case 0xE:
            switch (nn) {
                case 0x9E:
                    // Skip if the key in register x is pressed
                    if (cpu->keypad[cpu->v[x]] == 1) {
                        cpu->PC += 2;
                    }
                    break;
                case 0xA1:
                    // Skip if the key in register x is not pressed
                    if (cpu->keypad[cpu->v[x]] == 0) {
                        cpu->PC += 2;
                    }
                    break;
            }
            break;
        case 0xF:
            switch (nn) {
                case 0x07:
                    cpu->v[x] = cpu->delay_timer;
                    break;
                case 0x0A: {
                    // wait for any key press
                    int key_pressed = -1;
                    for (uint8_t i = 0; i < NUM_KEYS; i++) {
                        if (cpu->keypad[i] == 1) {
                            cpu->v[x] = i;
                            key_pressed = 1;
                            break;
                        }
                    }

                    if (key_pressed == -1) {
                        cpu->PC -= 2;
                    }
                    break;
                }
                case 0x15:
                    cpu->delay_timer = cpu->v[x];
                    break;
                case 0x18:
                    cpu->sound_timer = cpu->v[x];
                    break;
                case 0x1E: {
                    uint16_t tmp = cpu->I + cpu->v[x];
                    if (tmp > 0xFFF) {
                        cpu->v[0xF] = 1;

                    }
                    cpu->I += cpu->v[x];
                    break;
                }
                case 0x29: {
                    uint8_t hex_char = (cpu->v[x] & 0x0F); // Hex character stored in vx
                    cpu->I = START_FONT_MEM + (hex_char * 5);
                    break;
                }
                case 0x33: {
                    // Binary coded decimal conversion
                    uint8_t num = cpu->v[x];

                    for (int8_t i = 2; i >= 0; i--) {
                        cpu->memory[cpu->I + i] = num % 10;
                        num /= 10;
                    }

                    break;
                }
                case 0x55:
                    // Ambiguous instruction but according to the guide it didn't matter much
                    for (uint8_t i = 0; i <= x; i++) {
                        cpu->memory[cpu->I + i] = cpu->v[i];
                    }
                    break;
                case 0x65:
                    // Ambiguous instruction but according to the guide it didn't matter much
                    for (uint8_t i = 0; i <= x; i++) {
                        cpu->v[i] = cpu->memory[cpu->I + i];
                    }
                    break;
            }
            break;
        default:
            // Don't do anything
            break;
    }
}

int load_rom(CPU* cpu, const char* filename) {
    FILE* rom = fopen(filename, "rb");
    if (rom == NULL) {
        print_error(ERROR_ROM_OPEN, "Could not open ROM file");
        return -1;
    }

    // Get the file size
    fseek(rom, 0, SEEK_END);
    size_t rom_size = ftell(rom);
    rewind(rom);

    // Make sure the rom fits in the memory
    if (rom_size > MEM_SIZE - START_PROGRAM_MEM) {
        print_error(ERROR_ROM_SIZE, "ROM file too large");
        return -1;
    }

    // Load rom into memory starting at START_PROGRAM_MEM (0x200)
    size_t bytes_read = fread(&cpu->memory[START_PROGRAM_MEM], 1, rom_size, rom);
    if (bytes_read != rom_size) {
        print_error(ERROR_ROM_SIZE, "Could not read ROM file");
        fclose(rom);
        return -1;
    }

    fclose(rom);
    return 0;
}
