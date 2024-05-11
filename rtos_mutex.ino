/**
 * This example demonstrates FreeRTOS mutex to prevent stack variable getting 
 * destroyed before being copied.
 * 
 */

SemaphoreHandle_t lock = xSemaphoreCreateMutex();

// Blink LED based on rate passed by parameter
void blinkLED(void *parameters) {
    int num = 0;

    if( pdTRUE == xSemaphoreTake(lock, 0)){
        // Copy the parameter into a local variable
        num = *(int *)parameters;
        // Print the parameter
        Serial.print("Received: ");
        Serial.println(num);
    }

    xSemaphoreGive(lock);
    // Configure the LED pin
    pinMode(LED_BUILTIN, OUTPUT);

    // Blink forever and ever
    while (1) {
        digitalWrite(LED_BUILTIN, HIGH);
        vTaskDelay(num / portTICK_PERIOD_MS);
        digitalWrite(LED_BUILTIN, LOW);
        vTaskDelay(num / portTICK_PERIOD_MS);
    }
}

//*****************************************************************************
// Main (runs as its own task with priority 1 on core 1)

void setup() {
    long int delay_arg;

    Serial.begin(115200);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.println("Enter a number for delay (milliseconds)");

    // Wait for input from Serial
    while (Serial.available() <= 0);

    // Read integer value
    delay_arg = Serial.parseInt();
    Serial.print("Sending: ");
    Serial.println(delay_arg);

    // Task creation
    xTaskCreate(
      blinkLED,
      "Blink LED",
      1024,
      (void *)&delay_arg,
      1,
      NULL
      );

    xSemaphoreTake(lock, 100/portTICK_PERIOD_MS);
    
    Serial.println("Done!");
    xSemaphoreGive(lock);
}

void loop() {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}