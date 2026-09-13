# Rogue Deluxe - rebuilt FOP project

A clean-room rebuild of the uploaded Fundamentals of Programming Rogue project in **C11 + ncurses**. It keeps the course constraint (terminal graphics) while turning the unfinished prototype into a coherent playable roguelike.

## Build

Ubuntu/Debian:
```bash
sudo apt install build-essential libncurses-dev
make
./rogue
```

macOS (Homebrew ncurses is usually already available to clang through the system curses compatibility layer):
```bash
make
./rogue
```

Recommended terminal size: **98x31 or larger**.

## Features

- Polished ncurses menus, color UI, HUD, message log and inventory
- Account registration/login + guest mode
- Password/email validation and local account database
- Persistent settings (difficulty and hero color)
- Scoreboard with top-player styling and current-user highlighting
- Binary save/continue with version checking
- Procedurally generated connected rooms and winding corridors
- Fog of war + line-of-sight exploration
- Themed rooms, doors, pillars, windows, traps and stairs
- Food, gold, healing potions, rune scrolls, weapons and arrows
- Mace, dagger, sword, bow and magic wand
- Melee and ranged combat
- Snake, undead, fire beast, demon and giant enemies
- Enemy aggro / pursuit AI
- Hunger, HP, XP, score, buffs (shield/power/haste)
- Four-floor run with a real victory state

## Controls

- Move: arrow keys or `H J K L`; diagonals `Y U B N`
- `E`: eat food
- `P`: drink potion
- `Z`: use rune scroll
- `W`: cycle owned weapon
- `F` then a direction: fire bow/wand
- `>`: use stairs when standing on them
- `I`: inventory
- `S`: quick save
- `?`: help
- `Esc`: pause menu

## Project structure

- `include/rogue.h` - all game data types and module interfaces
- `src/world.c` - generation, movement, combat, AI, items, visibility
- `src/ui.c` - all ncurses rendering and screens
- `src/persistence.c` - users, settings, save files and scoreboard data
- `src/main.c` - application/menu/game-loop orchestration

The old archive contained multiple incomplete source copies with conflicting APIs and duplicate `main()` functions. This rebuilt version intentionally uses one consistent architecture rather than trying to preserve those defects.
