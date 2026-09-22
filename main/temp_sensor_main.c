#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"

static const char *TAG = "MCP9700A";

#define SENSOR_ADC_CHANNEL ADC_CHANNEL_0   // GPIO0 on ESP32-C6

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
        int raw;

        ESP_ERROR_CHECK(
            adc_oneshot_read(
                adc_handle,
                SENSOR_ADC_CHANNEL,
                &raw
            )
        );
        int temperature = (raw - 0.500) / 0.010;
        ESP_LOGI(TAG, "Temp: %d", temperature);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}