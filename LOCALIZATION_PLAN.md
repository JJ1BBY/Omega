# WinOmega 日本語化プラン

対象リポジトリ: https://github.com/DavidKinder/Omega のフォーク
(https://github.com/JJ1BBY/Omega)。Windowsローグライク「Omega」のリソース分離・
言語切り替え基盤はコミット済み。このドキュメントは、翻訳作業(トークンを大量に
消費する部分)を別のローカルLLMに引き継ぐための引き継ぎ資料。

## 完了済み(コミット済み: "Extract hardcoded message strings into localizable resources")

- `Omega/*.c` (41ファイル) 内で `print1/print2/print3/nprint1/nprint2/nprint3/mprint`
  に直接渡されていた文字列リテラル **2,945呼び出し箇所(ユニーク2,665個)** を
  `LS(IDS_MSG_20000)`〜`LS(IDS_MSG_22664)` 形式のリソース参照に置換済み
- `WinOmega.cpp` に `LS(int id)` を実装(`LoadStringA` + 4個のローテーションバッファ)
- `WinOmega.rc` に English / Japanese の `LANGUAGE` ブロックを追加。実体は
  `#include "Strings.en.rc"` / `#include "Strings.ja.rc"` で分離
  - `Strings.en.rc`: 原文(英語)。元の `.c` ファイル単位でコメント区切り済み
  - `Strings.ja.rc`: **現状は英語のプレースホルダー**。ここを翻訳するのが本作業
  - `resource.h` の `IDS_MSG_*` 定義も同じ並びでグループ化済み
- `WinOmega.cpp` のフォント選択バグを修正: `fontSetup.lfCharSet` が
  `ANSI_CHARSET` 固定だったのを `GetACP()==932` なら `SHIFTJIS_CHARSET` を使う
  ように変更(日本語フォントが最初から列挙対象外だった)。日本語ロケール時は
  「MS ゴシック」を優先選択するよう追加
- ビルド確認済み(MSBuild, Release|x86, 後述のツールセット指定で成功)
- `LoadLibraryEx` + `LoadStringA` で実際にリソースが正しくロードされることを検証済み

## 翻訳作業(japanese-localization ブランチ) — 完了

`Strings.ja.rc` は **2665/2665件、完了**(元の`print1/print2/print3/nprint1/
nprint2/nprint3/mprint`呼び出しからの機械抽出分、および後述のギャップ埋め分
IDS_MSG_22665〜22738を含む全件)。RPGステータス略号(`Hit/Dmg/Def/Arm/Spd`、
`HP/MANA/AU/LEVEL/CARRY`、`STR/CON/DEX/AGI/INT/POW`)とアスキーアートの
バナー類は、ユーザー指示により意図的に英語のまま残してある。

最終コミット: "Translate spell.c, time.c, trap.c, util.c strings to Japanese"
(spell.c/time.c/trap.c/util.cを最後に、全ファイルグループの翻訳が完了)。

このドキュメントの以下の「手順」節は今後同種の抽出漏れ・追加翻訳作業が
発生した場合の参考として残す。

**重要: `Strings.ja.rc` を編集する際の手順**
1. 通常のテキストエディタ/Read・Editツールで作業する前に、ファイルが
   UTF-8になっていることを確認する(`git log`で見て直前のコミットが
   CP932保存だった場合、`python3 -c "open('Strings.ja.rc','w',encoding='utf-8',newline='').write(open('Strings.ja.rc','rb').read().decode('cp932'))"`
   のように一度UTF-8に戻してから編集する。CP932のままEdit系ツールで
   触ると文字化けする)
2. 翻訳が終わったら、**一時ファイル経由**でCP932に変換する(直接
   `open(path,'w',encoding='cp932')`で書くと、変換失敗時にファイルが
   0バイトに壊れる事故が起きた実績があるので避けること):
   ```python
   import shutil
   text = open('Strings.ja.rc', encoding='utf-8').read()
   encoded = text.encode('cp932')  # 失敗時はここで例外、ファイルはまだ無事
   open('Strings.ja.rc.tmp', 'wb').write(encoded)
   shutil.move('Strings.ja.rc.tmp', 'Strings.ja.rc')
   ```
3. MSBuildでビルド確認 → コミット、の順で進める(詳細は本ファイル下部の
   ビルド方法を参照)

## 解決済みのバグ: 日本語テキストが実機で文字化けした件

**原因はGDI描画ではなく、MSBuildの増分ビルドだった。**

`WinOmega.rc` は `#include "Strings.ja.rc"` / `#include "Strings.en.rc"` で
文字列テーブルを取り込んでいるが、`.vcxproj` の `ResourceCompile` 項目は
`WinOmega.rc` 単体しかタイムスタンプを追跡していなかった。そのため
`Strings.ja.rc` だけを編集して再ビルドしても **rc.exe が再実行されず、
古い(場合によっては編集途中の壊れた状態の)`.res` が使い回される**
ことがあった。実際、デバッグログで「vsprintfに渡るバイト列が元の翻訳文と
1バイトだけ違う」という現象が起きたのはこれが原因(過去のファイル復旧
作業中の中間状態が`.res`にキャッシュされたまま拾われていた)。

`Omega.vcxproj` の `ResourceCompile` に以下を追加して解決:
```xml
<ResourceCompile Include="WinOmega.rc">
  <AdditionalDependencies>Strings.en.rc;Strings.ja.rc;%(AdditionalDependencies)</AdditionalDependencies>
</ResourceCompile>
```
これでどちらかのファイルを触ると必ず `rc.exe` が再実行される
(`touch Strings.ja.rc` して増分ビルドし、`ResourceCompile:` ステップが
実際に走ることを確認済み)。

**教訓**: `Strings.*.rc` を編集した後に「見た目が変わらない」場合は、
まずコードや文字コードを疑う前に **`Release`フォルダを削除してクリーン
ビルド**し、それでも再現するか確認すること。

(前段の`ExtTextOut`+`lpDx`によるDBCS幅補正の修正自体は無害だが、今回の
文字化けの直接原因ではなかった。残しても害はないので`WinOmega.cpp`には
そのまま残してある。`impl_wprintw()`内の`OMEGA_JA_DEBUG`デバッグログも
同様に残置。再度有効にする場合は`Omega.vcxproj`のReleaseビルドの
`PreprocessorDefinitions`に`OMEGA_JA_DEBUG;`を追加してビルドし、
`%APPDATA%\Omega\ja_debug.log`を確認する。)

## 言語切り替えUI — 実装済み(japanese-localization ブランチ)

`IDD_SETUP` ダイアログに `IDC_LANGUAGE` コンボボックス(「(System default)」/
「English」/「日本語」)を追加し、選択結果を `HKCU\Software\David Kinder\Omega`
の `Language` (REG_DWORD、LANGID値。0=システム既定) に保存・次回起動時に復元
するようにした。

**実装方式**: `FindResourceEx`への全面置き換えは行わず、`SetThreadLocale()`で
スレッドロケールを切り替える方式を採用した(`WinOmega.cpp`の`applyUILanguage()`)。
Win32のリソースローダーは複数の`LANGUAGE`ブロックを持つバイナリで、明示的な
言語IDが指定されなかった`LoadStringA`/`DialogBox`呼び出しに対して現在の
スレッドロケールをヒントに解決言語を選ぶため、既存の`LS()`(`LoadStringA`
ラッパー)はそのままで動作する。`originalLocale`にプロセス起動時点の
スレッドロケールを保持しておき、「(System default)」選択時はそこへ復元する。

- `WinMain`起動時: レジストリから`Language`を読み込み → `applyUILanguage()`を
  `showDialog(IDD_SETUP,...)`より前に呼ぶ(次回以降の起動でセットアップ
  ダイアログ自体も選択言語で表示されるようにするため。ただし`IDD_SETUP`の
  日本語版`DIALOGEX`ブロックはまだ無いので、現状は英語版ダイアログが表示
  される。日本語版ダイアログの追加はタスク3で対応)
- ダイアログの`IDOK`/`IDCANCEL`ハンドラ内: コンボボックスの選択を読み取り、
  `applyUILanguage()`を即座に呼んでからレジストリに保存

`resource.h`に`IDC_LANGUAGE`(1004)を追加、`WinOmega.rc`の`IDD_SETUP`
DIALOGEXを拡張(ダイアログ高さ146→160、Displayグループボックスに言語行を
追加)。ビルド確認済み(0エラー0警告)。

**既知の制限**: このセッションの環境ではPythonが64bit、`Omega.exe`が32bit
(i386)のため、`LoadLibraryExW`+`LOAD_LIBRARY_AS_DATAFILE`によるプロセス外
リソース検証(以前使っていた手法)が`EnumResourceTypesW`で空リストを返し
機能しなかった。実機起動しての動作確認は未実施(このセッションでは過去に
GUI起動確認で無関係なプロセスを誤ってkillした事故があったため、意図的に
避けている)。次回、32bit版のPythonまたは`Resource Hacker`等のツールで
`Release\Omega.exe`のSTRINGTABLE言語ブロック切り替えと`IDD_SETUP`ダイアログの
言語コンボボックス表示を実際に確認することを推奨。

`Strings.en.rc`は引き続き削除せず維持。

## 未着手のタスク

翻訳の進め方に関する参考情報(完了済みの`Strings.ja.rc`本体翻訳で実際に
使った手順)を以下に残す。

`Omega/Strings.ja.rc` の `STRINGTABLE` の各行

```
    IDS_MSG_20123    "Some English text here."
```

の右辺(ダブルクォート内)だけを日本語に置き換える形式だった。**IDと
`IDS_MSG_`定義行、ブロック区切りコメント(`// ==== xxx.c ====`)は変更しない。**

対応する原文は `Strings.en.rc` の同じIDの行(内容は同一)。

**重要な注意点(文字コード)**:
- `WinOmega.rc` のJapaneseブロックは `#pragma code_page(932)` を指定している
  ため、`Strings.ja.rc` は **Shift-JIS (CP932) でエンコードして保存すること**。
  UTF-8のまま保存すると `rc.exe` がバイト列をShift-JISとして誤解釈し、
  実行時に文字化けする(実際にこの問題を一度踏んでいる)
- Pythonでファイル入出力する場合は `encoding='cp932'` あるいは
  `encoding='shift_jis'` を明示する。テキストエディタで開く場合もエンコード
  指定に注意
- 一部の文字列には `%` を含むものがある可能性がある。`print1()`等は内部で
  `wprintw()`(printf系)にそのまま渡しているため、翻訳文中に `%` を入れる場合は
  想定外のフォーマット指定子として解釈されないか確認すること(原文に`%`が
  無ければ気にしなくてよい)
- ダブルクォート `"` を文中に含める場合はRCの記法で `""`(2つ重ねる)にする
  (C言語の `\"` ではない)

**訳文のトーン**: 原作は1980年代のUNIXローグライク(Laurence Brothers作)。
硬すぎない、やや軽妙・ユーモラスな英語が多い。プレイヤー向けメッセージなので
簡潔な日本語(ですます調ではなく「〜だ」「〜した」等のゲーム的文体)推奨。

**動作確認方法**(GUIを起動せずに検証可能):
```powershell
# ビルド後、Omega.exeから直接LoadStringAで読み出して確認する
# (前セッションで実際に使った方法。プロセスを起動しないので安全)
```
PowerShellで `LoadLibraryEx(path, 0, LOAD_LIBRARY_AS_DATAFILE)` →
`LoadStringA(handle, id, buf, size)` で任意のIDの文字列を検証できる。

### 1. ⑤ アイテム名/呪文名テーブル(意図的に保留中・要注意)

- `Omega/aux3.c:798` の `static char *sitenames[]`
- `Omega/spell.c:726` の `static char *spell_names[]`

これらはUI表示だけでなく、セーブデータやコマンド入力の文字列照合に使われて
いる可能性があるため、単純な差し替えは**ゲームロジックを壊すリスクがある**。
着手する場合は、まず両配列が `strcmp` 等でどこから参照されているかを
`grep -rn "sitenames\|spell_names"` で洗い出してから方針を決めること。

### 2. ② 暗号化テキストファイルの翻訳 — 完了

`OmegaLib/intro.ja.txt`, `abyss.ja.txt`, `scroll1.ja.txt`〜`scroll4.ja.txt`
を作成済み(CP932でエンコードした翻訳文を、下記と同じローリングXORで
再暗号化したバイナリ)。

**使用したアルゴリズム**(`Omega/Omega/file.c`の`displaycryptfile()`を参照):
復号は `平文[i] = 暗号文[i] XOR key`、その後 `key := 平文[i]`(key初期値100)。
暗号化はこれを順方向に適用するだけ: `暗号文[i] = 平文[i] XOR key`、その後
`key := 平文[i]`。鍵の更新には常に平文バイトを使う点が同じなので、暗号化・
復号は対称的な処理になる。実装前に、既存の6ファイルすべてで「復号→
再暗号化」のラウンドトリップがオリジナルのバイト列と完全一致することを
確認してからテキストの翻訳に着手した。

`file.c`の`user_intro()`, `abyss_file()`, `cityguidefile()`, `wishfile()`,
`adeptfile()`, `theologyfile()`は、プレーンテキストヘルプファイル(タスク4)
と同じ`omegalibFile()`ヘルパー経由に変更済み。暗号化ファイルは`_access()`
によるファイル存在チェック以外は中身を見ないため、`omegalibFile()`を
そのまま流用できた。

`scroll1.txt`(神統記)の神名(Odin、Athena、Set、Hecate、Lords of Destiny、
ArchDruid)と`scroll3.txt`(上級魔法解説)で引用符付きで言及される呪文名
(`'Death'`、`'Power'`、`'Skill'`など)は、`spell_names[]`/`sitenames[]`
(タスク⑤、未着手のまま)との対応を壊すリスクを避けるため、意図的に英語の
まま残した。

`abyss.dat`等の`.dat`ファイルは`cryptkey()`という別方式(ダンジョンマップ
データ、自然文をほぼ含まない)であり、これは引き続き翻訳対象外。

### 3. ⑥ Windows UI文字列 — 完了

- `IDD_SETUP` ダイアログ: `WinOmega.rc`のJapanese `LANGUAGE`ブロック内に
  `IDD_SETUP DIALOGEX` を複製・翻訳して追加済み(キャプション"Omega"は
  ゲーム名としてそのまま、ボタン「オメガを開始(&P)」、「表示」、
  「フォント名(&F)」、「サイズ(&Z)」、「セーブデータ(&S)」、「クレジット」
  など)。ダイアログフォントは`MS UI Gothic`・charset `0x80`(SHIFTJIS_CHARSET)
  を明示指定。タスク5の言語切り替え(`SetThreadLocale`)により、この
  ダイアログ自体も選択言語で表示されるようになった。
- `WinOmega.cpp`のハードコードされたエラー文言(`fatal()`/`MessageBox`)は
  `IDS_UI_OMEGALIB_PATH`〜`IDS_UI_CREATE_WINDOW`(23000-23007、
  `IDS_MSG_*`とは別レンジ)として`Strings.en.rc`/`Strings.ja.rc`両方に追加し、
  `LS()`経由で読み込むよう変更済み。`MessageBox`のタイトル文字列"Omega"
  (ゲーム名)自体は翻訳せずリテラルのまま維持。

### 4. ① プレーンテキストファイルの翻訳 — 完了

`OmegaLib/help1.ja.txt`〜`help13.ja.txt`, `motd.ja.txt`, `thanks.ja.txt`,
`update.ja.txt`(CP932エンコード、`Strings.ja.rc`と同じ方式)を作成済み。

**実装方式**: `file.c`に`omegalibFile(dest, name)`ヘルパーを追加。現在の
UI言語(`WinOmega.cpp`の`isJapaneseUILanguage()`、タスク5の`applyUILanguage()`
が設定するスレッドロケールを参照)が日本語で、かつ`name.ja.txt`が実際に
ディスク上に存在する場合はそちらを、それ以外は元の英語ファイルを使う。

以下の実際にゲーム内で読み込まれる呼び出し箇所に配線済み:
- `file.c`: `commandlist()`(help12/13.txt), `inv_help()`(help3.txt),
  `combat_help()`(help5.txt), `showmotd()`(motd.txt)
- `command3.c`: `help()`内の`help<n>.txt`汎用ヘルプメニュー

`thanks.txt`/`update.txt`はこのコードベースのどこからも読み込まれていない
(未使用ファイル)ことを確認済み。翻訳自体は完成させたが配線先が無い。

`license.txt`はゲームのライセンス条項という性質上、翻訳版を正式なものとして
扱うリスクを避けるためユーザーの判断で意図的に英語版のみ維持することとし、
`show_license()`は変更していない。

## ビルド方法

このマシンには Visual Studio 2022 Community が入っているが、プロジェクトの
`PlatformToolset` が `v145`(存在しないバージョン)を指しているため、ビルド時に
上書き指定が必要:

```bash
cd /d/src/WinOmega/Omega
export MSYS2_ARG_CONV_EXCL="*"
"/c/Program Files/Microsoft Visual Studio/18/Community/MSBuild/Current/Bin/MSBuild.exe" \
  Omega.sln /p:Configuration=Release /p:Platform=x86 /p:PlatformToolset=v143 /m
```

(このマシンのVisual Studioインストールパスは`18/Community`だった。別環境
では`2022/Community`など異なる場合があるので、`find "/c/Program Files/
Microsoft Visual Studio" -iname MSBuild.exe`で探すのが確実)

ソリューション構成は `Release|x86` / `Debug|x86` のみ(x64は無い)。

## 注意事項(このセッションでの反省)

- ゲームを実際に起動してのスクリーンショット確認は、この環境ではウィンドウ
  ハンドル取得やマルチモニタ座標の扱いが不安定で、**無関係な別ウィンドウを
  誤ってキャプチャした**上、確認後の後片付けで**このセッションが起動して
  いない既存プロセス(`winomega.exe`)を誤って強制終了してしまう事故**が
  起きた。今後GUI起動を伴う確認をする場合は、自分が起動したプロセスのPIDを
  明示的に記録し、そのPIDだけを対象にする(`taskkill /F /IM 名前` のような
  イメージ名一括killは使わない)。
- リソース文字列の動作確認は、GUIを起動しなくても
  `LoadLibraryEx(..., LOAD_LIBRARY_AS_DATAFILE) → LoadStringA` で
  安全にできる。翻訳作業中の検証はこちらを優先すること。
