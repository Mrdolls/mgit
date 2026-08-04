#include "update.h"
#include "../utils/term_utils.h"
#include <stdio.h>
#include <stdlib.h>

int cmd_update(int argc, char **argv) {
    (void)argc;
    (void)argv;

    printf("\n%s %sChecking for updates...%s\n", MGIT_BADGE, ANSI_BOLD, ANSI_RESET);

    /* Quietly check if we are in the source repository */
#ifdef _WIN32
    int ret = system("git pull origin master > nul 2>&1");
#else
    int ret = system("git pull origin master > /dev/null 2>&1");
#endif

    if (ret == 0) {
        printf("%s %sRebuilding mgit executable...%s\n", MGIT_STEP_PREFIX, ANSI_BOLD, ANSI_RESET);
        system("make");
        printf("\n%s %sUpdate complete!%s\n\n", MGIT_SUCCESS_BADGE, ANSI_BRIGHT_GREEN, ANSI_RESET);
        return 0;
    }

    printf("%s %sDownloading and installing latest version from GitHub...%s\n", MGIT_STEP_PREFIX, ANSI_BOLD, ANSI_RESET);
    ret = system("curl -sSL https://raw.githubusercontent.com/Mrdolls/mgit/master/install.sh | bash");

    if (ret == 0) {
        printf("\n%s %sSuccessfully updated to the latest version.%s\n\n", MGIT_SUCCESS_BADGE, ANSI_BRIGHT_GREEN, ANSI_RESET);
    } else {
        printf("\n%s %sFailed to update mgit.%s\n\n", MGIT_ERROR_BADGE, ANSI_BRIGHT_RED, ANSI_RESET);
    }

    return ret;
}
