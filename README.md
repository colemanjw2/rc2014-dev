```md
# RC2014 Development Workflow (BASIC + HLOAD)

This project targets an **RC2014** system running **BASIC with HLOAD ROM**.
The goal is a zero-friction development loop:

> Edit C → one keypress → HEX streams → program runs

No CP/M. No disks. No manual pasting.

---

## TL;DR (Recovery Checklist)

If nothing makes sense later, do this:

1. Plug in USB-serial adapter
2. On RC2014, type:
```

HLOAD

```
3. In VS Code, press:
```

Ctrl + U

```
4. Program compiles, uploads, and runs

---

## Hardware Setup

### Serial Connection
- USB-to-TTL serial adapter
- **Bluetooth HC-05 REMOVED**
- HC-05 TX was holding RX and blocking inbound data

### Wiring
```

USB-Serial TX  → RC2014 RX
USB-Serial RX  → RC2014 TX
GND            ↔ GND

```

### Serial Parameters
- Baud rate: **115200**
- Data: 8-N-1
- Flow control: **OFF** (no RTS/CTS, no XON/XOFF)

Linux device:
```

/dev/ttyUSB0

```

---

## RC2014 Software Mode

- ROM: **BASIC with HLOAD**
- Loader command:
```

HLOAD

```
- Input format: **Intel HEX (.ihx)**
- Program auto-starts after load
- No CP/M involved

---

## Host Development Environment

- OS: Linux (Ubuntu)
- Editor: VS Code
- Compiler: **z88dk**
- Upload method: raw serial stream
- Progress indicator: `pv`

---

## VS Code Configuration (Source of Truth)

All automation lives in:

```

.vscode/tasks.json

````

### Build Task: `RC2014-build`

```json
{
  "label": "RC2014-build",
  "type": "shell",
  "command": "zcc",
  "args": [
    "+rc2014",
    "-clib=new",
    "-create-app",
    "-org=9000",
    "${file}",
    "-o",
    "${fileBasenameNoExtension}"
  ],
  "group": {
    "kind": "build",
    "isDefault": true
  }
}
````

**What this does**

* Compiles the currently open `.c` file
* Targets RC2014
* Links program to start at **0x9000**
* Produces:

  ```
  filename.ihx
  ```

---

### Upload Task: `RC2014-upload`

```json
{
  "label": "RC2014-upload",
  "type": "shell",
  "command": "bash",
  "args": [
    "-c",
    "/usr/bin/pv -p -t -e -r ${fileBasenameNoExtension}.ihx > /dev/ttyUSB0"
  ],
  "dependsOn": "RC2014-build"
}
```

**What this does**

* Automatically runs `RC2014-build`
* Streams the resulting `.ihx` over serial
* Shows a progress bar
* No pacing (wired serial is fast enough)

---

## Keyboard Shortcut (Critical)

Defined in:

```
~/.config/Code/User/keybindings.json
```

```json
{
  "key": "ctrl+u",
  "command": "workbench.action.tasks.runTask",
  "args": "RC2014-upload",
  "when": "editorTextFocus"
}
```

### Meaning

**Ctrl + U** =

1. Compile current C file
2. Upload HEX to RC2014
3. Program runs immediately

This is the entire development loop.

---

## Optional: Serial Monitor

For observing output or manual interaction:

```bash
sudo minicom -D /dev/ttyUSB0 -b 115200
```

Minicom settings:

* Hardware flow control: **No**
* Software flow control: **No**

Uploads do **not** go through minicom.

---

## Normal Development Flow

1. Open project in VS Code
2. Edit a `.c` file
3. On RC2014:

   ```
   HLOAD
   ```
4. In VS Code:

   ```
   Ctrl + U
   ```
5. Watch progress bar
6. Program executes

---

## If Things Break

Check these in order:

* Is the HC-05 disconnected?
* Is `/dev/ttyUSB0` correct?
* Is RC2014 waiting at `HLOAD`?
* Did you press **Ctrl + U** (not build only)?
* Does `pv` show data transferring?
* Are TX/RX crossed correctly?

---

## Design Philosophy

* Treat RC2014 like a ROM-loaded appliance
* No filesystem assumptions
* No CP/M dependency
* One-keystroke iteration

If this setup ever stops making sense, **this README plus `.vscode/tasks.json` are the authoritative record**.

```

