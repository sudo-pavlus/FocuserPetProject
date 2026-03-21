# 🐾 FocuserPet

A lightweight animated desktop companion built with C++ and SFML that lives on your Windows taskbar. It walks, idles, reacts to your cursor, and can be dragged around the screen.

---

## ✨ Features

- **Animated sprite system** — Multiple states: Idle, Walk, Dialogue, Attack, Protection
- **Taskbar-anchored** — Automatically detects taskbar position and sits right above it
- **Cursor interaction** — Switches to Protection animation when hovered, resumes normal behavior when cursor leaves
- **Drag & drop** — Click and drag the pet anywhere on screen; releases and falls back to the taskbar with physics
- **Autonomous behavior** — Cycles through Idle → Walk → Dialogue states on a timer when left alone
- **Transparent window** — Uses magenta color key (`RGB(255, 0, 255)`) for borderless, always-on-top rendering
- **Always on top** — Sits above all other windows via `HWND_TOPMOST`

---

## 🎬 States

| State | Trigger | Frames |
|-------|---------|--------|
| `IDLE` | Default / after drag | 7 |
| `WALK` | Auto after 3s idle | 12 |
| `DIALOGUE` | Auto after walk | 6 |
| `ATTACK` | — | 9 |
| `PROTECTION` | Cursor hover | 2 |

---

## 🏗️ Architecture

```
FocuserPet.cpp
├── GetTaskbarTop()        — Win32 API taskbar position detection
├── PetState enum          — IDLE, WALK, DIOLOGUE, ATTACK, PROTECTION
├── PetAnimation struct    — texture, frameCount, frameDuration, stride
├── Animation loop         — frame-based sprite sheet slicing
├── Drag system            — global mouse offset tracking
├── Fall physics           — velocity-based drop to taskbar on release
└── Transparent window     — WS_EX_LAYERED + LWA_COLORKEY
```

---

## 🛠️ Requirements

- Windows 10/11
- [SFML 3.0.2](https://www.sfml-dev.org/download/) (x64)
- Visual Studio 2022 with C++20

---

## ⚙️ Setup

### 1. Install SFML

Download SFML 3.0.2 and extract to `C:\SFML-3.0.2\`

```
C:\SFML-3.0.2\
├── include\
├── lib\
└── bin\
```

### 2. Open the project

Open `FocuserPetProject.slnx` in Visual Studio 2022.

### 3. Add your sprite sheets

Place sprite sheets in:

```
FocuserPet/assets/sprites/Test_girl/
├── Idle.png        (7 frames, 128x72px each)
├── Walk.png        (12 frames)
├── Dialogue.png    (6 frames)
├── Attack.png      (9 frames)
└── Protection.png  (2 frames)
```

Each sprite sheet is a horizontal strip of `128x72` frames (128px wide, 128-56=72px tall).

### 4. Build & Run

Set configuration to **Debug x64** or **Release x64** and hit `F5`.

SFML `.dll` files are automatically copied to the output directory via the post-build event.

---

## 🎨 Sprite Sheet Format

```
[ Frame 0 ][ Frame 1 ][ Frame 2 ][ Frame 3 ] ...
  128px      128px      128px      128px
  <──────────── total width = frames × 128 ──────────────>
```

Height: `128 - 56 = 72px`
Scale: `2.5x` (rendered at `320x180px`)

---

## 🖱️ Controls

| Action | Result |
|--------|--------|
| Hover over pet | Protection animation |
| Left click + drag | Move pet freely |
| Release | Pet falls back to taskbar |
| `Escape` | Close |

---

## 📁 Project Structure

```
FocuserPetProject.slnx
└── FocuserPet/
    ├── FocuserPet.cpp          # Main application logic
    ├── FocuserPet.h
    ├── FocuserPet.vcxproj      # MSVC project file
    ├── Resource.h
    ├── framework.h
    ├── targetver.h
    └── assets/
        └── sprites/
            └── Test_girl/      # Sprite sheets go here
```

---

## 🔧 Tech Stack

`C++20` `SFML 3.0.2` `Win32 API` `Visual Studio 2022` `Windows 10/11`

---

## 📝 License

Mozilla Public License 2.0 — see `LICENSE.txt` for details.
