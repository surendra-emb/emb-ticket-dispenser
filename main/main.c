#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

#define BLINK_GPIO GPIO_NUM_46  // GPIO pin number where your LED is connected

void blink_task(void *pvParameters) {
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    while (1) {
        // Toggle the LED state
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Delay for 1 second
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Delay for 1 second
    }
}


// Debounce delay in milliseconds
#define DEBOUNCE_DELAY 50
#define SW_GPIO		GPIO_NUM_1

void switch_press_task(void *arg) {
    gpio_set_direction(SW_GPIO, GPIO_MODE_INPUT);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(SW_GPIO, GPIO_PULLUP_ONLY);

    int switch_state = 1;  // Initialize to HIGH (not pressed)
    int last_state = 1;



    while (1) {
        int current_state = gpio_get_level(SW_GPIO);

        if (current_state != last_state) {
            vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_DELAY));
            current_state = gpio_get_level(SW_GPIO);

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


void app_main() {



    //xTaskCreate(&blink_task, "blink_task", 2048, NULL, 5, NULL);
    xTaskCreate(switch_press_task, "switch_press_task", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
}
