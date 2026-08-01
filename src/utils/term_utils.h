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

/* ANSI Colors */
#define ANSI_RESET   "\033[0m"
#define ANSI_BOLD    "\033[1m"
#define ANSI_DIM     "\033[2m"
#define ANSI_RED     "\033[31m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_BLUE    "\033[34m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_CYAN    "\033[36m"
#define ANSI_WHITE   "\033[37m"
#define ANSI_BG_BLUE "\033[44m"
#define ANSI_BG_CYAN "\033[46m"

/* Functions */
void term_init(void);
void term_enable_raw_mode(void);
void term_disable_raw_mode(void);
void term_clear_screen(void);
KeyCode term_read_key(void);
void term_get_size(int *rows, int *cols);

#endif /* TERM_UTILS_H */
