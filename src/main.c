#include "cpu.h"
#include "display.h"

const uint32_t REFRESH_RATE = 1000 / 60; // 60 Hz

int main(int argc, char** argv) {

    CPU cpu;
    if (initialize_cpu(&cpu) < 0) {
        printf("Error!\n");
        return 1;
    }

    Display display;
    if (initialize_display(&display) < 0) {
        printf("Error!\n");
        return 1;
    }

    SDL_Event event;
    int quit = 0;
    uint32_t last_updated_time = SDL_GetTicks();

    // Main loop
    while (!quit) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }

        uint32_t current_time = SDL_GetTicks();
        if (current_time - last_updated_time >= REFRESH_RATE) {

            // Update timers
            if (cpu.delay_timer > 0) {
                cpu.delay_timer--;
            }

            if (cpu.sound_timer > 0) {
                cpu.sound_timer--;
                // TODO: Add sound
            }

            last_updated_time = current_time;
            for(int i = 0; i < 5; i++) {
                uint8_t line = cpu.memory[START_FONT_MEM + 5 * 0 + i];
                for (int j = 0; j < 8; j++) {
                    if ((line & (0x80 >> j)) != 0) {
                        cpu.framebuffer[(5 + i) * SCREEN_WIDTH + 2 + j] = 1;
                    }
                }
            }

            for (int i = 0; i < 5; i++) {
                uint8_t line = cpu.memory[START_FONT_MEM + 5 * 1 + i];
                for (int j = 0; j < 8; j++) {
                    if ((line & (0x80 >> j)) != 0) {
                        cpu.framebuffer[(5 + i) * SCREEN_WIDTH + 7 + j] = 1;
                    }
                }
            }

            update_display(&display, &cpu);
        }
    }

    // Cleanup
    cleanup_display(&display);

    return 0;
}
