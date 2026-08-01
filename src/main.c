#include <stdio.h>
#include <string.h>
#include "utils/term_utils.h"
#include "commands/push.h"
#include "commands/tui_show.h"
#include "commands/update.h"
#include "commands/uninstall.h"

#define MGIT_VERSION "0.1.0"

static void print_usage(void) {
    printf("\n%s %sLightweight Git Workflow Automation CLI (v%s)%s\n\n",
           MGIT_BADGE, ANSI_BOLD, MGIT_VERSION, ANSI_RESET);
    printf("%sUSAGE:%s\n", ANSI_BOLD, ANSI_RESET);
    printf("  %smg%s %s<command>%s %s[options]%s\n\n", ANSI_BRIGHT_CYAN, ANSI_RESET, ANSI_BRIGHT_YELLOW, ANSI_RESET, ANSI_BRIGHT_BLACK, ANSI_RESET);
    printf("%sAVAILABLE COMMANDS:%s\n", ANSI_BOLD, ANSI_RESET);
    printf("  %spush [message]%s   Automate git add ., git commit -m [message] and git push\n", ANSI_BRIGHT_CYAN, ANSI_RESET);
    printf("                     %s(Default commit message: \"Auto push\")%s\n", ANSI_BRIGHT_BLACK, ANSI_RESET);
    printf("  %sshow%s             Open interactive TUI history viewer with Switch & Restauration\n", ANSI_BRIGHT_CYAN, ANSI_RESET);
    printf("  %supdate%s           Download and apply the latest version from GitHub\n", ANSI_BRIGHT_CYAN, ANSI_RESET);
    printf("  %suninstall%s        Cleanly uninstall mgit and remove shell configuration\n", ANSI_BRIGHT_CYAN, ANSI_RESET);
    printf("  %shelp%s             Show this help message\n", ANSI_BRIGHT_CYAN, ANSI_RESET);
    printf("  %sversion%s          Show version information\n\n", ANSI_BRIGHT_CYAN, ANSI_RESET);
    printf("%sEXAMPLES:%s\n", ANSI_BOLD, ANSI_RESET);
    printf("  %smg push \"Fix user authentication bug\"%s\n", ANSI_BRIGHT_GREEN, ANSI_RESET);
    printf("  %smg show%s\n", ANSI_BRIGHT_GREEN, ANSI_RESET);
    printf("  %smg update%s\n\n", ANSI_BRIGHT_GREEN, ANSI_RESET);
}

int main(int argc, char **argv) {
    term_init();

    if (argc < 2) {
        print_usage();
        return 0;
    }

    const char *subcmd = argv[1];

    if (strcmp(subcmd, "push") == 0) {
        return cmd_push(argc - 2, argv + 2);
    } else if (strcmp(subcmd, "show") == 0) {
        return cmd_show(argc - 2, argv + 2);
    } else if (strcmp(subcmd, "update") == 0) {
        return cmd_update(argc - 2, argv + 2);
    } else if (strcmp(subcmd, "uninstall") == 0) {
        return cmd_uninstall(argc - 2, argv + 2);
    } else if (strcmp(subcmd, "version") == 0 || strcmp(subcmd, "--version") == 0 || strcmp(subcmd, "-v") == 0) {
        printf("%s version %s%s%s\n", MGIT_BADGE, ANSI_BOLD, MGIT_VERSION, ANSI_RESET);
        return 0;
    } else if (strcmp(subcmd, "help") == 0 || strcmp(subcmd, "--help") == 0 || strcmp(subcmd, "-h") == 0) {
        print_usage();
        return 0;
    } else {
        printf("\n%s %sUnknown command: %s%s\n\n", MGIT_ERROR_BADGE, ANSI_BRIGHT_RED, subcmd, ANSI_RESET);
        print_usage();
        return 1;
    }
}
