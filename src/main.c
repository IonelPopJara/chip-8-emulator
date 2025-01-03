#include <getopt.h>
#include "cpu.h"
#include "display.h"
#include "error.h"
#include "audio.h"

const uint32_t REFRESH_RATE = 1000 / 60; // 60 Hz
uint32_t CLOCK_SPEED = 1000 / 700; // 700 instructions per second

int main(int argc, char** argv) {
    char* rom_path = NULL;
    uint32_t clock_speed = 700;
    int original_mode = 0;

    int opt;
    while ((opt = getopt(argc, argv, "r:c:o")) != -1) {
        switch(opt) {
            case 'r':
                rom_path = optarg;
                break;
            case 'c':
                clock_speed = atoi(optarg);
                break;
            case 'o':
                original_mode = 1;
                break;
            default:
                printf("Usage: %s -r rom_path [-c clock_speed] [-o]\n", argv[0]);
                return 1;
        }
    }

    if (rom_path == NULL) {
        print_error(ERROR_MISSING_ARGS, "Rom path is required");
        return 1;
    }

    CLOCK_SPEED = 1000 / clock_speed;

    CPU cpu;
    if (initialize_cpu(&cpu) < 0) {
        print_error(ERROR_CPU_INIT, "CPU could not be initialized");
        return 1;
    }

    cpu.original_mode = original_mode;

    Display display;
    if (initialize_display(&display) < 0) {
        print_error(ERROR_DISPLAY_INIT, "Display could not be initialized");
        return 1;
    }

    Audio audio;

    if (initialize_audio(&audio) < 0) {
        return 1;
    }

    if (load_rom(&cpu, rom_path) < 0) {
        print_error(ERROR_ROM_LOAD, "ROM could not be loaded");
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
                toggle_beep(&audio, 1);
            } else {
                toggle_beep(&audio, 0);
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
    cleanup_audio(&audio);

    return 0;
}
