# ASVM — Auto Start VMware Machine

A small interactive Linux tool that generates a startup script for a VMware virtual machine and (optionally) registers it in `crontab`, so the VM starts automatically at boot.

## Features

- Interactive prompts, no config file needed
- Generates an executable `asvm.sh` that launches your VM with `vmware`
- Fullscreen (`-X`) or windowed (`-x`) mode
- Optionally adds an `@reboot` entry to your crontab (never adds duplicates)
- Input validation: checks that the directory and `.vmx` file exist, and re-asks on invalid answers
- Paths with spaces just work (no backslash escaping needed)

## Requirements

- Linux
- VMware Workstation / Player with the `vmware` command in `PATH`
- `g++` with C++17 support (uses `std::filesystem`)
- `cron`

## Build

```bash
g++ -std=c++17 main.cpp -o asvm
```

## Usage

Run it from the directory where you want `asvm.sh` to live. The crontab entry points to that script, so don't move it afterwards.

```bash
./asvm
```

You will be asked for:

| Prompt | Example |
|---|---|
| Directory containing the `.vmx` file | `/home/user/vmware/Windows XP Professional/` |
| `.vmx` file name | `Windows XP Professional.vmx` |
| Fullscreen? (1 = yes, 2 = no) | `1` |
| Add to crontab? (1 = yes, 2 = no) | `1` |

Type paths as they are. Backslash-escaped spaces (`\ `) are also accepted.

### What gets generated

`asvm.sh`:

```sh
#!/bin/sh
cd '/home/user/vmware/Windows XP Professional/' || exit 1
vmware 'Windows XP Professional.vmx' -X
```

Crontab entry (if you chose yes):

```
@reboot '/path/to/asvm.sh'
```

## Uninstall

1. Run `crontab -e` and delete the `@reboot .../asvm.sh` line.
2. Delete `asvm.sh`.

## Known limitations

- `@reboot` runs before any desktop login. `vmware` opens a GUI, so it may fail without a graphical session. If the VM doesn't start, consider a desktop autostart entry, a systemd user service, or `vmrun ... nogui`.
- Use absolute paths; `~` is not expanded.

## Tested on
debian13 with vmware workstation 17
## License

## License
MIT License. See [LICENSE](LICENSE).
