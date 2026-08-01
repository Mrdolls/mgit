#include "git_utils.h"
#include "term_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int git_is_repository(void) {
#ifdef _WIN32
    return (system("git rev-parse --is-inside-work-tree > nul 2>&1") == 0);
#else
    return (system("git rev-parse --is-inside-work-tree > /dev/null 2>&1") == 0);
#endif
}

int git_has_changes(void) {
#ifdef _WIN32
    FILE *fp = popen("git status --porcelain 2>nul", "r");
#else
    FILE *fp = popen("git status --porcelain 2>/dev/null", "r");
#endif
    if (!fp) return 0;

    char buffer[256];
    int has_changes = (fgets(buffer, sizeof(buffer), fp) != NULL);
    pclose(fp);

    return has_changes;
}

int git_has_unpushed_commits(void) {
#ifdef _WIN32
    FILE *fp = popen("git log @{u}..HEAD --oneline 2>nul", "r");
#else
    FILE *fp = popen("git log @{u}..HEAD --oneline 2>/dev/null", "r");
#endif
    if (!fp) return 0;

    char buffer[256];
    int has_unpushed = (fgets(buffer, sizeof(buffer), fp) != NULL);
    pclose(fp);

    return has_unpushed;
}

int git_get_current_branch(char *buf, size_t size) {
#ifdef _WIN32
    FILE *fp = popen("git rev-parse --abbrev-ref HEAD 2>nul", "r");
#else
    FILE *fp = popen("git rev-parse --abbrev-ref HEAD 2>/dev/null", "r");
#endif
    if (!fp) return 0;

    if (fgets(buf, (int)size, fp) != NULL) {
        buf[strcspn(buf, "\r\n")] = '\0';
        pclose(fp);
        return 1;
    }
    pclose(fp);

    strncpy(buf, "HEAD", size);
    buf[size - 1] = '\0';
    return 0;
}

int git_get_head_hash(char *buf, size_t size) {
#ifdef _WIN32
    FILE *fp = popen("git rev-parse --short HEAD 2>nul", "r");
#else
    FILE *fp = popen("git rev-parse --short HEAD 2>/dev/null", "r");
#endif
    if (!fp) return 0;

    if (fgets(buf, (int)size, fp) != NULL) {
        buf[strcspn(buf, "\r\n")] = '\0';
        pclose(fp);
        return 1;
    }
    pclose(fp);
    return 0;
}

int git_check_pull_status(int *behind_count, int *ahead_count) {
    *behind_count = 0;
    *ahead_count = 0;

#ifdef _WIN32
    FILE *fp = popen("git rev-list --left-right --count HEAD...@{u} 2>nul", "r");
#else
    FILE *fp = popen("git rev-list --left-right --count HEAD...@{u} 2>/dev/null", "r");
#endif
    if (!fp) return 0;

    char buffer[128];
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        int ahead = 0, behind = 0;
        if (sscanf(buffer, "%d\t%d", &ahead, &behind) == 2 || sscanf(buffer, "%d %d", &ahead, &behind) == 2) {
            *ahead_count = ahead;
            *behind_count = behind;
        }
    }
    pclose(fp);
    return 1;
}

int git_add_all(void) {
    printf("%s %sStaging files%s (%sgit add .%s)\n",
           MGIT_STEP_PREFIX, ANSI_BOLD, ANSI_RESET, ANSI_BRIGHT_CYAN, ANSI_RESET);
#ifdef _WIN32
    return system("git add . > nul 2>&1");
#else
    return system("git add . > /dev/null 2>&1");
#endif
}

int git_commit(const char *message) {
    char cmd[1024];
    printf("%s %sCreating commit%s (%sgit commit -m \"%s\"%s)\n",
           MGIT_STEP_PREFIX, ANSI_BOLD, ANSI_RESET, ANSI_BRIGHT_YELLOW, message, ANSI_RESET);
#ifdef _WIN32
    snprintf(cmd, sizeof(cmd), "git commit -q -m \"%s\" > nul 2>&1", message);
#else
    snprintf(cmd, sizeof(cmd), "git commit -q -m \"%s\" > /dev/null 2>&1", message);
#endif
    return system(cmd);
}

int git_push(const char *branch) {
    char cmd[1024];
    if (branch && strlen(branch) > 0) {
        printf("%s %sPushing to remote%s (%sgit push origin %s%s)\n",
               MGIT_STEP_PREFIX, ANSI_BOLD, ANSI_RESET, ANSI_BRIGHT_MAGENTA, branch, ANSI_RESET);
#ifdef _WIN32
        snprintf(cmd, sizeof(cmd), "git push --quiet origin %s > nul 2>&1", branch);
#else
        snprintf(cmd, sizeof(cmd), "git push --quiet origin %s > /dev/null 2>&1", branch);
#endif
    } else {
        printf("%s %sPushing to remote%s (%sgit push%s)\n",
               MGIT_STEP_PREFIX, ANSI_BOLD, ANSI_RESET, ANSI_BRIGHT_MAGENTA, ANSI_RESET);
#ifdef _WIN32
        snprintf(cmd, sizeof(cmd), "git push --quiet > nul 2>&1");
#else
        snprintf(cmd, sizeof(cmd), "git push --quiet > /dev/null 2>&1");
#endif
    }
    return system(cmd);
}

int git_pull_quiet(const char *branch) {
    char cmd[1024];
    if (branch && strlen(branch) > 0) {
        printf("%s %sPulling from remote%s (%sgit pull origin %s%s)\n",
               MGIT_STEP_PREFIX, ANSI_BOLD, ANSI_RESET, ANSI_BRIGHT_CYAN, branch, ANSI_RESET);
        snprintf(cmd, sizeof(cmd), "git pull origin %s", branch);
    } else {
        printf("%s %sPulling from remote%s (%sgit pull%s)\n",
               MGIT_STEP_PREFIX, ANSI_BOLD, ANSI_RESET, ANSI_BRIGHT_CYAN, ANSI_RESET);
        snprintf(cmd, sizeof(cmd), "git pull");
    }
    return system(cmd);
}

int git_checkout(const char *target) {
    char cmd[1024];
    printf("%s %sChecking out%s (%sgit checkout %s%s)\n",
           MGIT_STEP_PREFIX, ANSI_BOLD, ANSI_RESET, ANSI_BRIGHT_CYAN, target, ANSI_RESET);
    snprintf(cmd, sizeof(cmd), "git checkout %s", target);
    return system(cmd);
}

int git_reset_hard(const char *target) {
    char cmd[1024];
    printf("%s %sResetting hard%s (%sgit reset --hard %s%s)\n",
           MGIT_STEP_PREFIX, ANSI_BOLD, ANSI_RESET, ANSI_BRIGHT_RED, target, ANSI_RESET);
    snprintf(cmd, sizeof(cmd), "git reset --hard %s", target);
    return system(cmd);
}

int git_get_history(CommitInfo **commits_out, int *count_out) {
    *commits_out = NULL;
    *count_out = 0;

    FILE *fp = popen("git log --all -n 50 --pretty=format:\"%h|%an|%ar (%cd)|%s\" --date=format:\"%d/%m/%Y\"", "r");
    if (!fp) return 0;

    int capacity = 50;
    CommitInfo *commits = malloc(sizeof(CommitInfo) * capacity);
    if (!commits) {
        pclose(fp);
        return 0;
    }

    int count = 0;
    char line[512];

    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0) continue;

        char *token_hash = strtok(line, "|");
        char *token_author = strtok(NULL, "|");
        char *token_date = strtok(NULL, "|");
        char *token_subject = strtok(NULL, "|");

        if (token_hash && token_author && token_date && token_subject) {
            strncpy(commits[count].hash, token_hash, sizeof(commits[count].hash) - 1);
            commits[count].hash[sizeof(commits[count].hash) - 1] = '\0';

            strncpy(commits[count].author, token_author, sizeof(commits[count].author) - 1);
            commits[count].author[sizeof(commits[count].author) - 1] = '\0';

            strncpy(commits[count].date, token_date, sizeof(commits[count].date) - 1);
            commits[count].date[sizeof(commits[count].date) - 1] = '\0';

            strncpy(commits[count].subject, token_subject, sizeof(commits[count].subject) - 1);
            commits[count].subject[sizeof(commits[count].subject) - 1] = '\0';

            count++;
            if (count >= capacity) break;
        }
    }

    pclose(fp);
    *commits_out = commits;
    *count_out = count;
    return 1;
}

void git_free_history(CommitInfo *commits, int count) {
    (void)count;
    if (commits) {
        free(commits);
    }
}
