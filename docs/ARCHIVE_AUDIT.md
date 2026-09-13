# Audit of the uploaded archive

## What was in the archive

The archive contains multiple copies/stages of the same Rogue coursework project, an embedded Git repository, the Phase 1 specification PDF, an unrelated one-page pointer exercise PDF (`e3.pdf`), macOS metadata, and a `pico.save` file.

## Why the previous source was not a safe base

The latest loose source was structurally inconsistent and could not be treated as one compilable program. Examples:

- More than one source file defines `main()`.
- `main.c` includes `../include/rogue.h` even though the uploaded top-level layout has `rogue.h` beside it.
- Both `creatLevel` and `createLevel` are used; declarations and definitions disagree.
- Both `moveMonstters` and `moveMonsters` appear.
- The header has an unterminated declaration (`killMonster`) and incomplete/invalid struct members.
- `Door.position` is declared as a pointer in one version while room code treats it as an embedded struct in another.
- Several functions are used without matching declarations, and some declared functions never exist.
- The old login parser reads `username,password,email` but compares the entire tail (`password,email`) to the entered password.
- Random password generation did not reliably NUL-terminate the generated string.
- Several allocations and movement paths could leak memory or use invalid/uninitialized values.
- There are duplicate snapshots in different folders with different hashes, so simply “fixing the latest file” would discard other work.

Because the course document explicitly values extensibility/clean implementation, the safest solution was to rebuild the game around one coherent state model and module boundary rather than preserve incompatible APIs.

## Requirement coverage used for the rebuild

From the Phase 1 specification, the rebuild covers the requested C + ncurses approach and the major functional areas: graphical menus, account creation/login/guest play, settings, scoreboard, game messages, procedural rooms/corridors, doors, walls, pillars/windows, exploration visibility, stairs, movement, food, gold, weapons/spells, save/continue, and game completion.

The rebuild also implements the combat/enemy half that the Phase 1 document says is intended for the next phase: multiple monster types, pursuit behavior, melee/ranged combat, health/damage progression and a complete multi-floor win/lose loop.

## Verification performed

- C11 syntax check with `-Wall -Wextra -Wpedantic`: no warnings after cleanup.
- 250 generated floors tested for a traversable route from the player spawn to the staircase.
- Monster spawn tiles validated as traversable.
- Password and email validation smoke-tested.
- Save structures include magic/version fields so incompatible save files are rejected instead of being blindly loaded.

The execution environment used for this reconstruction had the ncurses runtime library but not its development headers, so a full ncurses link was not possible there. The source was syntax-checked against the ncurses API surface and the game-logic/persistence modules were compiled and executed in tests. On a normal development system with `libncurses-dev` (Linux) or curses development headers (macOS), `make` is the intended build path.
