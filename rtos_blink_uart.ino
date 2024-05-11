/**
 * This example demonstrates Blinking an LED based on user input from UART.
 * 
 */

// Initial LED blink delay
static int wait = 1000;

// Task to blink led
void blink_led(void *pvParameters){
    while(1){
        digitalWrite(LED_BUILTIN, HIGH);
        vTaskDelay((TickType_t)(wait / portTICK_PERIOD_MS));

        digitalWrite(LED_BUILTIN, LOW);
        vTaskDelay((TickType_t)(wait / portTICK_PERIOD_MS));
    }
}

// Task to read UART input
void read_input(void *pvParameters){
    while(1){
        if(Serial.available() > 0){
            wait = Serial.parseInt();
            Serial.print("Delay is set to:");
            Serial.println(wait, DEC);
        }
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}

void setup() {  
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);

    vTaskDelay(1000/portTICK_PERIOD_MS);
    Serial.println("Uart is working");
    
    // Task creation
    xTaskCreate(
        blink_led,
        "Blink Led",
        512,
        NULL,
        1,
        NULL
    );

    xTaskCreate(
        read_input,
        "Read Input",
        1024,
        NULL,
        1,
        NULL
    );

    vTaskDelete(NULL);
}

void loop(){

}