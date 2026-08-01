#include "tui_show.h"
#include "../utils/git_utils.h"
#include "../utils/term_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void draw_tui(CommitInfo *commits, int count, int selected, const char *branch, int rows, int cols) {
    (void)cols;
    term_clear_screen();

    printf("%s%s┌─────────────────────────────────────────────────────────────────────────────┐%s\n", ANSI_BOLD, ANSI_CYAN, ANSI_RESET);
    printf("%s%s│                      MGIT TUI HISTORY INSPECTOR                             │%s\n", ANSI_BOLD, ANSI_CYAN, ANSI_RESET);
    printf("%s%s└─────────────────────────────────────────────────────────────────────────────┘%s\n", ANSI_BOLD, ANSI_CYAN, ANSI_RESET);
    printf(" %sBranch:%s %s%s%s | %sCommits:%s %s%d%s\n",
           ANSI_BRIGHT_BLACK, ANSI_RESET, ANSI_BOLD, branch, ANSI_RESET,
           ANSI_BRIGHT_BLACK, ANSI_RESET, ANSI_BOLD, count, ANSI_RESET);
    printf("%s───────────────────────────────────────────────────────────────────────────────%s\n", ANSI_BRIGHT_BLACK, ANSI_RESET);

    int max_display = rows - 7;
    if (max_display < 1) max_display = 1;

    int start_index = 0;
    if (selected >= max_display) {
        start_index = selected - max_display + 1;
    }

    int end_index = start_index + max_display;
    if (end_index > count) end_index = count;

    for (int i = start_index; i < end_index; i++) {
        if (i == selected) {
            printf("%s %s %-7s │ %-12.12s │ %-12.12s │ %-.40s %s\n",
                   ANSI_BG_CYAN, ANSI_BOLD,
                   commits[i].hash, commits[i].author, commits[i].date, commits[i].subject,
                   ANSI_RESET);
        } else {
            printf("   %s%-7s%s │ %s%-12.12s%s │ %s%-12.12s%s │ %s%-.40s%s\n",
                   ANSI_BRIGHT_YELLOW, commits[i].hash, ANSI_RESET,
                   ANSI_BRIGHT_WHITE, commits[i].author, ANSI_RESET,
                   ANSI_BRIGHT_BLACK, commits[i].date, ANSI_RESET,
                   ANSI_RESET, commits[i].subject, ANSI_RESET);
        }
    }

    printf("%s───────────────────────────────────────────────────────────────────────────────%s\n", ANSI_BRIGHT_BLACK, ANSI_RESET);
    printf("%s[UP/DOWN]%s Navigate  %s[S]%s Switch (checkout)  %s[R]%s Restauration (reset --hard)  %s[Q]%s Quit\n",
           ANSI_BRIGHT_CYAN, ANSI_RESET,
           ANSI_BRIGHT_GREEN, ANSI_RESET,
           ANSI_BRIGHT_RED, ANSI_RESET,
           ANSI_BRIGHT_YELLOW, ANSI_RESET);
    fflush(stdout);
}

int cmd_show(int argc, char **argv) {
    (void)argc;
    (void)argv;

    if (!git_is_repository()) {
        printf("%s %sError: Not a git repository.%s\n", MGIT_ERROR_BADGE, ANSI_BRIGHT_RED, ANSI_RESET);
        return 1;
    }

    char branch[128] = "HEAD";
    git_get_current_branch(branch, sizeof(branch));

    CommitInfo *commits = NULL;
    int count = 0;

    if (!git_get_history(&commits, &count) || count == 0) {
        printf("%s %sCould not retrieve git history or history is empty.%s\n", MGIT_ERROR_BADGE, ANSI_BRIGHT_RED, ANSI_RESET);
        return 1;
    }

    term_init();
    term_enable_raw_mode();

    int selected = 0;
    int running = 1;

    while (running) {
        int rows = 24, cols = 80;
        term_get_size(&rows, &cols);

        draw_tui(commits, count, selected, branch, rows, cols);

        KeyCode key = term_read_key();

        switch (key) {
            case KEY_UP:
                if (selected > 0) selected--;
                break;

            case KEY_DOWN:
                if (selected < count - 1) selected++;
                break;

            case KEY_QUIT:
            case KEY_ESC:
                running = 0;
                break;

            case KEY_SWITCH: {
                term_disable_raw_mode();
                term_clear_screen();
                printf("\n%s %sSWITCH COMMIT (git checkout)%s\n", MGIT_BADGE, ANSI_BOLD, ANSI_RESET);
                printf("%s Target Commit : %s%s (%s)%s\n", ANSI_BRIGHT_BLACK, ANSI_BOLD, commits[selected].hash, commits[selected].subject, ANSI_RESET);
                printf("\n%sAre you sure you want to checkout this commit? (y/N): %s", ANSI_BRIGHT_YELLOW, ANSI_RESET);
                fflush(stdout);

                char response[16] = "";
                if (fgets(response, sizeof(response), stdin) && (response[0] == 'y' || response[0] == 'Y')) {
                    printf("\n");
                    git_checkout(commits[selected].hash);
                } else {
                    printf("\n%sOperation cancelled.%s\n", ANSI_BRIGHT_BLACK, ANSI_RESET);
                }
                printf("\nPress ENTER to return to TUI...");
                fflush(stdout);
                getchar();

                /* Refresh history */
                git_free_history(commits, count);
                git_get_current_branch(branch, sizeof(branch));
                git_get_history(&commits, &count);
                if (selected >= count && count > 0) selected = count - 1;

                term_enable_raw_mode();
                break;
            }

            case KEY_RESTORE: {
                term_disable_raw_mode();
                term_clear_screen();
                printf("\n%s %sRESTAURATION (git reset --hard)%s\n", MGIT_ERROR_BADGE, ANSI_BOLD, ANSI_RESET);
                printf("%sThis operation will PERMANENTLY ERASE all uncommitted changes!%s\n\n", ANSI_BRIGHT_RED, ANSI_RESET);
                printf("%s Target Commit : %s%s (%s)%s\n", ANSI_BRIGHT_BLACK, ANSI_BOLD, commits[selected].hash, commits[selected].subject, ANSI_RESET);
                printf("\n%sAre you absolutely sure you want to proceed? (type 'yes' to confirm): %s", ANSI_BRIGHT_YELLOW, ANSI_RESET);
                fflush(stdout);

                char response[16] = "";
                if (fgets(response, sizeof(response), stdin)) {
                    response[strcspn(response, "\r\n")] = '\0';
                    if (strcmp(response, "yes") == 0 || strcmp(response, "y") == 0 || strcmp(response, "Y") == 0) {
                        printf("\n");
                        git_reset_hard(commits[selected].hash);
                    } else {
                        printf("\n%sRestauration cancelled.%s\n", ANSI_BRIGHT_BLACK, ANSI_RESET);
                    }
                } else {
                    printf("\n%sRestauration cancelled.%s\n", ANSI_BRIGHT_BLACK, ANSI_RESET);
                }
                printf("\nPress ENTER to return to TUI...");
                fflush(stdout);
                getchar();

                /* Refresh history */
                git_free_history(commits, count);
                git_get_current_branch(branch, sizeof(branch));
                git_get_history(&commits, &count);
                if (selected >= count && count > 0) selected = count - 1;

                term_enable_raw_mode();
                break;
            }

            default:
                break;
        }
    }

    term_disable_raw_mode();
    term_clear_screen();
    git_free_history(commits, count);

    return 0;
}
