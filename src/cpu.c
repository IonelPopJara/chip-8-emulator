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
