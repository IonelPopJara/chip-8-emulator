#include "cpu.h"
#include "display.h"

const uint32_t REFRESH_RATE = 1000 / 60; // 60 Hz
uint32_t CLOCK_SPEED = 1000 / 700; // 700 instructions per second

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

    if (load_rom(&cpu, "./src/ibm_logo.ch8") < 0) {
        printf("Error!\n");
        return 1;
    }

    SDL_Event event;
    int quit = 0;
    uint32_t last_updated_time = SDL_GetTicks();

    uint32_t last_cpu_cycle = SDL_GetTicks();

    // Main loop
    while (!quit) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = 1;
                    break;
                case SDL_KEYDOWN:
                    handle_input(&cpu, event.key);
                    break;
                case SDL_KEYUP:
                    handle_input(&cpu, event.key);
                    break;
            }
        }

        uint32_t current_time = SDL_GetTicks();
        if (current_time - last_updated_time >= REFRESH_RATE) {
            last_updated_time = SDL_GetTicks();

            // Update timers
            if (cpu.delay_timer > 0) {
                cpu.delay_timer--;
            }

            if (cpu.sound_timer > 0) {
                cpu.sound_timer--;
                // TODO: Add sound
            }

            update_display(&display, &cpu);
        }

        if (current_time - last_cpu_cycle >= CLOCK_SPEED) {
            last_cpu_cycle = SDL_GetTicks();

            // Placeholder functions
            uint16_t opcode = fetch(&cpu);
            execute(&cpu, opcode);
        }
    }

    // Cleanup
    cleanup_display(&display);

    return 0;
}
