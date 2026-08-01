#include "uninstall.h"
#include "../utils/term_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cmd_uninstall(int argc, char **argv) {
    (void)argc;
    (void)argv;

    printf("\n%s %sStarting uninstallation of mgit...%s\n", MGIT_INFO_BADGE, ANSI_BOLD, ANSI_RESET);

    char *home = getenv("HOME");
    if (!home) home = getenv("USERPROFILE");

    if (home) {
        char path[1024];

        /* Remove binary folder ~/.mgit */
        snprintf(path, sizeof(path), "%s/.mgit", home);
        char cmd[2048];
#ifdef _WIN32
        snprintf(cmd, sizeof(cmd), "rmdir /s /q \"%s\" 2>NUL", path);
#else
        snprintf(cmd, sizeof(cmd), "rm -rf \"%s\"", path);
#endif
        system(cmd);

        /* Clean .bashrc */
        snprintf(path, sizeof(path), "%s/.bashrc", home);
        FILE *fp = fopen(path, "r");
        if (fp) {
            char temp_path[1024];
            snprintf(temp_path, sizeof(temp_path), "%s/.bashrc.tmp", home);
            FILE *fp_out = fopen(temp_path, "w");
            if (fp_out) {
                char line[512];
                while (fgets(line, sizeof(line), fp)) {
                    if (strstr(line, "mgit") == NULL && strstr(line, "alias mg=") == NULL) {
                        fputs(line, fp_out);
                    }
                }
                fclose(fp_out);
                fclose(fp);
                remove(path);
                rename(temp_path, path);
            } else {
                fclose(fp);
            }
        }

        /* Clean .zshrc */
        snprintf(path, sizeof(path), "%s/.zshrc", home);
        fp = fopen(path, "r");
        if (fp) {
            char temp_path[1024];
            snprintf(temp_path, sizeof(temp_path), "%s/.zshrc.tmp", home);
            FILE *fp_out = fopen(temp_path, "w");
            if (fp_out) {
                char line[512];
                while (fgets(line, sizeof(line), fp)) {
                    if (strstr(line, "mgit") == NULL && strstr(line, "alias mg=") == NULL) {
                        fputs(line, fp_out);
                    }
                }
                fclose(fp_out);
                fclose(fp);
                remove(path);
                rename(temp_path, path);
            } else {
                fclose(fp);
            }
        }
    }

    printf("\n%s %smgit has been cleanly uninstalled.%s\n\n", MGIT_SUCCESS_BADGE, ANSI_BRIGHT_GREEN, ANSI_RESET);
    return 0;
}
