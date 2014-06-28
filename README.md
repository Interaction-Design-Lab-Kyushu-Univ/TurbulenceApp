TurbulenceApp
=============

Turbulenceの投影側アプリです。Macでのみ動作確認しています。

**事前に用意するべきもの**

- openFrameworks 0.8.0

以下の2つのライブラリは、MacPortsを使うなどして自前でビルドしておいてください。

- Boost
- FluidSynth

```
sudp port install boost +universal
sudo port install fluidsynth +universal
```

以下の3つのアドオンをダウンロードしてoFのaddonsに入れてください。

- ofxLibwebsocket
- ofxDome
- ofxBox2d

最後の一点はファイル容量が大きすぎてGitHubに乗らなかったファイルです。
ネット上のどこかにあるので、ダウンロードしてbin/data/sound/の中に入れてください。

- FluidR3_GM.sf2

**ビルドの仕方**

ファイルを全部用意できたら、次の手順を実行してください。

1. このリポジトリの内容をoF0.8の2階層下のディレクトリ（myApps/Turbulence など）に入れる。
2. Xcodeを開いてBoostとFluidSynthに適切なパスを通す。
3. Buildを実行。

BoostとFluidSynthは何も考えずに作るとDLLになります。
そのままでは他の環境で動かないので、他の環境で実行するときは依存しているライブラリをappの中に入れる必要があります。
詳しくはググってください。

**アイテムの差し替え**

アイテムを差し替えるだけならビルドし直す必要はありません。
bin/data/settings.xmlを見てください。
\<flyingobjects>〜\</flyingobjects>の中身がアイテムです。
アイテムIDは上から順に0, 1, 2... と対応づけられます。

**設定項目**

- \<image> : 画像ファイルの名前
- \<sound> : アイテムが投げ入れられたときに鳴るwavファイルの名前でした。現在は使用されていません。
- \<hitsound> : 当たったときに鳴るwavファイルの名前でした。現在は使用されていません。
- \<radius> : 当たり判定（円）の半径です。単位はピクセルです。
- \<density> : アイテムの重さに対する係数です。デフォルトの重さは半径の2乗に比例します。
- \<channel> : アイテムが当たったときの音。いわゆるMIDIチャンネルです。チャンネルとプログラムナンバーの対応は\<synth>〜\</synth>の中に書かれています。
