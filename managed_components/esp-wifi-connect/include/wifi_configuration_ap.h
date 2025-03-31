#ifndef _WIFI_CONFIGURATION_AP_H_
#define _WIFI_CONFIGURATION_AP_H_

#include <string>
#include <functional>
#include <esp_http_server.h>
#include <esp_event.h>
#include <esp_timer.h>
#include "dns_server.h"
#include <esp_netif.h>

class WifiConfigurationAp {
public:
    // 定义回调函数类型
    using WifiConnectSuccessCallback = std::function<void(const std::string& ssid, const std::string& password, const std::string& uid)>;

    using WifiConnectFailCallback = std::function<void(const std::string& ssid, const std::string& password, const std::string& uid)>;
    static WifiConfigurationAp& GetInstance();

    void SetLanguage(const std::string&& language);
    void SetSsidPrefix(const std::string&& ssid_prefix);
    void Start();
    void Start(WifiConnectSuccessCallback success_cb, WifiConnectFailCallback fail_cb);
    void Stop();
    void StartSmartConfig();
    std::string GetWebServerUrl();

private:
    WifiConfigurationAp();
    ~WifiConfigurationAp();

    // 禁用拷贝构造和赋值操作
    WifiConfigurationAp(const WifiConfigurationAp&) = delete;
    WifiConfigurationAp& operator=(const WifiConfigurationAp&) = delete;

    void SetConnectCallbacks(WifiConnectSuccessCallback success_cb, WifiConnectFailCallback fail_cb);
    void StartAccessPoint();
    void StartWebServer();
    std::string GetSsid();
    bool ConnectToWifi(const std::string& ssid, const std::string& password);
    void Save(const std::string& ssid, const std::string& password);

    static void WifiEventHandler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data);
    static void IpEventHandler(void* arg, esp_event_base_t event_base,
                             int32_t event_id, void* event_data);
    static void SmartConfigEventHandler(void* arg, esp_event_base_t event_base,
                                      int32_t event_id, void* event_data);

    std::string language_;
    std::string ssid_prefix_;
    std::string current_uid_;  // 存储当前连接尝试的 UID
    bool is_connecting_ = false;

    esp_netif_t* ap_netif_ = nullptr;
    httpd_handle_t server_ = nullptr;
    EventGroupHandle_t event_group_ = nullptr;
    esp_timer_handle_t scan_timer_ = nullptr;
    esp_event_handler_instance_t instance_any_id_ = nullptr;
    esp_event_handler_instance_t instance_got_ip_ = nullptr;
    esp_event_handler_instance_t sc_event_instance_ = nullptr;
    DnsServer dns_server_;

    // 回调函数
    WifiConnectSuccessCallback success_callback_;
    WifiConnectFailCallback fail_callback_;
}; 

#endif // _WIFI_CONFIGURATION_AP_H_