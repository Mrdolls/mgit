#include "push.h"
#include "../utils/git_utils.h"
#include "../utils/term_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cmd_push(int argc, char **argv) {
    if (!git_is_repository()) {
        printf("%sError: Not a git repository.%s\n", ANSI_RED, ANSI_RESET);
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

    printf("%s[mgit] Preparing push on branch: %s%s%s\n", ANSI_CYAN, ANSI_BOLD, branch, ANSI_RESET);

    if (git_add_all() != 0) {
        printf("%sError: 'git add .' failed.%s\n", ANSI_RED, ANSI_RESET);
        return 1;
    }

    /* Commit may return non-zero if nothing to commit, which is okay */
    git_commit(message);

    if (git_push(branch) != 0) {
        printf("%sError: 'git push' failed.%s\n", ANSI_RED, ANSI_RESET);
        return 1;
    }

    printf("%s[mgit] Successfully pushed changes to remote!%s\n", ANSI_GREEN, ANSI_RESET);
    return 0;
}
