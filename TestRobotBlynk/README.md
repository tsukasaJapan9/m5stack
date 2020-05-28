# setup
### package install
```
$ sudo apt install mosquitto
$ sudo apt install mosquitto-clients
```
### broker check
パッケージをインストールした時点でPC上でMQTTのbrokerが起動しているため、一応動作確認する(ポートはデフォルトだと1883)

サブスクライバを待機させる
```
$ mosquitto_sub -h localhost -t test/control
```

メッセージをバブリッシュする
```
$ mosquitto_pub -h localhost -t test/control -m hello
```

サブスクライバにメッセージが飛んでくればbrokerはOK
```
$ mosquitto_sub -h localhost -t test/control
hello
```

# m5stackを使った動作確認
プログラムを書き込んでbrokerに繋がったというメッセージがでたらメッセージをパブリッシュ
```
$ mosquitto_pub -h localhost -t m5stack/control -m 1
```