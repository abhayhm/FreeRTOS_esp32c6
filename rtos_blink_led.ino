/**
 * This example demonstrates Blinking an LED using Task.
 * 
 */

// Task to blink led each second
void blink_led(void *pvParameters){
    while(1){
        digitalWrite(LED_BUILTIN, HIGH);
        vTaskDelay(1000/portTICK_PERIOD_MS);

        digitalWrite(LED_BUILTIN, LOW);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    // Task creation
    xTaskCreate(
        blink_led,
        "Blink Led",
        512,
        NULL,
        1,
        NULL
    );
}

void loop() {}

