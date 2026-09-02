# mgit

**mgit** is a lightweight C-based CLI tool with an interactive TUI (Terminal User Interface) designed to automate and streamline daily Git workflows.

---

## Features

- **`mgit <url> [name]`**: Clones a remote repository directly by passing its GitHub URL.
- **`mgit push [msg]`**: Automates `git add .`, `git commit -m "[msg]"` (default message: `"Auto push"`), and `git push` on your active branch.
- **`mgit show`**: Launches an interactive TUI history inspector. Allows navigating commits, inspecting metadata, performing a `Switch` (`git checkout`), or executing a `Restauration` (`git reset --hard`).
- **`mgit update`**: Automatically fetches and builds the latest release from GitHub.
- **`mgit uninstall`**: Cleanly removes the executable and cleans up configuration entries in `.bashrc` and `.zshrc`.

---

## Quick Installation

Run the one-line installer:

```bash
curl -sSL https://raw.githubusercontent.com/Mrdolls/mgit/master/install.sh | bash
```

Or build manually from source:

```bash
git clone git@github.com:Mrdolls/mgit.git
cd mgit
make
make install
```

---

## Usage & Examples

### Direct Repository Cloning
```bash
# Clone directly by passing the GitHub URL
mgit git@github.com:Mrdolls/mgit.git

# Clone into a custom directory name
mgit git@github.com:Mrdolls/mgit.git my_custom_dir
```

### Automate Git Push
```bash
# Push with default commit message "Auto push"
mgit push

# Push with custom commit message
mgit push "Refactor user authentication module"
```

### Interactive TUI Inspector
```bash
mgit show
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
mgit update

# Uninstall mgit
mgit uninstall
```
