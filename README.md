# Omega

Omega is a game written by Laurence Brothers in the late 1980s for Unix workstations. It derives principally from the tradition of earlier games such as Rogue and Hack, where the player explores a labyrinth of dungeons and monsters through the medium of an ASCII map display.

Omega is interesting as it was one of the first such games to show the influence of more story and quest led games, particularly the Ultima series that were released commercially by Origin Systems for various micros during the 1980s. Rather than simply wandering round a dungeon, in Omega the player has various paths open to them to appease the gods of Omega's world.

I've taken Omega 0.80.2 and built it as a "proper" Windows application (rather than the console mode builds that have been available before). This version can also optionally show the area around the player using graphics, rather than the usual ASCII characters. The graphics come from David Gervais' tiles, drawn originally for the game Angband.

![Omega in play](omega.png)

## Japanese localization

The `japanese-localization` branch adds a full Japanese translation
alongside the original English text.

**How it works:** every in-game message string was moved out of the C
source and into Windows resources, split by language:
`Strings.en.rc` (English) and `Strings.ja.rc` (Japanese), both `#include`d
from `WinOmega.rc` inside their own `LANGUAGE` block. Game code looks a
string up by ID through a small `LS(id)` helper (a `LoadStringA` wrapper),
which resolves against whichever `LANGUAGE` block matches the thread's
current locale -- the same mechanism Windows itself uses to serve the
right resource out of a multi-language binary. Plain-text files
(`help*.txt`, `motd.txt`) and the encrypted story/lore text
(`intro.txt`, `abyss.txt`, `scroll*.txt`) follow the same idea one level
up: a Japanese copy sits alongside the English original as `name.ja.txt`,
and `omegalibFile()` in `file.c` picks whichever one matches the current
language when a file is opened (the encrypted files are decrypted,
translated, and re-encrypted with the same rolling-XOR cipher the game
already uses, so no other code needed to change). `license.txt` is
deliberately left English-only, since it's the game's legal license text.

**Choosing a language:** the setup dialog shown at startup has a
**Language** dropdown -- *System default*, *English*, or *日本語*. Selecting
one calls `SetThreadLocale()`, which is what makes the resource lookup
above pick the matching `LANGUAGE` block; *System default* just leaves
the OS's own locale in charge (so the game already opens in Japanese on
a Japanese-locale Windows install without touching this setting). The
choice is saved to the registry and re-applied on the next launch.

**English is the fallback.** Anywhere a Japanese string, help file, or
resource dialog doesn't exist -- an untranslated leftover, a future
addition, whatever -- Windows' own resource-language negotiation falls
back to the English `LANGUAGE` block automatically, so nothing is ever
silently blank.

See `LOCALIZATION_PLAN.md` on that branch for the full implementation
notes and the remaining deferred work (item/spell name tables).

## Building

Download and install Visual Studio Community edition from https://visualstudio.microsoft.com/. In the installer, under "Workloads", make sure that "Desktop development with C++" is selected.

Install git. I use the version of git that is part of MSYS2, a Linux-like environment for Windows, but Git for Windows can be used from a Windows command prompt.

Open the environment that you are using git from, and switch to the root directory that the build environment will be created under. Clone this repository with git:
```
git clone https://github.com/DavidKinder/Omega.git
```
Start Visual Studio, open the solution "Omega.sln", then build and run the "Omega" project.
