باشه، این نسخه‌ی README حرفه‌ای‌تره. اینو به عنوان `README.md` جایگزین کن:

```markdown
# 🗡️ ROGUE DELUXE

## A C11 Terminal Dungeon Adventure

<p align="center">

A procedural roguelike experience built with **C11** and **ncurses**.

Explore unknown dungeons, fight deadly creatures, collect ancient treasures,  
and survive long enough to reach the final floor.

</p>

---

## 🌌 Game Overview

You are an explorer trapped inside a mysterious underground world.

Every dungeon is different.

Every decision matters.

Fight monsters, manage your resources, discover powerful weapons, and descend deeper into the unknown.

Only the strongest adventurers reach the exit.

---

# ✨ Features

## 🏰 Procedural Dungeon Generation

- Randomly generated dungeon floors
- Dynamic rooms and corridors
- Doors, traps, treasures and hidden paths
- Fog of war and visibility system

---

## ⚔️ Combat System

Turn-based combat with:

- Multiple weapons
- Melee and ranged attacks
- Enemy encounters
- Damage calculation
- Experience and leveling system

Available weapons:

| Weapon | Type |
|---|---|
| Mace | Heavy damage |
| Dagger | Fast attacks |
| Sword | Balanced |
| Bow | Ranged |
| Wand | Magic |

---

## 👹 Enemies

Encounter different creatures:

| Enemy | Description |
|-|-|
| 🐍 Snake | Fast and dangerous |
| ☠️ Undead | Dark dungeon creature |
| 🔥 Fire Beast | High damage enemy |
| 👹 Demon | Powerful magical enemy |
| 🗿 Giant | Slow but extremely strong |

---

## 🎒 Items & Resources

Collect and manage:

- 💰 Gold
- 🍖 Food
- 🧪 Potions
- ✨ Magic runes
- 🏹 Arrows
- ⚔️ Weapons

Your survival depends on smart resource management.

---

# 🎮 Controls

| Key | Action |
|-|-|
| Arrow Keys / HJKL | Move |
| E | Eat food |
| P | Drink potion |
| Z | Use rune |
| W | Change weapon |
| F | Fire ranged weapon |
| I | Open inventory |
| S | Save game |
| > | Descend stairs |
| ESC | Pause menu |

---

# 🖥️ Interface

Built with:

- ncurses color rendering
- Dynamic HUD
- Combat messages
- Inventory screen
- Settings menu
- Scoreboard system

---

# 🛠️ Technology Stack

| Technology | Usage |
|-|-|
| C11 | Main programming language |
| ncurses | Terminal graphics |
| Makefile | Build system |
| Git | Version control |

---

# 📂 Project Structure

```

Rogue-Game/

├── source/
│
├── include/
│   └── rogue.h
│
├── src/
│   ├── main.c
│   ├── world.c
│   ├── ui.c
│   └── persistence.c
│
├── docs/
│
└── README.md

````

---

# 🚀 Installation

## Requirements

- macOS / Linux
- C compiler
- ncurses library


Install ncurses on macOS:

```bash
brew install ncurses
````

Clone the repository:

```bash
git clone git@github.com:NazilaAbedi/Rogue-Game.git
```

Build:

```bash
cd Rogue-Game/source

make
```

Run:

```bash
./rogue
```

---

# 🧠 Architecture

## main.c

Responsible for:

* Program flow
* Main menu
* Game loop
* User interaction

## world.c

Responsible for:

* Dungeon generation
* Player movement
* Combat
* Enemy behavior
* Game mechanics

## ui.c

Responsible for:

* ncurses rendering
* Colors
* Menus
* HUD
* Screens

## persistence.c

Responsible for:

* User accounts
* Save/load system
* Player statistics

---

# 💾 Save System

The game supports:

* User profiles
* Password validation
* Save files
* Score tracking
* Hall of Legends

---

# 🏆 Objective

Reach the final dungeon floor.

Defeat enemies.

Collect treasures.

Escape alive.

---

# 👩‍💻 Developer

**Nazila Abedi**

Programming Fundamentals Project

---

