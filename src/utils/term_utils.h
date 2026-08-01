#ifndef TERM_UTILS_H
#define TERM_UTILS_H

#include <stddef.h>

/* Key codes */
typedef enum {
    KEY_UNKNOWN = 0,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_ENTER,
    KEY_ESC,
    KEY_QUIT,
    KEY_SWITCH,
    KEY_RESTORE,
    KEY_OTHER
} KeyCode;

/* Rich ANSI Color Palette */
#define ANSI_RESET          "\033[0m"
#define ANSI_BOLD           "\033[1m"
#define ANSI_DIM            "\033[2m"
#define ANSI_ITALIC         "\033[3m"
#define ANSI_UNDERLINE      "\033[4m"

#define ANSI_BLACK          "\033[30m"
#define ANSI_RED            "\033[31m"
#define ANSI_GREEN          "\033[32m"
#define ANSI_YELLOW         "\033[33m"
#define ANSI_BLUE           "\033[34m"
#define ANSI_MAGENTA        "\033[35m"
#define ANSI_CYAN           "\033[36m"
#define ANSI_WHITE          "\033[37m"

#define ANSI_BRIGHT_BLACK   "\033[90m"
#define ANSI_BRIGHT_RED     "\033[91m"
#define ANSI_BRIGHT_GREEN   "\033[92m"
#define ANSI_BRIGHT_YELLOW  "\033[93m"
#define ANSI_BRIGHT_BLUE    "\033[94m"
#define ANSI_BRIGHT_MAGENTA "\033[95m"
#define ANSI_BRIGHT_CYAN    "\033[96m"
#define ANSI_BRIGHT_WHITE   "\033[97m"

#define ANSI_BG_BLUE        "\033[44m"
#define ANSI_BG_CYAN        "\033[46m"
#define ANSI_BG_MAGENTA     "\033[45m"
#define ANSI_BG_DARK_GRAY   "\033[100m"

/* Stylized Badges */
#define MGIT_BADGE          "\033[1;96m[mgit]\033[0m"
#define MGIT_STEP_PREFIX    "\033[1;94m  ==>\033[0m"
#define MGIT_SUCCESS_BADGE  "\033[1;92m[SUCCESS]\033[0m"
#define MGIT_ERROR_BADGE    "\033[1;91m[ERROR]\033[0m"
#define MGIT_INFO_BADGE     "\033[1;93m[INFO]\033[0m"

/* Functions */
void term_init(void);
void term_enable_raw_mode(void);
void term_disable_raw_mode(void);
void term_clear_screen(void);
KeyCode term_read_key(void);
void term_get_size(int *rows, int *cols);

#endif /* TERM_UTILS_H */
