# 🗡️ ROGUE DELUXE

## A C11 Terminal Dungeon Adventure

> Enter the forgotten depths.  
> Fight deadly creatures.  
> Collect ancient artifacts.  
> Escape alive.

---

## 🌌 Game Overview

**Rogue Deluxe** is a terminal-based roguelike adventure game developed in **C11** using the **ncurses** library.

The player explores randomly generated dungeons, fights enemies, collects valuable items, manages resources, and attempts to survive through multiple dangerous floors.

Every dungeon is different.  
Every decision matters.

---

## ✨ Features

### 🏰 Procedural Dungeon Generation

- Randomly generated dungeon floors
- Dynamic rooms and corridors
- Doors, traps, treasures, and hidden paths
- Fog of war and visibility system
- Multi-floor exploration

---

### ⚔️ Combat System

A turn-based combat system including:

- Melee combat
- Ranged attacks
- Enemy encounters
- Damage calculation
- Experience and score system

#### Available Weapons

| Weapon | Description |
|---|---|
| Mace | Heavy damage weapon |
| Dagger | Fast attack weapon |
| Sword | Balanced weapon |
| Bow | Long-range attacks |
| Wand | Magical attacks |

---

### 👹 Enemies

Explore the dungeon and encounter different creatures:

| Enemy | Description |
|---|---|
| 🐍 Snake | Fast and dangerous enemy |
| ☠️ Undead | Dark dungeon creature |
| 🔥 Fire Beast | Powerful elemental enemy |
| 👹 Demon | Magical and deadly creature |
| 🗿 Giant | Slow but extremely strong |

---

### 🎒 Items & Resources

Collect and manage:

- 💰 Gold
- 🍖 Food
- 🧪 Health potions
- ✨ Magic rune scrolls
- 🏹 Arrows
- ⚔️ Weapons

Resource management is essential for survival.

---

## 🎮 Controls

| Key | Action |
|---|---|
| Arrow Keys / H J K L | Move player |
| E | Eat food |
| P | Drink potion |
| Z | Use magic rune |
| W | Change weapon |
| F | Fire ranged weapon |
| I | Open inventory |
| S | Save game |
| > | Descend stairs |
| ESC | Pause menu |

---

## 🖥️ User Interface

The game uses **ncurses** to provide:

- Color-based terminal graphics
- Game HUD
- Inventory screen
- Settings menu
- Help screen
- Scoreboard system
- Combat message log

---

## 🛠️ Technology Stack

| Technology | Usage |
|---|---|
| C11 | Main programming language |
| ncurses | Terminal graphics and UI |
| Makefile | Build system |
| Git | Version control |

---

## 📂 Project Structure

```
Rogue-Game/
│
├── source/
│   │
│   ├── include/
│   │   └── rogue.h
│   │
│   ├── src/
│   │   ├── main.c
│   │   ├── world.c
│   │   ├── ui.c
│   │   └── persistence.c
│   │
│   └── Makefile
│
├── docs/
│
└── README.md
```

---

## 🚀 Installation

### Requirements

- macOS or Linux
- C compiler
- ncurses library

### Install ncurses (macOS)

```bash
brew install ncurses
```

### Clone Repository

```bash
git clone git@github.com:NazilaAbedi/Rogue-Game.git
```

### Build

```bash
cd Rogue-Game/source
make
```

### Run

```bash
./rogue
```

---

## 🧠 Architecture

### main.c

Responsible for:

- Application flow
- Main menu
- Game loop
- User interaction

### world.c

Responsible for:

- Dungeon generation
- Player movement
- Combat system
- Enemy behavior
- Game mechanics

### ui.c

Responsible for:

- ncurses rendering
- Colors
- Menus
- HUD
- Screens

### persistence.c

Responsible for:

- User management
- Save/load system
- Player statistics

---

## 💾 Save System

Rogue Deluxe includes:

- User profiles
- Password validation
- Save and load functionality
- Score tracking
- Hall of Legends leaderboard

---

## 🏆 Objective

Your mission:

1. Explore the dungeon
2. Defeat enemies
3. Collect treasures
4. Reach the final floor
5. Escape alive

---

## 👩‍💻 Developer

**Nazila Abedi**

Programming Fundamentals Project

---

## 📜 License

This project was created as an educational project for a Programming Fundamentals course.
