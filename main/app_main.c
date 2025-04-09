/* Play WAV file processed by ALC

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "esp_err.h"
#include "board.h"
#include "periph_spiffs.h"
#include "config.h"
#include "sdk_api.h"

static const char *TAG = "USE_GIZWITS_EXAMPLE";

#define DEFAULT_VOLUME 80
#define MAX_VOLUME 100
static audio_board_handle_t audio_board_handle = NULL;
static int volume = DEFAULT_VOLUME;


void user_set_volume_add(int vol)
{
    user_set_volume(volume + vol);
}

void user_set_volume_sub(int vol)
{
    user_set_volume(volume - vol);
}

void user_set_volume(int vol) {
     if(vol < 0) {
        vol = 0; 
    }
    if(vol > MAX_VOLUME) {
        vol = MAX_VOLUME;
    }
    audio_hal_set_volume(audio_board_handle->audio_hal, vol);
    volume = vol;
    // 保存新的音量设置
    user_storage_save_volume(volume);
}

int user_get_volume() {
    return volume;
}

void init_board()
{
    audio_board_handle = audio_board_init();
    audio_hal_ctrl_codec(audio_board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_BOTH, AUDIO_HAL_CTRL_START);

    uint8_t volume;
    user_storage_load_volume(&volume, DEFAULT_VOLUME);
    ESP_LOGI(TAG, "volume: %d", volume);
    audio_hal_set_volume(audio_board_handle->audio_hal, volume);
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t esp_set = esp_periph_set_init(&periph_cfg);
    // 挂 spiffs
    periph_spiffs_cfg_t spiffs_cfg = {
        .root = "/spiffs",
        .partition_label = "flash_tone",
        .max_files = 12,
        .format_if_mount_failed = true};
    esp_periph_handle_t spiffs_handle = periph_spiffs_init(&spiffs_cfg);
    esp_periph_start(esp_set, spiffs_handle);
    // 启动本地播放
    audio_tone_init();
    audio_tone_play("spiffs://spiffs/converted_turn_on.mp3");
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    } else {
        ESP_LOGE(TAG, "nvs_flash_init failed: %d", ret);
    }


    init_board();

    sdk_init(HARD_VERSION, SOFT_VERSION, SDK_ACTIVED_MODE_SERVER_VAD);
    callback_init();
    bool is_config_wifi = get_is_config_wifi();
    bool is_connected_wifi = get_wifi_is_connected();

    print_heap_info(TAG, NULL);

    if(is_config_wifi == false) {
        user_set_volume(80);
        audio_tone_play("spiffs://spiffs/go_to_config.mp3");
    } else if (is_connected_wifi == false) {
    }
}
