#include "update.h"
#include "../utils/term_utils.h"
#include <stdio.h>
#include <stdlib.h>

int cmd_update(int argc, char **argv) {
    (void)argc;
    (void)argv;

    printf("%s[mgit] Checking for updates...%s\n", ANSI_CYAN, ANSI_RESET);

    /* Try git pull if in source repo or run installer */
    int ret = system("git pull origin main");
    if (ret == 0) {
        printf("%s[mgit] Rebuilding mgit executable...%s\n", ANSI_CYAN, ANSI_RESET);
        system("make");
        printf("%s[mgit] Update complete!%s\n", ANSI_GREEN, ANSI_RESET);
        return 0;
    }

    printf("%s[mgit] Running remote update script...%s\n", ANSI_CYAN, ANSI_RESET);
    ret = system("curl -sSL https://raw.githubusercontent.com/Mrdolls/mgit/main/install.sh | bash");

    if (ret == 0) {
        printf("%s[mgit] Successfully updated to the latest version.%s\n", ANSI_GREEN, ANSI_RESET);
    } else {
        printf("%s[mgit] Failed to update mgit.%s\n", ANSI_RED, ANSI_RESET);
    }

    return ret;
}
