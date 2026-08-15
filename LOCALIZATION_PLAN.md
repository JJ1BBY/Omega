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

## 未着手のタスク

### 1. 本命: `Strings.ja.rc` の翻訳(2,665件)

`Omega/Strings.ja.rc` を開き、`STRINGTABLE` の各行

```
    IDS_MSG_20123    "Some English text here."
```

の右辺(ダブルクォート内)だけを日本語に置き換える。**IDと`IDS_MSG_`定義行、
ブロック区切りコメント(`// ==== xxx.c ====`)は変更しない。**

対応する原文は `Strings.en.rc` の同じIDの行(内容は同一なので、`Strings.en.rc`
を見ながら`Strings.ja.rc`だけ書き換えれば良い)。

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

### 2. ⑤ アイテム名/呪文名テーブル(意図的に保留中・要注意)

- `Omega/aux3.c:798` の `static char *sitenames[]`
- `Omega/spell.c:726` の `static char *spell_names[]`

これらはUI表示だけでなく、セーブデータやコマンド入力の文字列照合に使われて
いる可能性があるため、単純な差し替えは**ゲームロジックを壊すリスクがある**。
着手する場合は、まず両配列が `strcmp` 等でどこから参照されているかを
`grep -rn "sitenames\|spell_names"` で洗い出してから方針を決めること。

### 3. ② 暗号化テキストファイルの翻訳

対象: `OmegaLib/intro.txt`, `abyss.txt`, `scroll1.txt`〜`scroll4.txt`
(`file.c` の `displaycryptfile()` が読む)

暗号方式(ローリングXOR、`Omega/Omega/file.c:595-626` に実装):
```
key = 100 (初期値)
各バイト c について:
  key = c XOR key   ← これが復号後の平文バイト
  出力(表示)する
```
つまり `平文[i] = 暗号文[i] XOR key[i-1]`、`key[0]=100`、
`key[i] = 平文[i]`(= 直前の平文バイトが次のXOR鍵になる)。

復号・翻訳・再暗号化の流れ:
1. 上記アルゴリズムでファイルをバイト列として復号 → 平文を取得
2. 平文を日本語に翻訳(全角文字を含む場合、桁ズレやページ送り
   `if (y > (LINES-6))` の判定に影響する可能性は低い想定だが、
   `intro.txt`等は見た目重視の演出テキストなので実際に表示して確認推奨)
3. 同じアルゴリズムで**逆方向に**再暗号化: `key`を100から開始し、
   `暗号文[i] = 平文[i] XOR key`、その後 `key = 平文[i]` に更新、を
   平文の先頭から順に適用(暗号化・復号で全く同じ関数を使い回せる実装が
   多いので、`displaycryptfile`のロジックを流用したCLIツールを別途書くのが早い)
4. 既存の `abyss.dat` 等の `.dat` ファイルは `cryptkey()` という別方式
   (ダンジョンマップデータ、自然文はほぼ含まれない)なので翻訳対象外

同じ暗号を使う `abyss_file()`, `cityguidefile()` 等の呼び出し元は
`Omega/Omega/file.c` 内で `grep -n displaycryptfile` すれば一覧できる。

### 4. ⑥ Windows UI文字列

- `IDD_SETUP` ダイアログ(`WinOmega.rc:71-93`)が **Englishブロックにしか
  存在しない**。日本語版ダイアログを出すには、Japaneseブロック内に
  `IDD_SETUP DIALOGEX` を複製して文言を訳す必要がある(キャプション
  "Omega"、ボタン "&Play Omega"、"Display"、"&Font name"、"Font si&ze"、
  "&Saved games"、"Credits" など)
- `WinOmega.cpp` 内のハードコードされたエラー文言(`fatal()`/`MessageBox`):
  - L389: `MessageBox(0,msg,"Omega",...)` (msg自体は動的生成)
  - L632, 638, 642, 695, 753, 772, 812: `fatal("...")` 各種
  - L778: `MessageBox(0,"Graphics file not loaded.","Omega",...)`

  これらは`①⑤`と違いプレイヤーが頻繁に見るテキストではない(異常系のみ)ため
  優先度は低い。対応する場合は `IDS_UI_*` のような別レンジのIDを新設し、
  `LS()`経由に置き換えるのが自然(`IDS_MSG_*`のレンジ20000-22664とは別に、
  例えば23000番台を使うなど)。

### 5. ① プレーンテキストファイルの翻訳

`OmegaLib/help1.txt`〜`help13.txt`, `license.txt`, `thanks.txt`, `update.txt`,
`motd.txt` は暗号化なしの生テキスト。日本語版を `OmegaLib/help1.ja.txt` の
ように別名で用意し、`Omega/Omega/file.c` 側でOS言語判定して読み分ける仕組みが
必要(現状は決め打ちのファイル名を`strcat`で組み立てているだけなので、
`GetUserDefaultLangID()` 等で分岐する処理を追加する)。

## ビルド方法

このマシンには Visual Studio 2022 Community が入っているが、プロジェクトの
`PlatformToolset` が `v145`(存在しないバージョン)を指しているため、ビルド時に
上書き指定が必要:

```bash
cd /d/src/WinOmega/Omega
export MSYS2_ARG_CONV_EXCL="*"
"/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/amd64/MSBuild.exe" \
  Omega.sln /p:Configuration=Release /p:Platform=x86 /p:PlatformToolset=v143 /m
```

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
