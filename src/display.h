#ifndef DISP_H
#define DISP_H

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCREEN_SCALE 10

#include <stdint.h>
#include <SDL2/SDL.h>

// NOTE: font.h kept giving me probles, this is the solution I found
// Forward declare CPU struct instead of including cpu.h
struct CPU;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
} Display;

int initialize_display(Display *display);
void update_display(Display *display, const struct CPU *cpu);
void cleanup_display(Display *display);

#endif
