#!/usr/bin/env bash
set -e

# ANSI Color Palette
RESET="\033[0m"
BOLD="\033[1m"
DIM="\033[2m"
CYAN="\033[1;96m"
GREEN="\033[1;92m"
YELLOW="\033[1;93m"
BLUE="\033[1;94m"
RED="\033[1;91m"
GRAY="\033[90m"

BADGE="\033[1;96m[mgit]\033[0m"
STEP_PREFIX="\033[1;94m  ==>\033[0m"
SUCCESS_BADGE="\033[1;92m[SUCCESS]\033[0m"
ERROR_BADGE="\033[1;91m[ERROR]\033[0m"

INSTALL_DIR="$HOME/.mgit"
BIN_DIR="$INSTALL_DIR/bin"
TARGET_BIN="$BIN_DIR/mg"

echo ""
echo -e "${CYAN}┌──────────────────────────────────────────────────────────┐${RESET}"
echo -e "${CYAN}│               MGIT CLI INSTALLER (v0.1.0)                │${RESET}"
echo -e "${CYAN}└──────────────────────────────────────────────────────────┘${RESET}"
echo ""

# Step 1: Directory Setup
echo -e "${STEP_PREFIX} Creating target directory: ${GRAY}${BIN_DIR}${RESET}"
mkdir -p "$BIN_DIR"

# Step 2: Build / Copy Binary
if [ -f "Makefile" ]; then
    echo -e "${STEP_PREFIX} Compiling binary from local source..."
    make clean > /dev/null 2>&1 || true
    make > /dev/null 2>&1
    cp mg* "$BIN_DIR/"
else
    echo -e "${STEP_PREFIX} Fetching and building latest release..."
    TMP_DIR=$(mktemp -d)
    git clone --quiet https://github.com/Mrdolls/mgit.git "$TMP_DIR/mgit" > /dev/null 2>&1
    cd "$TMP_DIR/mgit"
    make > /dev/null 2>&1
    cp mg* "$BIN_DIR/"
    rm -rf "$TMP_DIR"
fi

# Step 3: Shell Configuration
echo -e "${STEP_PREFIX} Configuring shell profiles..."
SHELL_CONFIGS=("$HOME/.bashrc" "$HOME/.zshrc")
UPDATED_SHELLS=""

for CONFIG in "${SHELL_CONFIGS[@]}"; do
    touch "$CONFIG"
    if ! grep -q "mgit/bin" "$CONFIG"; then
        echo "" >> "$CONFIG"
        echo "# mgit CLI configuration" >> "$CONFIG"
        echo "export PATH=\"$BIN_DIR:\$PATH\"" >> "$CONFIG"
        echo "alias mg=\"$BIN_DIR/mg\"" >> "$CONFIG"
        UPDATED_SHELLS="$UPDATED_SHELLS $(basename "$CONFIG")"
    fi
done

# Step 4: Installation Summary Card
echo ""
echo -e "${GRAY}────────────────────────────────────────────────────────────${RESET}"
echo -e "${SUCCESS_BADGE} ${GREEN}Installation finished successfully!${RESET}"
echo -e "${GRAY}────────────────────────────────────────────────────────────${RESET}"
echo -e "  ${BOLD}Binary Location${RESET} : ${CYAN}${TARGET_BIN}${RESET}"
if [ -n "$UPDATED_SHELLS" ]; then
    echo -e "  ${BOLD}Shell Profiles${RESET}  :${GRAY}${UPDATED_SHELLS}${RESET}"
fi
echo ""
echo -e "  ${BOLD}Quick Start${RESET}     : ${GREEN}mg help${RESET}"
echo -e "  ${BOLD}Uninstallation${RESET}  : ${YELLOW}mg uninstall${RESET} ${GRAY}(or remove ${BIN_DIR})${RESET}"
echo -e "${GRAY}────────────────────────────────────────────────────────────${RESET}"
echo -e " ${DIM}Note: Run 'source ~/.zshrc' (or source ~/.bashrc) to refresh.${RESET}"
echo ""
