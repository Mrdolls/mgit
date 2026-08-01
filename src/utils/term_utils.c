#include "term_utils.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
static DWORD orig_mode_in;
static DWORD orig_mode_out;
static int raw_mode_enabled = 0;
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/select.h>
static struct termios orig_termios;
static int raw_mode_enabled = 0;
#endif

void term_init(void) {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= 0x0004; /* ENABLE_VIRTUAL_TERMINAL_PROCESSING */
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif
}

void term_enable_raw_mode(void) {
    if (raw_mode_enabled) return;

#ifdef _WIN32
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(hIn, &orig_mode_in);
    GetConsoleMode(hOut, &orig_mode_out);

    DWORD raw_in = orig_mode_in & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
    SetConsoleMode(hIn, raw_in);
    raw_mode_enabled = 1;
#else
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) return;
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_iflag &= ~(IXON | ICRNL);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == 0) {
        raw_mode_enabled = 1;
    }
#endif
}

void term_disable_raw_mode(void) {
    if (!raw_mode_enabled) return;

#ifdef _WIN32
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    SetConsoleMode(hIn, orig_mode_in);
    raw_mode_enabled = 0;
#else
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    raw_mode_enabled = 0;
#endif
}

void term_clear_screen(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

KeyCode term_read_key(void) {
#ifdef _WIN32
    int c = _getch();
    if (c == 0 || c == 224) {
        c = _getch();
        switch (c) {
            case 72: return KEY_UP;
            case 80: return KEY_DOWN;
            case 75: return KEY_LEFT;
            case 77: return KEY_RIGHT;
            default: return KEY_OTHER;
        }
    }
    if (c == 13 || c == 10) return KEY_ENTER;
    if (c == 27) return KEY_ESC;
    if (c == 'q' || c == 'Q') return KEY_QUIT;
    if (c == 's' || c == 'S') return KEY_SWITCH;
    if (c == 'r' || c == 'R') return KEY_RESTORE;
    if (c == 'p' || c == 'P') return KEY_PULL;
    return KEY_OTHER;
#else
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1) return KEY_UNKNOWN;

    if (c == 27) { /* Escape sequence or standalone ESC */
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        struct timeval tv = {0, 50000}; /* 50ms timeout */

        int res = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
        if (res <= 0) {
            /* No follow-up sequence bytes -> standalone ESC key! */
            return KEY_ESC;
        }

        char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return KEY_ESC;
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return KEY_ESC;

        if (seq[0] == '[') {
            switch (seq[1]) {
                case 'A': return KEY_UP;
                case 'B': return KEY_DOWN;
                case 'C': return KEY_RIGHT;
                case 'D': return KEY_LEFT;
            }
        }
        return KEY_ESC;
    }

    if (c == '\n' || c == '\r') return KEY_ENTER;
    if (c == 'q' || c == 'Q') return KEY_QUIT;
    if (c == 's' || c == 'S') return KEY_SWITCH;
    if (c == 'r' || c == 'R') return KEY_RESTORE;
    if (c == 'p' || c == 'P') return KEY_PULL;

    return KEY_OTHER;
#endif
}

void term_get_size(int *rows, int *cols) {
    *rows = 24;
    *cols = 80;

#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
        *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
#else
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0) {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
    }
#endif
}
