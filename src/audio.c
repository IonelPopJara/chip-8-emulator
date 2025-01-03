#define AMPLITUDE 28000
#define FREQUENCY 440.0

#include <math.h>
#include "audio.h"

void audio_callback(void* userdata, Uint8* stream, int len) {
    (void)userdata;
    static double phase = 0;
    int16_t* stream16 = (int16_t*)stream;
    int samples = len / 2; // 16-bit samples

    for (int i = 0; i < samples; i++) {
        stream16[i] = (int16_t)(AMPLITUDE * sin(phase));
        phase += 2.0 *M_PI * FREQUENCY / 44100.0;
        if (phase > 2.0 * M_PI) {
            phase -= 2.0 * M_PI;
        }
    }
}

int initialize_audio(Audio* audio) {
    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = 44100;
    want.format = AUDIO_S16;
    want.channels = 1;
    want.samples = 2048;
    want.callback = audio_callback;

    audio->audioDevice = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (audio->audioDevice == 0) {
        printf("Failed to open audio device");
        return -1;
    }

    audio->beeping = 0;
    return 0;
}

void toggle_beep(Audio* audio, int on) {
    if (on && !audio->beeping) {
        SDL_PauseAudioDevice(audio->audioDevice, 0);
        audio->beeping = 1;
    } else if (!on && audio->beeping) {
        SDL_PauseAudioDevice(audio->audioDevice, 1);
        audio->beeping = 0;
    }
}

void cleanup_audio(Audio* audio) {
    SDL_CloseAudioDevice(audio->audioDevice);
}
