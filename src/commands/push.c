#include "push.h"
#include "../utils/git_utils.h"
#include "../utils/term_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cmd_push(int argc, char **argv) {
    if (!git_is_repository()) {
        printf("%s %sError: Not a git repository.%s\n", MGIT_ERROR_BADGE, ANSI_BRIGHT_RED, ANSI_RESET);
        return 1;
    }

    char message[1024] = "Auto push";
    if (argc > 0) {
        message[0] = '\0';
        for (int i = 0; i < argc; i++) {
            if (i > 0) strncat(message, " ", sizeof(message) - strlen(message) - 1);
            strncat(message, argv[i], sizeof(message) - strlen(message) - 1);
        }
    }

    char branch[128] = "";
    git_get_current_branch(branch, sizeof(branch));

    printf("\n%s %sAutomated Push Workflow%s\n", MGIT_BADGE, ANSI_BOLD, ANSI_RESET);
    printf("%s  Branch  :%s %s%s%s\n", ANSI_BRIGHT_BLACK, ANSI_RESET, ANSI_BOLD, ANSI_BRIGHT_CYAN, branch);
    printf("%s  Message :%s %s\"%s\"%s\n", ANSI_BRIGHT_BLACK, ANSI_RESET, ANSI_ITALIC, message, ANSI_RESET);
    printf("%s──────────────────────────────────────────────────────────%s\n", ANSI_BRIGHT_BLACK, ANSI_RESET);

    if (git_add_all() != 0) {
        printf("\n%s %sFailed to stage files.%s\n", MGIT_ERROR_BADGE, ANSI_BRIGHT_RED, ANSI_RESET);
        return 1;
    }

    git_commit(message);

    if (git_push(branch) != 0) {
        printf("\n%s %sFailed to push changes to remote.%s\n", MGIT_ERROR_BADGE, ANSI_BRIGHT_RED, ANSI_RESET);
        return 1;
    }

    printf("%s──────────────────────────────────────────────────────────%s\n", ANSI_BRIGHT_BLACK, ANSI_RESET);
    printf("%s %sSuccessfully pushed all changes to remote!%s\n\n", MGIT_SUCCESS_BADGE, ANSI_BRIGHT_GREEN, ANSI_RESET);
    return 0;
}
