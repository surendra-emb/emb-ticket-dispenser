#include <stdio.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "mqtt_client.h"

#include "lwip/err.h"
#include "lwip/sys.h"


#define CONFIG_ESP_WIFI_SSID "homenet6165_2.4"
#define CONFIG_ESP_WIFI_PASSWORD "CLB2722753"
#define CONFIG_ESP_MAXIMUM_RETRY 5

#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY


#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_BOTH
#define EXAMPLE_H2E_IDENTIFIER CONFIG_ESP_WIFI_PW_ID
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#define CONFIG_ESP_WIFI_PW_ID ""



/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1


#define BLINK_GPIO GPIO_NUM_46  // GPIO pin number where your LED is connected


// Debounce delay in milliseconds
#define DEBOUNCE_DELAY 		50
#define SW_GPIO1				GPIO_NUM_1
#define SW_GPIO2				GPIO_NUM_2

static const char *TAG = "ETD";

const char *ssid = "homenet6165_2.4";
const char *pass = "CLB2722753";


void switch_press_task1(void *arg) {
    gpio_set_direction(SW_GPIO1, GPIO_MODE_INPUT);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(SW_GPIO1, GPIO_PULLUP_ONLY);

    int switch_state = 1;  // Initialize to HIGH (not pressed)
    int last_state = 1;



    while (1) {
        int current_state = gpio_get_level(SW_GPIO1);

        if (current_state != last_state) {
            vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_DELAY));
            current_state = gpio_get_level(SW_GPIO1);

            if (current_state != last_state) {
                switch_state = current_state;
                // Handle the switch state change
                if (switch_state == 1) {			// sur.mah: confirm the hardware if switch_state should be 1 or 0 to switch
                										// on the led
                    // Switch is pressed
                	//msg_id = esp_mqtt_client_publish(client, pub_topic, message, 0, 0, 0);
                	//ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

                	// Button pressed, turn on the LED
                    gpio_set_level(BLINK_GPIO, true);

                } else {
                    // Button released, turn off the LED
                    gpio_set_level(BLINK_GPIO, false);
                    // Switch is released
                }
            }
        }

        last_state = current_state;
    }
}

void switch_press_task2(void *arg) {
    gpio_set_direction(SW_GPIO2, GPIO_MODE_INPUT);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(SW_GPIO2, GPIO_PULLUP_ONLY);

    int switch_state = 1;  // Initialize to HIGH (not pressed)
    int last_state = 1;



    while (1) {
        int current_state = gpio_get_level(SW_GPIO2);

        if (current_state != last_state) {
            vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_DELAY));
            current_state = gpio_get_level(SW_GPIO2);

            if (current_state != last_state) {
                switch_state = current_state;
                // Handle the switch state change
                if (switch_state == 1) {			// sur.mah: confirm the hardware if switch_state should be 1 or 0 to switch
                										// on the led
                    // Switch is pressed
                	//msg_id = esp_mqtt_client_publish(client, pub_topic, message, 0, 0, 0);
                	//ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

                	// Button pressed, turn on the LED
                    gpio_set_level(BLINK_GPIO, true);

                } else {
                    // Button released, turn off the LED
                    gpio_set_level(BLINK_GPIO, false);
                    // Switch is released
                }
            }
        }

        last_state = current_state;
    }
}






static int s_retry_num = 0;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}





void
wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            /* Authmode threshold resets to WPA2 as default if password matches WPA2 standards (pasword len => 8).
             * If you want to connect the device to deprecated WEP/WPA networks, Please set the threshold value
             * to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set the password with length and format matching to
             * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
             */
            .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
            .sae_pwe_h2e = ESP_WIFI_SAE_MODE,
            .sae_h2e_identifier = EXAMPLE_H2E_IDENTIFIER,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}



void
app_main() {

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
  //  wifi_init_sta();

    //xTaskCreate(&blink_task, "blink_task", 2048, NULL, 5, NULL);
    xTaskCreate(switch_press_task1, "switch_press_task1", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
    xTaskCreate(switch_press_task2, "switch_press_task2", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);

    while (1) {

        //vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
        vTaskDelay(pdMS_TO_TICKS(1000));		// Delay for 1 second
    }
}
