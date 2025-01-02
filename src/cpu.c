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
    cpu->SP = 0;
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
