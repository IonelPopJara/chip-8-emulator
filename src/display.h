#ifndef DISP_H
#define DISP_H

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCREEN_SCALE 10

#include <stdint.h>
#include <SDL2/SDL.h>

#include "cpu.h"

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    uint32_t* pixels;
} Display;

int initialize_display(Display *display);
void update_display(Display *display, CPU *cpu);
void cleanup_display(Display *display);

#endif
