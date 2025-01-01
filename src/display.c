#include "display.h"
#include "cpu.h"

int initialize_display(Display *display) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    display->window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * SCREEN_SCALE, SCREEN_HEIGHT * SCREEN_SCALE, SDL_WINDOW_SHOWN);

    if (display->window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    display->renderer = SDL_CreateRenderer(display->window, -1, SDL_RENDERER_ACCELERATED);
    if (display->renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    display->texture = SDL_CreateTexture(display->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

    if (display->texture == NULL) {
        printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    return 0;
}

void update_display(Display *display, const struct CPU *cpu) {
    uint32_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

    // Convert our 1-bit framebuffer to 32-bit pixels
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        pixels[i] = cpu->framebuffer[i] ? 0xFFFFFFFF : 0x000000FF;
    }

    // Update texture with pixel data
    SDL_UpdateTexture(display->texture, NULL, pixels, SCREEN_WIDTH * sizeof(uint32_t));

    // Clear renderer
    SDL_RenderClear(display->renderer);

    // Copy texture to renderer
    SDL_RenderCopy(display->renderer, display->texture, NULL, NULL);

    // Update screen
    SDL_RenderPresent(display->renderer);
}

// NOTE: Do not confuse this function with clean_display
void cleanup_display(Display* display) {
    SDL_DestroyTexture(display->texture);
    SDL_DestroyRenderer(display->renderer);
    SDL_DestroyWindow(display->window);
    SDL_Quit();
}
