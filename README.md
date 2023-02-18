# PSX_cyber_usb
PlayStation1, PlayStation2 コントローラを Raspberry Pi Picoを介して USB HID Joystickに変換するものです。  
特に、メガドライブ ミニ2に接続した場合に、電波新聞社製 XE1AJ-USB 相当の動作となることを意識しています。

# 材料
1. PlayStation1/2 コントローラ (SCPH-110, SCPH-10010など)
1. PlayStationコントローラ延長ケーブルなど [例](https://www.amazon.co.jp/third-party-PS1-2%E7%94%A8%E3%82%B3%E3%83%B3%E3%83%88%E3%83%AD%E3%83%BC%E3%83%A9%E3%83%BC%E5%BB%B6%E9%95%B7%E3%82%B1%E3%83%BC%E3%83%96%E3%83%AB/dp/B00C0NZWUI)
1. 1kΩ抵抗2本、配線
1. 3端子スライドスイッチ (6ボタンモードとの切り替えが必要な場合)
1. Raspberry Pi Pico および USBケーブル


# 準備
## Raspberry Pi Pico のファームウェア書き込み
1. Raspberry Pi Pico のBOOTSELボタンを押しながらPCにUSB接続し、`ps_cyber_usb.uf2`ファイルを書き込みます

## ハードウェア準備
1. 回路図にしたがって、PlayStaionコントローラ延長ケーブルと Raspberry Pi Pico を接続します

# 使い方
## 接続
1. PlayStation延長ケーブルに、PlayStaionコントローラを接続します
1. Raspberr Pi Pico を PC やメガドライブミニ2に接続します

## 操作
PlayStationコントローラのANALOGモード および回路中の MODE SWの設定によって、操作が変わります

### PlayStationコントローラ デジタルモード
#### MODE SW = GNDの場合 (メガドライブミニ2での利用に適したモード)

| PlayStation | USB-HID | メガドラ2|
|-------------|------| ---------------|
|DIGITAL UP | UP | UP|
|DIGITAL DOWN| DOWN |DOWN|
|DIGITAL LEFT | LEFT|LEFT|
|DIGITAL RIGHT| RIGHT|RIGHT|
|□ | Button1 | A|
|△| Button2 | B|
|○| Button3| C|
|×| Button2| B|
|L1| Button1| A|
|R1|Button2| B|
|L2| -  | -|
|R2|Button4| X|
|SELECT|Button7|MODE|
|START|Button8|START|


#### MODE SW = HIGH の場合 (6ボタンゲーム用モード)

| PlayStation | USB | メガドライブミニ2|
|-------------|------| ---------------|
|DIGITAL UP | UP | UP|
|DIGITAL DOWN| DOWN |DOWN|
|DIGITAL LEFT | LEFT|LEFT|
|DIGITAL RIGHT| RIGHT|RIGHT|
|□ | Button4 | X|
|△| Button5 | Y|
|○| Button2| B|
|×| Button1| A|
|L1| Button6| Z|
|R1|Button3| C|
|L2|Button3|C|
|R2|Button6|Z|
|SELECT|Button7|MODE|
|START|Button8|START|

### PlayStationコントローラ アナログモード
MODE SWの状態に関係なく、XE1AJ-USB相当の動作になります  
  (ただし、トリガボタンE1, E2の割り当てはありません)

| PlayStation | USB | XE1AJ-USB相当|
|-------------|------| ---------------|
|LEFT ANALOG UP/DOWN | Y-axis | Stick Y-axis|
|LEFT ANALOG LEFT/RIGHT| X-axis | Stick X-axis|
|RIGHT ANALOG UP/DOWN | Z-Rotation|Throttle|
|RIGHT ANALOG LEFT/RIGHT| Z-axis |-|
|□ | Button1 | A|
|△| Button2 | B|
|○| Button3| C|
|×| Button2| B|
|L1| Button1| A|
|R1|Button2| B|
|L2| -  | -|
|R2|Button4| D|
|SELECT|Button7|SELECT|
|START|Button8|START|

※ PlayStationコントローラの複数のボタンが、A,B ボタンに割り当てられているのは、
特にAfterBurner II での操作しやすさを考慮しているためです

# 非保証
- 本リポジトリ内のプログラム、回路図は、正常に動作することを期待して作成していますが、正常な動作を保証しません  
- 本リポジトリ内のプログラム・回路図を参照・利用したことにより生じた損害(メガドライブミニ/ミニ2が破損する、Raspberry Pi Picoが破損する、PlayStationコントローラが破損するなど)に対し、制作者は一切補償しません  
- 制作時は他の資料も参照し、回路図に誤りがないかどうか確認しながら行ってください

# 補足
## プログラムについて
このプログラムは、[Raspberry Pi Picoのサンプルプログラム](https://github.com/raspberrypi/pico-examples/tree/master/usb/device/dev_hid_composite)をベースに制作しています

コンパイルは、上記のサンプルプログラムと同様に行います。

## 回路について
PlayStationコントローラの電源を、Raspberry Pi Picoの3.3Vレギュレータ出力から取得しています  
レギュレータは1Aくらいの供給能力があると考えていますが、PlayStationコントローラの消費電流が不明、かつメガドライブミニ2のUSBの電流供給能力は不明です  
このため、電源周りの回路は大変怪しいのですが、
- 本回路は、市販のPSX-USB変換回路と大差ない(500mAで十分駆動できる)だろう
- メガドライブミニ2も1ポート500mAくらいは供給できるだろう

と考えて、問題ないのではと考えています.. (誤解しているかもしれません.. )
