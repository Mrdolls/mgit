#include "tui_show.h"
#include "../utils/git_utils.h"
#include "../utils/term_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void draw_tui(CommitInfo *commits, int count, int selected, const char *branch,
                     const char *head_hash, int behind_count, int ahead_count, int rows, int cols) {
    term_clear_screen();

    if (cols < 70) cols = 80;

    int hash_w = 7;
    int author_w = 12;
    int date_w = 16;
    int subject_w = cols - (hash_w + author_w + date_w + 16);
    if (subject_w < 20) subject_w = 20;

    printf("%s%s┌─────────────────────────────────────────────────────────────────────────────┐%s\n", ANSI_BOLD, ANSI_CYAN, ANSI_RESET);
    printf("%s%s│                      MGIT TUI HISTORY INSPECTOR                             │%s\n", ANSI_BOLD, ANSI_CYAN, ANSI_RESET);
    printf("%s%s└─────────────────────────────────────────────────────────────────────────────┘%s\n", ANSI_BOLD, ANSI_CYAN, ANSI_RESET);
    
    /* Header Info */
    printf(" %sBranch:%s %s%s%s | %sCommits:%s %s%d%s",
           ANSI_BRIGHT_BLACK, ANSI_RESET, ANSI_BOLD, branch, ANSI_RESET,
           ANSI_BRIGHT_BLACK, ANSI_RESET, ANSI_BOLD, count, ANSI_RESET);

    int is_detached = (strcmp(branch, "HEAD") == 0 || strncmp(branch, "(HEAD detached", 14) == 0);

    if (is_detached) {
        printf(" | %sStatus:%s %sDetached HEAD%s\n",
               ANSI_BRIGHT_BLACK, ANSI_RESET, ANSI_BRIGHT_YELLOW, ANSI_RESET);
    } else if (behind_count > 0) {
        printf(" | %sStatus:%s %sPull Needed (%d behind)%s\n",
               ANSI_BRIGHT_BLACK, ANSI_RESET, ANSI_BRIGHT_YELLOW, behind_count, ANSI_RESET);
    } else if (ahead_count > 0) {
        printf(" | %sStatus:%s %sAhead (%d commits)%s\n",
               ANSI_BRIGHT_BLACK, ANSI_RESET, ANSI_BRIGHT_CYAN, ahead_count, ANSI_RESET);
    } else {
        printf(" | %sStatus:%s %sUp to date%s\n",
               ANSI_BRIGHT_BLACK, ANSI_RESET, ANSI_BRIGHT_GREEN, ANSI_RESET);
    }

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
        int is_head = (head_hash && strlen(head_hash) > 0 && strncmp(commits[i].hash, head_hash, strlen(commits[i].hash)) == 0);

        if (i == selected) {
            /* Selected row in TUI */
            if (is_head) {
                printf("%s %s %-7.7s │ %-12.12s │ %-16.16s │ %-.*s %s%s\n",
                       ANSI_BG_CYAN, ANSI_BOLD,
                       commits[i].hash, commits[i].author, commits[i].date,
                       subject_w, commits[i].subject,
                       MGIT_HEAD_BADGE, ANSI_RESET);
            } else {
                printf("%s %s %-7.7s │ %-12.12s │ %-16.16s │ %-.*s %s\n",
                       ANSI_BG_CYAN, ANSI_BOLD,
                       commits[i].hash, commits[i].author, commits[i].date,
                       subject_w, commits[i].subject,
                       ANSI_RESET);
            }
        } else {
            /* Unselected row */
            if (is_head) {
                /* Current HEAD highlighted in GREEN */
                printf("   %s%s%-7.7s%s │ %s%-12.12s%s │ %s%-16.16s%s │ %s%-.*s %s\n",
                       ANSI_BOLD, ANSI_BRIGHT_GREEN, commits[i].hash, ANSI_RESET,
                       ANSI_BRIGHT_WHITE, commits[i].author, ANSI_RESET,
                       ANSI_BRIGHT_BLACK, commits[i].date, ANSI_RESET,
                       ANSI_RESET, subject_w, commits[i].subject, MGIT_HEAD_BADGE);
            } else {
                printf("   %s%-7.7s%s │ %s%-12.12s%s │ %s%-16.16s%s │ %s%-.*s%s\n",
                       ANSI_BRIGHT_YELLOW, commits[i].hash, ANSI_RESET,
                       ANSI_BRIGHT_WHITE, commits[i].author, ANSI_RESET,
                       ANSI_BRIGHT_BLACK, commits[i].date, ANSI_RESET,
                       ANSI_RESET, subject_w, commits[i].subject, ANSI_RESET);
            }
        }
    }

    printf("%s───────────────────────────────────────────────────────────────────────────────%s\n", ANSI_BRIGHT_BLACK, ANSI_RESET);
    printf("%s[UP/DOWN]%s Navigate  %s[S]%s Switch  %s[P]%s Pull  %s[R]%s Restauration  %s[Q]%s Quit\n",
           ANSI_BRIGHT_CYAN, ANSI_RESET,
           ANSI_BRIGHT_GREEN, ANSI_RESET,
           ANSI_BRIGHT_YELLOW, ANSI_RESET,
           ANSI_BRIGHT_RED, ANSI_RESET,
           ANSI_BRIGHT_MAGENTA, ANSI_RESET);
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

    char head_hash[64] = "";
    git_get_head_hash(head_hash, sizeof(head_hash));

    int behind_count = 0, ahead_count = 0;
    git_check_pull_status(&behind_count, &ahead_count);

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

        draw_tui(commits, count, selected, branch, head_hash, behind_count, ahead_count, rows, cols);

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

            case KEY_PULL: {
                term_disable_raw_mode();
                term_clear_screen();
                printf("\n%s %sPULL FROM REMOTE (git pull)%s\n", MGIT_BADGE, ANSI_BOLD, ANSI_RESET);
                git_pull_quiet(branch);

                printf("\nPress ENTER to return to TUI...");
                fflush(stdout);
                getchar();

                /* Refresh history, head hash, and pull status */
                git_free_history(commits, count);
                git_get_current_branch(branch, sizeof(branch));
                git_get_head_hash(head_hash, sizeof(head_hash));
                git_check_pull_status(&behind_count, &ahead_count);
                git_get_history(&commits, &count);
                if (selected >= count && count > 0) selected = count - 1;

                term_enable_raw_mode();
                break;
            }

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
                git_get_head_hash(head_hash, sizeof(head_hash));
                git_check_pull_status(&behind_count, &ahead_count);
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
                git_get_head_hash(head_hash, sizeof(head_hash));
                git_check_pull_status(&behind_count, &ahead_count);
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
