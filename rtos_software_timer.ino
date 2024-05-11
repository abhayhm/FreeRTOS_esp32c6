/**
 * This example demonstrates FreeRTOS software timer.
 * Goal is to impliment LCD backlight autodim feature using uart and softare timer.
 */

// Oneshot timer callback function
void led_off(TimerHandle_t xTimer){
    digitalWrite(LED_BUILTIN, LOW);
}

// One shot timer
TimerHandle_t oneShotTimer = xTimerCreate(
    "One shot timer",
    2500/portTICK_PERIOD_MS,
    pdFALSE,
    (void *)0,
    led_off);

// Task to check if user is active
void read_input(void* pvParameters){
    while(1){
        if(Serial.available()>0){
            Serial.read();
            digitalWrite(LED_BUILTIN, HIGH);
            xTimerStart(oneShotTimer,portMAX_DELAY);
        }
    }
}

void setup(){
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    // Task creation
    xTaskCreate(
        read_input,
        "Read uart input",
        1024,
        NULL,
        1,
        NULL);

    vTaskDelete(NULL);
}

void loop(){}