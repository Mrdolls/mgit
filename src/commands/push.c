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

    int has_changes = git_has_changes();
    int has_unpushed = git_has_unpushed_commits();

    if (!has_changes && !has_unpushed) {
        printf("\n%s %sNothing to commit, working tree clean.%s\n\n", MGIT_INFO_BADGE, ANSI_BRIGHT_YELLOW, ANSI_RESET);
        return 0;
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
    if (has_changes) {
        printf("%s  Message :%s %s\"%s\"%s\n", ANSI_BRIGHT_BLACK, ANSI_RESET, ANSI_ITALIC, message, ANSI_RESET);
        git_show_changes_summary();
    }

    if (has_unpushed) {
        git_show_unpushed_summary(branch);
    }

    printf("%s──────────────────────────────────────────────────────────%s\n", ANSI_BRIGHT_BLACK, ANSI_RESET);
    printf("%sProceed with push? (Y/n): %s", ANSI_BRIGHT_YELLOW, ANSI_RESET);
    fflush(stdout);

    char input[32] = "";
    if (fgets(input, sizeof(input), stdin)) {
        input[strcspn(input, "\r\n")] = '\0';
        if (strcmp(input, "n") == 0 || strcmp(input, "N") == 0 ||
            strcmp(input, "no") == 0 || strcmp(input, "NO") == 0) {
            printf("\n%s %sPush cancelled by user.%s\n\n", MGIT_INFO_BADGE, ANSI_BRIGHT_YELLOW, ANSI_RESET);
            return 0;
        }
    }

    printf("\n");

    if (has_changes) {
        if (git_add_all() != 0) {
            printf("\n%s %sFailed to stage files.%s\n", MGIT_ERROR_BADGE, ANSI_BRIGHT_RED, ANSI_RESET);
            return 1;
        }

        if (git_commit(message) != 0) {
            printf("\n%s %sFailed to create commit.%s\n", MGIT_ERROR_BADGE, ANSI_BRIGHT_RED, ANSI_RESET);
            return 1;
        }
    } else {
        printf("%s %sNo new changes to commit, pushing unpushed commits...%s\n",
               MGIT_STEP_PREFIX, ANSI_BRIGHT_YELLOW, ANSI_RESET);
    }

    if (git_push(branch) != 0) {
        printf("\n%s %sFailed to push changes to remote.%s\n", MGIT_ERROR_BADGE, ANSI_BRIGHT_RED, ANSI_RESET);
        return 1;
    }

    printf("%s──────────────────────────────────────────────────────────%s\n", ANSI_BRIGHT_BLACK, ANSI_RESET);
    printf("%s %sSuccessfully pushed changes to remote!%s\n\n", MGIT_SUCCESS_BADGE, ANSI_BRIGHT_GREEN, ANSI_RESET);
    return 0;
}
