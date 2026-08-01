#!/usr/bin/env bash
set -e

# mgit Installer Script
INSTALL_DIR="$HOME/.mgit"
BIN_DIR="$INSTALL_DIR/bin"

echo "[mgit] Installing mgit CLI..."

mkdir -p "$BIN_DIR"

if [ -f "Makefile" ]; then
    echo "[mgit] Building from local source..."
    make clean
    make
    cp mg* "$BIN_DIR/"
else
    echo "[mgit] Cloning remote repository..."
    TMP_DIR=$(mktemp -d)
    git clone https://github.com/Mrdolls/mgit.git "$TMP_DIR/mgit"
    cd "$TMP_DIR/mgit"
    make
    cp mg* "$BIN_DIR/"
    rm -rf "$TMP_DIR"
fi

# Add alias / PATH to shell profiles
SHELL_CONFIGS=("$HOME/.bashrc" "$HOME/.zshrc")

for CONFIG in "${SHELL_CONFIGS[@]}"; do
    # Create file if it doesn't exist yet
    touch "$CONFIG"
    if ! grep -q "mgit/bin" "$CONFIG"; then
        echo "" >> "$CONFIG"
        echo "# mgit CLI configuration" >> "$CONFIG"
        echo "export PATH=\"$BIN_DIR:\$PATH\"" >> "$CONFIG"
        echo "alias mg=\"$BIN_DIR/mg\"" >> "$CONFIG"
        echo "[mgit] Added alias and PATH to $CONFIG"
    fi
done

echo ""
echo "[mgit] Installation completed successfully!"
echo "[mgit] Restart your terminal or run: source ~/.bashrc (or source ~/.zshrc)"
echo "[mgit] Try running: mg help"
