#include <stdio.h>
#include "error.h"

void print_error(ErrorCode code, const char* message) {
    fprintf(stderr, "Error [%d]: %s - %s\n",
            code,
            get_error_string(code),
            message);
}
const char* get_error_string(ErrorCode code) {
    switch (code) {
        case ERROR_ROM_LOAD:
            return  "Rom Loading Error";
            break;
        default:
            return "Uknown Error";
    }
}

