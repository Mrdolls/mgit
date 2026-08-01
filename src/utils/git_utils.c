#ifdef _WIN32
#include <stdio.h>
#define popen _popen
#define pclose _pclose
#else
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#endif

#include "git_utils.h"
#include <stdlib.h>
#include <string.h>

int git_is_repository(void) {
    int ret = system("git rev-parse --is-inside-work-tree > NUL 2>&1");
    if (ret != 0) {
        ret = system("git rev-parse --is-inside-work-tree > /dev/null 2>&1");
    }
    return (ret == 0);
}

int git_get_current_branch(char *buf, size_t size) {
    FILE *fp = popen("git rev-parse --abbrev-ref HEAD 2>", "r");
    if (!fp) {
        fp = popen("git rev-parse --abbrev-ref HEAD", "r");
    }
    if (!fp) return 0;

    if (fgets(buf, (int)size, fp) != NULL) {
        buf[strcspn(buf, "\r\n")] = '\0';
        pclose(fp);
        return 1;
    }
    pclose(fp);
    return 0;
}

int git_add_all(void) {
    printf("[mgit] Running: git add .\n");
    return system("git add .");
}

int git_commit(const char *message) {
    char cmd[1024];
    printf("[mgit] Running: git commit -m \"%s\"\n", message);
    snprintf(cmd, sizeof(cmd), "git commit -m \"%s\"", message);
    return system(cmd);
}

int git_push(const char *branch) {
    char cmd[1024];
    if (branch && strlen(branch) > 0) {
        printf("[mgit] Running: git push origin %s\n", branch);
        snprintf(cmd, sizeof(cmd), "git push origin %s", branch);
    } else {
        printf("[mgit] Running: git push\n");
        snprintf(cmd, sizeof(cmd), "git push");
    }
    return system(cmd);
}

int git_checkout(const char *target) {
    char cmd[1024];
    printf("[mgit] Running: git checkout %s\n", target);
    snprintf(cmd, sizeof(cmd), "git checkout %s", target);
    return system(cmd);
}

int git_reset_hard(const char *target) {
    char cmd[1024];
    printf("[mgit] Running: git reset --hard %s\n", target);
    snprintf(cmd, sizeof(cmd), "git reset --hard %s", target);
    return system(cmd);
}

int git_get_history(CommitInfo **commits_out, int *count_out) {
    *commits_out = NULL;
    *count_out = 0;

    FILE *fp = popen("git log -n 50 --pretty=format:\"%h|%an|%ar|%s\"", "r");
    if (!fp) return 0;

    int capacity = 50;
    CommitInfo *commits = malloc(capacity * sizeof(CommitInfo));
    if (!commits) {
        pclose(fp);
        return 0;
    }

    char line[512];
    int count = 0;

    while (fgets(line, sizeof(line), fp) != NULL && count < capacity) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0) continue;

        char *token = strtok(line, "|");
        if (token) strncpy(commits[count].hash, token, sizeof(commits[count].hash) - 1);
        else commits[count].hash[0] = '\0';

        token = strtok(NULL, "|");
        if (token) strncpy(commits[count].author, token, sizeof(commits[count].author) - 1);
        else commits[count].author[0] = '\0';

        token = strtok(NULL, "|");
        if (token) strncpy(commits[count].date, token, sizeof(commits[count].date) - 1);
        else commits[count].date[0] = '\0';

        token = strtok(NULL, "|");
        if (token) strncpy(commits[count].subject, token, sizeof(commits[count].subject) - 1);
        else commits[count].subject[0] = '\0';

        count++;
    }

    pclose(fp);
    *commits_out = commits;
    *count_out = count;
    return 1;
}

void git_free_history(CommitInfo *commits, int count) {
    (void)count;
    if (commits) free(commits);
}
