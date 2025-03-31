// 用于实现客户自身使用的API

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <esp_wifi.h>
#include <inttypes.h>
#include <esp_log.h>
#include "cJSON.h"
#include "sdk_api.h"

static const char *TAG = "user_api";

// Implementation of notification callbacks
static void app_plugin_notify(char *data) {
    ESP_LOGE(TAG, "on_coze_plugin_notify: %s", data);
    char event_id[32];
    snprintf(event_id, sizeof(event_id), "%lld", esp_timer_get_time());
    cJSON *root = cJSON_Parse(data);
    if (!root) {
        ESP_LOGE(TAG, "Failed to parse JSON");
        return;
    }
    
    cJSON *data_obj = cJSON_GetObjectItem(root, "data");
    if (!data_obj) {
        ESP_LOGE(TAG, "No data field in JSON");
        cJSON_Delete(root);
        return;
    }
    
    cJSON *req_action = cJSON_GetObjectItem(data_obj, "required_action");
    if (!req_action) {
        ESP_LOGE(TAG, "No required_action field in data");
        cJSON_Delete(root);
        return;
    }
    
    cJSON *submit_tools = cJSON_GetObjectItem(req_action, "submit_tool_outputs");
    if (!submit_tools) {
        ESP_LOGE(TAG, "No submit_tool_outputs field in required_action");
        cJSON_Delete(root);
        return;
    }
    
    cJSON *tool_calls = cJSON_GetObjectItem(submit_tools, "tool_calls");
    if (!tool_calls || cJSON_GetArraySize(tool_calls) == 0) {
        ESP_LOGE(TAG, "No tool_calls array or empty array");
        cJSON_Delete(root);
        return;
    }
    
    cJSON *tool_call = cJSON_GetArrayItem(tool_calls, 0);
    if (!tool_call) {
        ESP_LOGE(TAG, "Failed to get first tool_call");
        cJSON_Delete(root);
        return;
    }
    
    cJSON *tool_call_id = cJSON_GetObjectItem(tool_call, "id");
    if (!tool_call_id || !cJSON_IsString(tool_call_id)) {
        ESP_LOGE(TAG, "No valid id in tool_call");
        cJSON_Delete(root);
        return;
    }

    cJSON *function = cJSON_GetObjectItem(tool_call, "function");
    if (!function) {
        ESP_LOGE(TAG, "No function in tool_call");
        cJSON_Delete(root);
        return;
    }

    cJSON *name = cJSON_GetObjectItem(function, "name");
    if (!name || !cJSON_IsString(name)) {
        ESP_LOGE(TAG, "No valid name in function");
        cJSON_Delete(root);
        return;
    }

    cJSON *arguments = cJSON_GetObjectItem(function, "arguments");
    if (!arguments || !cJSON_IsString(arguments)) {
        ESP_LOGE(TAG, "No valid arguments in function");
        cJSON_Delete(root);
        return;
    }

    // 解析 arguments 字符串，它本身是一个 JSON 字符串
    cJSON *args_json = cJSON_Parse(arguments->valuestring);
    if (!args_json) {
        ESP_LOGE(TAG, "Failed to parse arguments JSON: %s", arguments->valuestring);
        cJSON_Delete(root);
        return;
    }

    
    // 获取conversation_id
    cJSON *conv_id = cJSON_GetObjectItem(data_obj, "conversation_id");
    if (!conv_id || !cJSON_IsString(conv_id)) {
        ESP_LOGE(TAG, "No valid conversation_id");
        cJSON_Delete(args_json);
        cJSON_Delete(root);
        return;
    }
}

static bool is_first_wakeup = true;
static void app_event_notify(user_event_t event, cJSON *json_data) {
    switch(event) {
        case USER_EVENT_SLEEP:
            ESP_LOGI(TAG, "User event: Sleep");
            // battery_state_t state = get_battery_state();
            // ESP_LOGW(TAG, "battery state: %d", state);
            audio_tone_play("spiffs://spiffs/converted_sleep.mp3");
            // TODO
            // if (state == BATTERY_NOT_CHARGING) {
            //     // 电池状态下，直接关机
            //     vTaskDelay(pdMS_TO_TICKS(3000));
            //     gpio_set_power_status(0);
            // } else {
            //     // 关灯
            //     led_effect_stop();
            // }
            break;
        case USER_EVENT_USER_SPEAKING:
            ESP_LOGI(TAG, "User event: User speaking");
            break;
        case USER_EVENT_STANDBY:
            ESP_LOGI(TAG, "User event: Standby");
            break;
        case USER_EVENT_WIFI_RECONNECT_FAILED:
            ESP_LOGI(TAG, "User event: WiFi reconnect failed");
            audio_tone_play("spiffs://spiffs/converted_connectFail.mp3");
            break;
        case USER_EVENT_CHANGING_AI_AGENT:
            ESP_LOGI(TAG, "User event: Changing AI agent");
            audio_tone_play("spiffs://spiffs/config_ai_success.mp3");
            break;
        case USER_EVENT_NET_WORK_ERROR:
            ESP_LOGI(TAG, "User event: Network error");
            audio_tone_play("spiffs://spiffs/network_error.mp3");
            break;
        case USER_EVENT_SET_VOLUME:
            ESP_LOGI(TAG, "User event: Set volume");
            cJSON *volume = cJSON_GetObjectItem(json_data, "volume");
            if (volume && cJSON_IsNumber(volume)) {
                ESP_LOGI(TAG, "Volume: %d", volume->valueint);
                user_set_volume(volume->valueint);
                audio_tone_play("spiffs://spiffs/bo.mp3");
            }
            break;
        case USER_EVENT_CHAT_IN_PROGRESS:
#if defined (CONFIG_ACTIVED_MODE_BUTTON)
            audio_tone_play("spiffs://spiffs/bo.mp3");
#endif
            break;
        case USER_EVENT_WAKEUP:
            if (is_first_wakeup == false) {
                audio_tone_play("spiffs://spiffs/bo.mp3");
            }
            is_first_wakeup = false;
            break;
        case USER_EVENT_AI_SPEAKING:
            ESP_LOGI(TAG, "User event: AI speaking");
            break;
        case USER_EVENT_WIFI_INIT:
            ESP_LOGI(TAG, "User event: WiFi initialization");
            break;
        case USER_EVENT_WIFI_DISCONNECTED:
            ESP_LOGI(TAG, "User event: WiFi disconnected");
            break;
        case USER_EVENT_WIFI_CONNECTED:
            ESP_LOGI(TAG, "User event: WiFi connected");

            break;
        case USER_EVENT_WIFI_RECONNECTED:
            ESP_LOGI(TAG, "User event: WiFi reconnected");
            break;
        case USER_EVENT_WIFI_CONNECTING:
            audio_tone_play("spiffs://spiffs/converted_startConnect.mp3");
            ESP_LOGI(TAG, "User event: WiFi connecting");
            break;
    }
}

// Initialize callbacks during app startup
void callback_init(void) {
    // Set up callbacks
    sdk_set_coze_plugin_notify_callback(app_plugin_notify);
    sdk_set_user_event_notify_callback(app_event_notify);
}
