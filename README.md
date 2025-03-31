# SDK 示例代码分析及说明

## 概述

这是一个使用 libsocket_sdk.a 的示例代码，主要展示了如何初始化和使用SDK的各项功能。该示例包含了音频播放、音量控制、存储管理、SPIFFS文件系统挂载等功能，以及SDK初始化和事件处理的基本框架。

## 核心文件分析

### app_main.c

主程序文件，包含了系统初始化和主要功能实现：

1. **音量控制功能**
   - `user_set_volume()` - 设置音量
   - `user_set_volume_add()` - 增加音量
   - `user_set_volume_sub()` - 减少音量
   - `user_get_volume()` - 获取当前音量

2. **板级初始化**
   - `init_board()` - 初始化音频板、编解码器、SPIFFS文件系统
   - 加载保存的音量设置
   - 播放开机提示音

3. **主函数**
   - 初始化NVS闪存
   - 初始化音频板
   - 初始化SDK回调函数
   - 使用指定的硬件版本、软件版本和激活模式初始化SDK
   - 检查WiFi配置和连接状态

### sdk_api.h

SDK的API头文件，定义了SDK提供的功能接口：

1. **常量和枚举定义**
   - 休眠超时配置
   - 激活模式枚举：服务器VAD、按钮、按钮和唤醒词、服务器VAD和唤醒词

2. **SDK核心函数**
   - `sdk_init()` - SDK初始化
   - 版本信息获取函数
   - WiFi重置函数
   - 电源管理函数
   - 录音控制函数

3. **事件系统**
   - 用户事件枚举：休眠、设置音量、唤醒、AI切换、聊天进行中等
   - 回调函数类型定义
   - 回调函数设置接口

## 关键功能说明

### 1. 音频控制
代码实现了基本的音量控制功能，包括设置、增加和减少音量，以及音量范围限制(0-100)。音量设置会被保存到存储中。

### 2. 系统初始化
系统初始化包括NVS闪存初始化、音频板初始化、编解码器启动、SPIFFS文件系统挂载等步骤。

### 3. SDK初始化
使用`sdk_init()`函数初始化SDK，需要提供硬件版本、软件版本和激活模式。

### 4. 事件处理
SDK提供了事件通知系统，通过回调函数处理各类事件，如唤醒、WiFi状态变更、用户/AI说话等。

### 5. 文件系统
示例使用SPIFFS文件系统挂载音频文件，用于播放提示音等功能。

## 使用示例

```c
// 初始化SDK
sdk_init(HARD_VERSION, SOFT_VERSION, ACTIVED_MODE_SERVER_VAD_AND_WAKEUP);

// 设置音量
user_set_volume(80);

// 播放音频提示
audio_tone_play("spiffs://spiffs/converted_turn_on.mp3");

// 检查WiFi状态
bool is_config_wifi = get_is_config_wifi();
bool is_connected_wifi = get_wifi_is_connected();
```

## 注意事项

1. 使用前需要确保正确初始化NVS闪存
2. 音频提示文件需要预先存储在SPIFFS分区中
3. 需要正确设置硬件和软件版本
4. 根据需要选择合适的激活模式

## 总结

这个示例展示了使用libsocket_sdk.a进行开发的基本框架和常用功能，包括初始化、音频控制、事件处理等。开发者可以参考此示例进行自己的应用开发。
