#include "clone.h"
#include "../utils/term_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cmd_clone(int argc, char **argv) {
    if (argc < 1) {
        printf("\n%s %sError: Missing repository URL.%s\n", MGIT_ERROR_BADGE, ANSI_BRIGHT_RED, ANSI_RESET);
        printf("Usage: %smgit clone <repository_url> [destination_folder]%s\n\n", ANSI_BRIGHT_CYAN, ANSI_RESET);
        return 1;
    }

    const char *url = argv[0];
    const char *dest = (argc >= 2) ? argv[1] : "";

    printf("\n%s %sCloning Repository%s\n", MGIT_BADGE, ANSI_BOLD, ANSI_RESET);
    printf("%s  Repository  :%s %s%s%s\n", ANSI_BRIGHT_BLACK, ANSI_RESET, ANSI_BOLD, ANSI_BRIGHT_CYAN, url);
    if (strlen(dest) > 0) {
        printf("%s  Destination :%s %s%s%s\n", ANSI_BRIGHT_BLACK, ANSI_RESET, ANSI_BOLD, ANSI_BRIGHT_YELLOW, dest);
    }
    printf("%s──────────────────────────────────────────────────────────%s\n", ANSI_BRIGHT_BLACK, ANSI_RESET);

    char cmd[1024];
    if (strlen(dest) > 0) {
        printf("%s %sCloning remote repository into '%s'%s...\n",
               MGIT_STEP_PREFIX, ANSI_BOLD, dest, ANSI_RESET);
        snprintf(cmd, sizeof(cmd), "git clone \"%s\" \"%s\"", url, dest);
    } else {
        printf("%s %sCloning remote repository%s...\n",
               MGIT_STEP_PREFIX, ANSI_BOLD, ANSI_RESET);
        snprintf(cmd, sizeof(cmd), "git clone \"%s\"", url);
    }

    int ret = system(cmd);

    printf("%s──────────────────────────────────────────────────────────%s\n", ANSI_BRIGHT_BLACK, ANSI_RESET);

    if (ret == 0) {
        printf("%s %sRepository successfully cloned!%s\n\n", MGIT_SUCCESS_BADGE, ANSI_BRIGHT_GREEN, ANSI_RESET);
    } else {
        printf("%s %sFailed to clone repository.%s\n\n", MGIT_ERROR_BADGE, ANSI_BRIGHT_RED, ANSI_RESET);
    }

    return ret;
}
