#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "mqtt_client.h"

static const char *TAG = "MCP9700A";

#define SENSOR_ADC_CHANNEL ADC_CHANNEL_0   // GPIO0 on ESP32-C6


esp_mqtt_client_config_t mqtt_cfg = {
    .broker.address.uri = "mqtt://192.168.1.123:1883",
};

esp_mqtt_client_handle_t client =
    esp_mqtt_client_init(&mqtt_cfg);

esp_mqtt_client_start(client);


void app_main(void)
{
    adc_oneshot_unit_handle_t adc_handle;

    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

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

    while (1) {
        while (1) {
        
            int raw;

            ESP_ERROR_CHECK(
                adc_oneshot_read(
                adc_handle,
                SENSOR_ADC_CHANNEL,
                &raw));

            // Convert ADC raw value to voltage in volts
            float voltage = ((float)raw / 4095.0f) * 3.3f;

            // MCP9700A: 500 mV at 0°C, 10 mV per °C
            float temperature = (voltage - 0.500f) / 0.010f;

            ESP_LOGI(TAG, "Raw: %d | Voltage: %.3f V | Temperature: %.2f °C",
                    raw, voltage, temperature);

            char json[128];

            snprintf(
            json,
            sizeof(json),
            "{\"device_id\":\"esp32-c6-01\",\"temperature\":%.2f,\"voltage\":%.3f}",
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
}