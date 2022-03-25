# ATOM Babies: 二酸化炭素濃度チェック

<a href="https://gyazo.com/78703d4c5b78e80e5e16b9ceb2aba170"><img src="https://i.gyazo.com/78703d4c5b78e80e5e16b9ceb2aba170.jpg" alt="Image from Gyazo" width="640"/></a>

[M5StickC Proto Hat](https://shop.m5stack.com/products/m5stickc-proto-hat) に入る [M5StickC Proto Hat 向け CO<sub>2</sub> センサボード](https://mktechlab.net/co2-sensor-board-scd4x/)を[ATOM Babies](https://github.com/3110/atom-babies-arduino) に接続して，二酸化炭素濃度をチェックしてもらいます。

## 使用方法

### 二酸化炭素濃度の測定

ATOM Babies を電源に接続すると，センサーで測定した二酸化炭素（CO<sub>2</sub>）濃度に応じて表情が変わります。目が赤くなったら換気してあげてくださいね。

<a href="https://gyazo.com/5952d07fde8e08046e9b6952194bbf80"><img src="https://i.gyazo.com/5952d07fde8e08046e9b6952194bbf80.png" alt="Image from Gyazo" width="640"/></a>

### 顔の向きの自動設定

ATOM Babies の向きの応じて，顔の向きが変わるようになっています。

### 二酸化炭素濃度の表示

ATOM Babies の顔が表示されている状態で顔を押すと，現時点で計測されている二酸化炭素濃度の値をスクロールして表示します。

### 強制校正モード

ATOM Babies の顔を押しながら電源を入れると強制校正モードになります。顔が隠れて緑色になるので，ATOM Babies を外気に当たるところ（かつ，日の当たらないところ）に置いて，再度顔が表示されるまで約 3 分間ほどそのままに置いておいてください。

強制校正モードでは外気の二酸化炭素濃度を 450ppm として校正します。別の値で強制校正をしたい場合は，ソースコードの値（`CO2Checker::DEFAULT_FORCED_TARGET_CO2_CONCENTRATION`）を変更してください。
