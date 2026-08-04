#ifndef GIT_UTILS_H
#define GIT_UTILS_H

#include <stddef.h>

typedef struct {
    char hash[16];
    char author[64];
    char date[64];
    char subject[256];
} CommitInfo;

int git_is_repository(void);
int git_has_changes(void);
int git_has_unpushed_commits(void);
int git_get_current_branch(char *buf, size_t size);
int git_get_head_hash(char *buf, size_t size);
int git_check_pull_status(int *behind_count, int *ahead_count);
void git_show_changes_summary(void);
void git_show_unpushed_summary(const char *branch);
int git_add_all(void);
int git_commit(const char *message);
int git_push(const char *branch);
int git_pull_quiet(const char *branch);
int git_checkout(const char *target);
int git_reset_hard(const char *target);
int git_get_history(CommitInfo **commits_out, int *count_out);
void git_free_history(CommitInfo *commits, int count);

#endif /* GIT_UTILS_H */
