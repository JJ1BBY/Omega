# Omega

Omega is a game written by Laurence Brothers in the late 1980s for Unix workstations. It derives principally from the tradition of earlier games such as Rogue and Hack, where the player explores a labyrinth of dungeons and monsters through the medium of an ASCII map display.

Omega is interesting as it was one of the first such games to show the influence of more story and quest led games, particularly the Ultima series that were released commercially by Origin Systems for various micros during the 1980s. Rather than simply wandering round a dungeon, in Omega the player has various paths open to them to appease the gods of Omega's world.

I've taken Omega 0.80.2 and built it as a "proper" Windows application (rather than the console mode builds that have been available before). This version can also optionally show the area around the player using graphics, rather than the usual ASCII characters. The graphics come from David Gervais' tiles, drawn originally for the game Angband.

![Omega in play](omega.png)

## About this fork

This is a fork of [DavidKinder/Omega](https://github.com/DavidKinder/Omega)
(the original Windows port described above). The `japanese-localization`
branch adds, on top of that: a full Japanese translation with a runtime
language switcher (see below), and **gamepad support (experimental)**
(movement, confirm/cancel, and a hold-to-run modifier, with an in-game
configuration screen reachable from the system menu -- useful since the
game's controller mapping isn't standardized the way a keyboard's is).
`master` on this fork tracks upstream unchanged; all of the above lives
only on `japanese-localization`.

## Japanese localization

The `japanese-localization` branch adds a full Japanese translation
alongside the original English text.

**How it works:** every in-game message string was moved out of the C
source and into Windows resources, split by language:
`Strings.en.rc` (English) and `Strings.ja.rc` (Japanese), both `#include`d
from `WinOmega.rc` inside their own `LANGUAGE` block. Game code looks a
string up by ID through a small `LS(id)` helper (a `LoadStringA` wrapper),
which resolves against whichever `LANGUAGE` block matches the thread's
current UI language (`SetThreadUILanguage`) -- the same mechanism Windows
itself uses to serve the right resource out of a multi-language binary.
Plain-text files (`help*.txt`, `motd.txt`) and the encrypted story/lore text
(`intro.txt`, `abyss.txt`, `scroll*.txt`) follow the same idea one level
up: a Japanese copy sits alongside the English original as `name.ja.txt`,
and `omegalibFile()` in `file.c` picks whichever one matches the current
language when a file is opened (the encrypted files are decrypted,
translated, and re-encrypted with the same rolling-XOR cipher the game
already uses, so no other code needed to change). `license.txt` is
deliberately left English-only, since it's the game's legal license text.

**Choosing a language:** the setup dialog shown at startup has a
**Language** dropdown -- *System default*, *English*, or *日本語* -- next to
a **Remember this selection** checkbox. Selecting a language calls
`SetThreadUILanguage()`, which is what makes the resource lookup above
pick the matching `LANGUAGE` block; *System default* leaves the OS's own
UI language in charge instead (so on a Japanese-locale Windows install,
picking *System default* opens the game in Japanese). With the checkbox
checked, the choice is saved to the registry and re-applied on the next
launch; leaving it unchecked keeps the change for the current session
only. On a fresh install, before anything has been saved, the game
defaults to *English* rather than following the OS locale.

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
git clone https://github.com/JJ1BBY/Omega.git
```
This fork's default branch is `master`, tracking upstream unchanged; check out `japanese-localization` for the Japanese translation and gamepad support described above:
```
git checkout japanese-localization
```
Start Visual Studio, open the solution "Omega.sln", then build and run the "Omega" project.

---

# Omega(日本語)

Omegaは1980年代後半にLaurence BrothersがUnixワークステーション向けに書いたゲームです。RogueやHackといった初期のゲームの流れを汲んでおり、プレイヤーはASCIIマップ表示を通してダンジョンとモンスターの迷宮を探索します。

Omegaが興味深いのは、より物語性・クエスト性の強いゲーム――特に1980年代にOrigin Systemsが各種パソコン向けに商業リリースしたUltimaシリーズ――の影響を受けた初期のゲームの一つである点です。単にダンジョンを歩き回るだけでなく、Omegaの世界の神々の機嫌を取るための様々な道がプレイヤーには開かれています。

Omega 0.80.2を元に、(従来からあったコンソールモード版ではなく)「まともな」Windowsアプリケーションとしてビルドしました。このバージョンでは、通常のASCII文字の代わりにグラフィックでプレイヤー周辺のエリアを表示するオプションも選べます。グラフィックはAngband向けに描かれたDavid Gervaisのタイルを使用しています。

![Omega in play](omega.png)

## このフォークについて

これは[DavidKinder/Omega](https://github.com/DavidKinder/Omega)(上記の元のWindows移植版)のフォークです。`japanese-localization`ブランチでは、それに加えて: 実行時に切り替え可能な言語スイッチャー付きの完全な日本語翻訳(下記参照)と、**ゲームパッド対応(実験的)**(移動、決定/キャンセル、押しっぱなしでダッシュする修飾操作。システムメニューから開けるゲーム内設定画面付き――ゲームパッドのボタン配置はキーボードほど標準化されていないため便利です)を追加しています。このフォークの`master`はupstreamを変更なしで追従しており、上記の内容は全て`japanese-localization`ブランチにのみ存在します。

## 日本語ローカライズ

`japanese-localization`ブランチでは、オリジナルの英語テキストと並行して完全な日本語翻訳を追加しています。

**仕組み:** ゲーム内の全てのメッセージ文字列をCソースコードから切り出し、言語ごとに分けたWindowsリソースに移しました: `Strings.en.rc`(英語)と`Strings.ja.rc`(日本語)で、どちらも`WinOmega.rc`からそれぞれ専用の`LANGUAGE`ブロック内で`#include`されています。ゲームコードは`LS(id)`という小さなヘルパー(`LoadStringA`のラッパー)を通じてIDで文字列を引き、スレッドの現在のUI言語(`SetThreadUILanguage`)に一致する`LANGUAGE`ブロックが解決されます――これはWindows自身が多言語バイナリから適切なリソースを提供する際に使う仕組みと同じです。プレーンテキストファイル(`help*.txt`、`motd.txt`)と暗号化されたストーリー/ロアテキスト(`intro.txt`、`abyss.txt`、`scroll*.txt`)も同じ考え方をもう一段上のレベルで踏襲しています: 日本語版が`name.ja.txt`としてオリジナルの英語版と並んで置かれ、`file.c`の`omegalibFile()`がファイルを開く際に現在の言語に一致する方を選びます(暗号化ファイルは復号・翻訳の上、ゲームが元々使っているのと同じローリングXOR暗号で再暗号化しているため、他のコードを変更する必要はありませんでした)。`license.txt`はゲームの法的ライセンス文書であるため、意図的に英語のままにしています。

**言語の選択:** 起動時に表示される設定ダイアログには**言語**ドロップダウン(*System default*、*English*、*日本語*)と**この選択を記憶する**チェックボックスがあります。言語を選択すると`SetThreadUILanguage()`が呼ばれ、これが上記のリソース検索でどの`LANGUAGE`ブロックが選ばれるかを決めます。*System default*を選ぶとOS自体のUI言語に従います(そのため日本語ロケールのWindowsでは、*System default*を選ぶとゲームは日本語で起動します)。チェックボックスがオンの場合、選択内容はレジストリに保存され次回起動時にも適用されます。オフのままだと、その変更は今回のセッション限りになります。何も保存されていない新規インストール時は、OSロケールに関わらずデフォルトで*English*になります。

**英語はフォールバックです。** 日本語の文字列・ヘルプファイル・リソースダイアログが存在しない箇所(未翻訳の取りこぼし、将来の追加分など)では、Windows自体のリソース言語ネゴシエーションが自動的に英語の`LANGUAGE`ブロックにフォールバックするため、何も表示されず空白になることはありません。

実装の詳細と残タスク(アイテム/呪文名テーブルなど)については、そのブランチの`LOCALIZATION_PLAN.md`を参照してください。

## ビルド方法

https://visualstudio.microsoft.com/ からVisual Studio Community editionをダウンロード・インストールしてください。インストーラーの「ワークロード」で「C++によるデスクトップ開発」が選択されていることを確認してください。

gitをインストールしてください。私はMSYS2(Windows向けのLinuxライクな環境)に含まれるgitを使っていますが、Windowsのコマンドプロンプトから使えるGit for Windowsでも構いません。

gitを使う環境を開き、ビルド環境を作成するルートディレクトリに移動します。gitでこのリポジトリをクローンします:
```
git clone https://github.com/JJ1BBY/Omega.git
```
このフォークの既定ブランチは`master`で、upstreamを変更なしで追従しています。上記の日本語翻訳とゲームパッド対応が入った`japanese-localization`をチェックアウトしてください:
```
git checkout japanese-localization
```
Visual Studioを起動し、ソリューション"Omega.sln"を開いて、"Omega"プロジェクトをビルド・実行してください。
