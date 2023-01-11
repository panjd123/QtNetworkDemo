# QtNetworkTest

Qt 网络功能 demo

## 如何开始

1. 双开软件，或准备两台电脑在同一局域网内的电脑。分别作为服务端和客户端
2. 服务端，设置好 `PORT` 后 `Reset` 应用修改。点击 `RestartServer` 启动服务端监听
3. 客户端，设置好 `IP` 和 `PORT` 后 `Reset` 应用修改。点击 `Reconnect`，连接到对应 `IP` 的服务端。连接结果会在 `reconnect` 左侧显示。
4. 客户端在 `ClientSend` 左侧输入信息，点击按钮发送
5. 服务端收到客户端的信息后，会显示在`ServerGet` 左侧， `RestartServer` 旁会显示这最后一次通信的客户端（的本地内存地址，只是用以区分，没什么用）。
6. 服务端得到 `lastone` 信息后，可以类似地发送信息给客户端

