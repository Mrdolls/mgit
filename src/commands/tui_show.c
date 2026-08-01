#include "tui_show.h"
#include "../utils/git_utils.h"
#include "../utils/term_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void draw_tui(CommitInfo *commits, int count, int selected, const char *branch, int rows, int cols) {
    (void)cols;
    term_clear_screen();

    printf("%s%s--- MGIT TUI History Inspector ---%s\n", ANSI_BOLD, ANSI_CYAN, ANSI_RESET);
    printf("Current Branch: %s%s%s | Total Commits: %d\n", ANSI_YELLOW, branch, ANSI_RESET, count);
    printf("================================================================================\n");

    int max_display = rows - 6;
    if (max_display < 1) max_display = 1;

    int start_index = 0;
    if (selected >= max_display) {
        start_index = selected - max_display + 1;
    }

    int end_index = start_index + max_display;
    if (end_index > count) end_index = count;

    for (int i = start_index; i < end_index; i++) {
        if (i == selected) {
            printf("%s %s %-7s | %-12s | %-12s | %-.40s %s\n",
                   ANSI_BG_CYAN, ANSI_BOLD,
                   commits[i].hash, commits[i].author, commits[i].date, commits[i].subject,
                   ANSI_RESET);
        } else {
            printf("   %s%-7s%s | %-12s | %s%-12s%s | %-.40s\n",
                   ANSI_YELLOW, commits[i].hash, ANSI_RESET,
                   commits[i].author,
                   ANSI_DIM, commits[i].date, ANSI_RESET,
                   commits[i].subject);
        }
    }

    printf("================================================================================\n");
    printf("%s[UP/DOWN]%s Navigate | %s[S]%s Switch (checkout) | %s[R]%s Restauration (reset --hard) | %s[Q]%s Quit\n",
           ANSI_BOLD, ANSI_RESET,
           ANSI_BOLD, ANSI_RESET,
           ANSI_BOLD, ANSI_RESET,
           ANSI_BOLD, ANSI_RESET);
    fflush(stdout);
}

int cmd_show(int argc, char **argv) {
    (void)argc;
    (void)argv;

    if (!git_is_repository()) {
        printf("%sError: Not a git repository.%s\n", ANSI_RED, ANSI_RESET);
        return 1;
    }

    char branch[128] = "HEAD";
    git_get_current_branch(branch, sizeof(branch));

    CommitInfo *commits = NULL;
    int count = 0;

    if (!git_get_history(&commits, &count) || count == 0) {
        printf("%sError: Could not retrieve git history or history is empty.%s\n", ANSI_RED, ANSI_RESET);
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
                printf("%s[mgit] Switch (git checkout)%s\n", ANSI_BOLD, ANSI_RESET);
                printf("Target Commit: %s (%s)\n", commits[selected].hash, commits[selected].subject);
                printf("Are you sure you want to checkout this commit? (y/N): ");
                fflush(stdout);

                char response[16] = "";
                if (fgets(response, sizeof(response), stdin) && (response[0] == 'y' || response[0] == 'Y')) {
                    git_checkout(commits[selected].hash);
                } else {
                    printf("Operation cancelled.\n");
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
                printf("%s%s[WARNING] RESTAURATION (git reset --hard)%s\n", ANSI_BOLD, ANSI_RED, ANSI_RESET);
                printf("%sThis operation will PERMANENTLY ERASE all uncommitted changes!%s\n\n", ANSI_RED, ANSI_RESET);
                printf("Target Commit: %s (%s)\n", commits[selected].hash, commits[selected].subject);
                printf("Are you absolutely sure you want to proceed? (type 'yes' to confirm): ");
                fflush(stdout);

                char response[16] = "";
                if (fgets(response, sizeof(response), stdin)) {
                    response[strcspn(response, "\r\n")] = '\0';
                    if (strcmp(response, "yes") == 0 || strcmp(response, "y") == 0 || strcmp(response, "Y") == 0) {
                        git_reset_hard(commits[selected].hash);
                    } else {
                        printf("Restauration cancelled.\n");
                    }
                } else {
                    printf("Restauration cancelled.\n");
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
