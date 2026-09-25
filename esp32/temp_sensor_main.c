#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"

#include "esp_adc/adc_oneshot.h"
#include "mqtt_client.h"

static const char *TAG = "MCP9700A";

static EventGroupHandle_t wifi_event_group;

#define SENSOR_ADC_CHANNEL ADC_CHANNEL_0   // GPIO0 on ESP32-C6


// wifi setup
#include "wifisecrets.h" //wifi information

#define WIFI_CONNECTED_BIT BIT0

static void wifi_event_handler( void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {

        ESP_LOGI(TAG, "Wi-Fi started, connecting...");
        ESP_ERROR_CHECK(esp_wifi_connect());

    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {

        ESP_LOGW(TAG, "Wi-Fi disconnected, retrying...");
        esp_wifi_connect();

    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {

        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI( TAG, "Got IP address: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits( wifi_event_group, WIFI_CONNECTED_BIT );
    }
}

static void wifi_init(void) {

    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK( esp_event_loop_create_default() );

    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_event_handler_register( WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL ) );
    ESP_ERROR_CHECK( esp_event_handler_register( IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL ) );

    wifi_config_t wifi_config = {
                .sta = 
            {   .ssid = WIFI_SSID,
                .password = WIFI_PASSWORD,
                .threshold.authmode = WIFI_AUTH_WPA2_PSK, }, 
            };
    
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config( WIFI_IF_STA, &wifi_config ) );
    ESP_ERROR_CHECK( esp_wifi_start() );

    ESP_LOGI(TAG, "Waiting for Wi-Fi connection...");

    xEventGroupWaitBits( 
        wifi_event_group,
        WIFI_CONNECTED_BIT,
        pdFALSE,
        pdTRUE,
        portMAX_DELAY );

    ESP_LOGI(TAG, "Wi-Fi connection established.");

}


void app_main(void)
{

    // NVS initialization
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    // Wi-Fi setup
    wifi_init();

    // MQTT setup
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://192.168.0.20:1883",
    };

    esp_mqtt_client_handle_t client =
        esp_mqtt_client_init(&mqtt_cfg);

    ESP_ERROR_CHECK(esp_mqtt_client_start(client));

    // ADC setup
    adc_oneshot_unit_handle_t adc_handle;

    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };

    ESP_ERROR_CHECK(
        adc_oneshot_new_unit(&init_config, &adc_handle)
    );

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };

    ESP_ERROR_CHECK(
        adc_oneshot_config_channel(
            adc_handle,
            SENSOR_ADC_CHANNEL,
            &config
        )
    );

    // Main sensor loop
    while (1) {

        int raw;

        ESP_ERROR_CHECK(
            adc_oneshot_read(
                adc_handle,
                SENSOR_ADC_CHANNEL,
                &raw
            )
        );

        // Convert ADC raw value to voltage
        float voltage = ((float)raw / 4095.0f) * 3.3f;

        // MCP9700A:
        // 500 mV at 0°C
        // 10 mV per °C
        float temperature = (voltage - 0.500f) / 0.010f;

        ESP_LOGI(
            TAG,
            "Raw: %d | Voltage: %.3f V | Temperature: %.2f °C",
            raw,
            voltage,
            temperature
        );

        char json[128];

        snprintf(
            json,
            sizeof(json),
            "\n{\n\"device_id\":\"Tempreture.01\",\n\"temperature\":%.2f,\n\"voltage\":%.3f\n}",
            temperature,
            voltage
        );

        esp_mqtt_client_publish(
            client,
            "sensors/esp32-c6-01/temperature",
            json,
            0,
            1,
            0
        );

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}