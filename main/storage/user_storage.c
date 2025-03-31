#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "user_storage.h"

static const char *TAG = "USER_STORAGE";

// NVS 命名空间
#define STORAGE_NAMESPACE "local_data"

// 存储键名
#define RGB_RED_KEY "rgb_red"
#define RGB_GREEN_KEY "rgb_green"
#define RGB_BLUE_KEY "rgb_blue"
#define RGB_BRIGHTNESS_KEY "rgb_bright"
#define VOLUME_KEY "volume"

/**
 * 初始化存储系统
 * 
 * @return ESP_OK 成功，其他值表示失败
 */
esp_err_t user_storage_init(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS 分区被格式化，重新初始化
        ESP_LOGI(TAG, "Erasing NVS flash...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize NVS: %s", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "User storage initialized successfully");
    }
    
    return err;
}

/**
 * 保存 RGB 颜色值
 * 
 * @param r 红色分量 (0-255)
 * @param g 绿色分量 (0-255)
 * @param b 蓝色分量 (0-255)
 * @return ESP_OK 成功，其他值表示失败
 */
esp_err_t user_storage_save_rgb_color(uint8_t r, uint8_t g, uint8_t b)
{
    nvs_handle_t handle;
    esp_err_t err;
    
    // 打开 NVS 句柄
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS handle: %s", esp_err_to_name(err));
        return err;
    }
    
    // 写入 RGB 颜色分量
    err = nvs_set_u8(handle, RGB_RED_KEY, r);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error saving red component: %s", esp_err_to_name(err));
        nvs_close(handle);
        return err;
    }
    
    err = nvs_set_u8(handle, RGB_GREEN_KEY, g);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error saving green component: %s", esp_err_to_name(err));
        nvs_close(handle);
        return err;
    }
    
    err = nvs_set_u8(handle, RGB_BLUE_KEY, b);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error saving blue component: %s", esp_err_to_name(err));
        nvs_close(handle);
        return err;
    }
    
    // 提交更改
    err = nvs_commit(handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error committing NVS: %s", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "RGB color (%d, %d, %d) saved successfully", r, g, b);
    }
    
    // 关闭句柄
    nvs_close(handle);
    return err;
}

/**
 * 读取 RGB 颜色值
 * 
 * @param r 指向存储红色分量的变量的指针
 * @param g 指向存储绿色分量的变量的指针
 * @param b 指向存储蓝色分量的变量的指针
 * @param default_r 默认红色分量
 * @param default_g 默认绿色分量
 * @param default_b 默认蓝色分量
 * @return ESP_OK 成功，其他值表示失败
 */
esp_err_t user_storage_load_rgb_color(uint8_t *r, uint8_t *g, uint8_t *b, 
                                     uint8_t default_r, uint8_t default_g, uint8_t default_b)
{
    if (r == NULL || g == NULL || b == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    nvs_handle_t handle;
    esp_err_t err;
    bool using_defaults = false;
    
    // 打开 NVS 句柄
    err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS handle: %s", esp_err_to_name(err));
        *r = default_r;
        *g = default_g;
        *b = default_b;
        return err;
    }
    
    // 读取红色分量
    err = nvs_get_u8(handle, RGB_RED_KEY, r);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        *r = default_r;
        using_defaults = true;
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error loading red component: %s", esp_err_to_name(err));
        *r = default_r;
    }
    
    // 读取绿色分量
    err = nvs_get_u8(handle, RGB_GREEN_KEY, g);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        *g = default_g;
        using_defaults = true;
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error loading green component: %s", esp_err_to_name(err));
        *g = default_g;
    }
    
    // 读取蓝色分量
    err = nvs_get_u8(handle, RGB_BLUE_KEY, b);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        *b = default_b;
        using_defaults = true;
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error loading blue component: %s", esp_err_to_name(err));
        *b = default_b;
    }
    
    // 关闭句柄
    nvs_close(handle);
    
    if (using_defaults) {
        ESP_LOGI(TAG, "Some RGB components not found, using defaults: (%d, %d, %d)", *r, *g, *b);
    } else {
        ESP_LOGI(TAG, "Loaded RGB color: (%d, %d, %d)", *r, *g, *b);
    }
    
    return ESP_OK;
}

/**
 * 保存 RGB 亮度值
 * 
 * @param brightness 亮度值 (0-100)
 * @return ESP_OK 成功，其他值表示失败
 */
esp_err_t user_storage_save_brightness(uint8_t brightness)
{
    nvs_handle_t handle;
    esp_err_t err;
    
    // 确保亮度在有效范围内
    if (brightness > 100) {
        brightness = 100;
    }
    
    // 打开 NVS 句柄
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS handle: %s", esp_err_to_name(err));
        return err;
    }
    
    // 写入亮度值
    err = nvs_set_u8(handle, RGB_BRIGHTNESS_KEY, brightness);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error saving brightness: %s", esp_err_to_name(err));
        nvs_close(handle);
        return err;
    }
    
    // 提交更改
    err = nvs_commit(handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error committing NVS: %s", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "Brightness %d%% saved successfully", brightness);
    }
    
    // 关闭句柄
    nvs_close(handle);
    return err;
}

/**
 * 读取 RGB 亮度值
 * 
 * @param brightness 指向存储亮度值的变量的指针
 * @param default_brightness 如果没有存储的值，使用的默认亮度
 * @return ESP_OK 成功，其他值表示失败
 */
esp_err_t user_storage_load_brightness(uint8_t *brightness, uint8_t default_brightness)
{
    if (brightness == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    nvs_handle_t handle;
    esp_err_t err;
    
    // 打开 NVS 句柄
    err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS handle: %s", esp_err_to_name(err));
        *brightness = default_brightness;
        return err;
    }
    
    // 读取亮度值
    err = nvs_get_u8(handle, RGB_BRIGHTNESS_KEY, brightness);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(TAG, "Brightness not found, using default: %d%%", default_brightness);
        *brightness = default_brightness;
        err = ESP_OK;
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error loading brightness: %s", esp_err_to_name(err));
        *brightness = default_brightness;
    } else {
        ESP_LOGI(TAG, "Loaded brightness: %d%%", *brightness);
    }
    
    // 关闭句柄
    nvs_close(handle);
    return err;
}

/**
 * 保存音量值
 * 
 * @param volume 音量值 (0-100)
 * @return ESP_OK 成功，其他值表示失败
 */
esp_err_t user_storage_save_volume(uint8_t volume)
{
    nvs_handle_t handle;
    esp_err_t err;
    
    // 确保音量在有效范围内
    if (volume > 100) {
        volume = 100;
    }
    
    // 打开 NVS 句柄
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS handle: %s", esp_err_to_name(err));
        return err;
    }
    
    // 写入音量值
    err = nvs_set_u8(handle, VOLUME_KEY, volume);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error saving volume: %s", esp_err_to_name(err));
        nvs_close(handle);
        return err;
    }
    
    // 提交更改
    err = nvs_commit(handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error committing NVS: %s", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "Volume %d%% saved successfully", volume);
    }
    
    // 关闭句柄
    nvs_close(handle);
    return err;
}

/**
 * 读取音量值
 * 
 * @param volume 指向存储音量值的变量的指针
 * @param default_volume 如果没有存储的值，使用的默认音量
 * @return ESP_OK 成功，其他值表示失败
 */
esp_err_t user_storage_load_volume(uint8_t *volume, uint8_t default_volume)
{
    if (volume == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    nvs_handle_t handle;
    esp_err_t err;
    
    // 打开 NVS 句柄
    err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS handle: %s", esp_err_to_name(err));
        *volume = default_volume;
        return err;
    }
    
    // 读取音量值
    err = nvs_get_u8(handle, VOLUME_KEY, volume);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(TAG, "Volume not found, using default: %d%%", default_volume);
        *volume = default_volume;
        err = ESP_OK;
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error loading volume: %s", esp_err_to_name(err));
        *volume = default_volume;
    } else {
        ESP_LOGI(TAG, "Loaded volume: %d%%", *volume);
    }
    
    // 关闭句柄
    nvs_close(handle);
    return err;
}
