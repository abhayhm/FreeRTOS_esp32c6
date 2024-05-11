/**
 * This example demonstrates FreeRTOS task notification using uart to echo back the input.
 * 
 */

// Required globals
String message;

// Task handle
static TaskHandle_t xTaskToNotify = NULL;

// Task to read from uart and notify.
void readMsg_task(void *pvParameters){
    while(1){
        if(Serial.available() > 0){
            message = Serial.readStringUntil('\n');
            Serial.println("Successfully read the buffer.");
            vTaskDelay(5/portTICK_PERIOD_MS);
            xTaskNotifyGive(xTaskToNotify);
        }
    }
}

// Task to wakeup on notification and echo back the input
void printMsg_task(void *pvParameters){
    unsigned long notification;
    xTaskToNotify = xTaskGetCurrentTaskHandle();

    while(1){
        notification = ulTaskNotifyTake(pdTRUE,(TickType_t) portMAX_DELAY);
        if(notification > 0){
            Serial.print("Message: ");
            Serial.println(message);
            vTaskDelay(10/portTICK_PERIOD_MS);
        }
    }    
}

void setup(){
    Serial.begin(9600);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    Serial.println("Serial port working");

    // Task creation
    xTaskCreate(
        printMsg_task,
        "Print Message",
        1024,
        NULL,
        1,
        NULL
    );
    
    xTaskCreate(
        readMsg_task,
        "Read Message",
        1024,
        NULL,
        1,
        NULL
    );

    vTaskDelete(NULL);
}

void loop(){}