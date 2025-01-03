#ifndef ERROR_H
#define ERROR_H

typedef enum {
    ERROR_NONE,
    ERROR_MISSING_ARGS,
    ERROR_CPU_INIT,
    ERROR_DISPLAY_INIT,
    ERROR_ROM_LOAD,
    ERROR_ROM_OPEN,
    ERROR_ROM_SIZE,
    ERROR_ROM_READ,
    ERROR_SDL_INIT,
    ERROR_MEMORY
} ErrorCode;

void print_error(ErrorCode code, const char* message);
const char* get_error_string(ErrorCode code);

#endif
