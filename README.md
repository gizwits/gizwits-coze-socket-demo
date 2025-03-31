# GIZWITS COZE SOCKET

Gizwits coze demo

[https://github.com/gizwits/gizwits-coze-socket-demo](https://github.com/gizwits/gizwits-coze-socket-demo)

## 系统概述

该系统是基于ESP音频开发板构建的，实现了以下主要功能：
- AI 通话
- 音频文件播放（支持SPIFFS文件系统中的MP3文件）
- 与Gizwits云平台的连接和通信
- Wi-Fi配网和连接管理


## Board Config

需要先在ADF Board 里创建您的board

然后 

```
idf.py menuconfig
```
audio_hal 里选择您的board


## SDK集成

代码中的`sdk_init()`函数用于初始化与Gizwits云平台的连接：

```c
// 初始化回调
callback_init();
// SDK_ACTIVED_MODE_BUTTON_AND_WAKEUP 代表按钮+唤醒词模式 具体可看 sdk_api.h
sdk_init(HARD_VERSION, SOFT_VERSION, SDK_ACTIVED_MODE_BUTTON_AND_WAKEUP);
```

## 构建和运行
```c
idf.py build flash monitor
```

## 烧录授权
auth_data.bin 请联系我们获取

需要您提供日志里打印的 mac 地址
```
esptool.py write_flash 0x744000 auth_data.bin
```

## 下一步
SDK 会托管您的麦克风和喇叭

您可以根据您的按钮或者 led 等相关外设做更多的业务扩展
例如长按按钮关机，长按两个按钮重置等