#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include <math.h>

#define ADC_CHANNEL ADC_CHANNEL_6
#define ADC_BITWIDTH ADC_BITWIDTH_12
#define ADC_ATTEN ADC_ATTEN_DB_12

#define WIFI_SSID CONFIG_WIFI_SSID
#define WIFI_PASS CONFIG_WIFI_PASS
#define MQTT_BROKER CONFIG_MQTT_BROKER
#define MQTT_COMMAND_TOPIC CONFIG_MQTT_COMMAND_TOPIC
#define MQTT_RESPONSE_TOPIC CONFIG_MQTT_RESPONSE_TOPIC

static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle);
static void adc_calibration_deinit(adc_cali_handle_t handle);

adc_oneshot_unit_handle_t adc_handle;
adc_oneshot_unit_init_cfg_t init_config = {
    .unit_id = ADC_UNIT_1,
    .ulp_mode = ADC_ULP_MODE_DISABLE,
};
adc_oneshot_chan_cfg_t config = {
    .bitwidth = ADC_BITWIDTH,
    .atten = ADC_ATTEN,
};

int adc_raw;
int voltage;

void sample()
{   
    adc_oneshot_new_unit(&init_config, &adc_handle);

    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &config);

    adc_cali_handle_t adc_cali_handle = NULL;
    adc_calibration_init(ADC_UNIT_1, ADC_CHANNEL, ADC_ATTEN, &adc_cali_handle);

    adc_oneshot_read(adc_handle, ADC_CHANNEL, &adc_raw);
    
    adc_cali_raw_to_voltage(adc_cali_handle, adc_raw, &voltage);
    
    adc_oneshot_del_unit(adc_handle);
    
    adc_calibration_deinit(adc_cali_handle);
}

void app_main(void)
{

    while (1) {
        sample();
        printf("raw: %d\n", adc_raw);
        printf("cali: %d\n", voltage);

        int temp = ((10.888 - sqrt( pow(-10.888, 2) + 4 * 0.00347 * (1777.3 - voltage))) / (2 * -0.00347)) + 30;

        printf("temp: %d\n", temp);

        printf("WIFI_SSID: %s\n", WIFI_SSID);
        printf("WIFI_PASS: %s\n", WIFI_PASS);
        printf("MQTT_BROKER: %s\n", MQTT_BROKER);
        printf("MQTT_COMMAND_TOPIC: %s\n", MQTT_COMMAND_TOPIC);
        printf("MQTT_RESPONSE_TOPIC: %s\n", MQTT_RESPONSE_TOPIC);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/*---------------------------------------------------------------
        ADC Calibration
---------------------------------------------------------------*/
static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated) {
        printf("calibration scheme version is %s\n", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .chan = channel,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated) {
        printf("calibration scheme version is %s\n", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

    *out_handle = handle;
    if (ret == ESP_OK) {
        printf("Calibration Success\n");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        printf("eFuse not burnt, skip software calibration\n");
    } else {
        printf("Invalid arg or no memory\n");
    }

    return calibrated;
}

static void adc_calibration_deinit(adc_cali_handle_t handle)
{
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    printf("deregister %s calibration scheme\n", "Curve Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(handle));

#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    printf("deregister %s calibration scheme\n", "Line Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));
#endif
}