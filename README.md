# mgit

**mgit** (`mg`) is a lightweight C-based CLI tool with an interactive TUI (Terminal User Interface) designed to automate and streamline daily Git workflows.

---

## Features

- **`mg push [msg]`**: Automates `git add .`, `git commit -m "[msg]"` (default message: `"Auto push"`), and `git push` on your active branch.
- **`mg show`**: Launches an interactive TUI history inspector. Allows navigating commits, inspecting metadata, performing a `Switch` (`git checkout`), or executing a `Restauration` (`git reset --hard`).
- **`mg update`**: Automatically fetches and builds the latest release from GitHub.
- **`mg uninstall`**: Cleanly removes the executable and cleans up configuration entries in `.bashrc` and `.zshrc`.

---

## Quick Installation

Run the one-line installer:

```bash
curl -sSL https://raw.githubusercontent.com/VOTRE_PSEUDO/mgit/main/install.sh | bash
```

Or build manually from source:

```bash
git clone https://github.com/VOTRE_PSEUDO/mgit.git
cd mgit
make
make install
```

---

## Usage & Examples

### Automate Git Push
```bash
# Push with default commit message "Auto push"
mg push

# Push with custom commit message
mg push "Refactor user authentication module"
```

### Interactive TUI Inspector
```bash
mg show
```
In the TUI:
- **Up / Down**: Navigate commit history.
- **S**: Checkout selected commit (`git checkout <hash>`).
- **R**: Perform hard reset (`git reset --hard <hash>`). Requires explicit confirmation.
- **Q / ESC**: Exit TUI.

> **Safety Notice**: Executing a Restauration (`git reset --hard`) will permanently delete uncommitted working tree changes. Always review your changes before confirming.

### Updating & Uninstalling
```bash
# Update mgit to the latest version
mg update

# Uninstall mgit
mg uninstall
```

---

## License

MIT License.
