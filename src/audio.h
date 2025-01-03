#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>

typedef struct {
    SDL_AudioDeviceID audioDevice;
    int beeping;
} Audio;

int initialize_audio(Audio* audio);
void cleanup_audio(Audio* audio);
void toggle_beep(Audio* audio, int on);

#endif
