/**
 * This example demonstrates FreeRTOS queue using uart to read input and passing delay to blink task.
 * After successful 100 blinks, passing string and blink count on another queue to first task.
 * 
 */

// String
char blinked[] = "Blinked";

struct blink_message{
    char* s;
    u32_t t;
};

QueueHandle_t wait_queue = xQueueCreate((UBaseType_t)5, (UBaseType_t)sizeof(int));;
QueueHandle_t message_queue = xQueueCreate((UBaseType_t)5, (UBaseType_t)sizeof(struct blink_message));

void taskA(void *pvParameters){
    int wait = 0;
    String message;
    struct blink_message blk_msg;

    while(1){
        if(Serial.available()>0){
            message = Serial.readStringUntil('\n');
            if(message.startsWith("delay")){
                message.remove(0, 6);
                wait = message.toInt();
                if(errQUEUE_FULL == xQueueSend(wait_queue, (void *)&wait, (TickType_t)0)){
                    Serial.println("Message queue is full");
                }
            }
            else{
                Serial.println(message);
            }
        }
        if(pdTRUE == xQueueReceive(message_queue, (void *)&blk_msg, (TickType_t)0)){
            Serial.print(blk_msg.s);
            Serial.print(" ");
            Serial.println(blk_msg.t);
        }
    }

}

void taskB(void *pvParameters){
    int wait = 1000;
    u32_t count = 0;
    struct blink_message blk_msg;

    while(1){
        xQueueReceive(wait_queue, (void *)&wait, (TickType_t)0);

        digitalWrite(LED_BUILTIN, HIGH);
        vTaskDelay(wait/portTICK_PERIOD_MS);
        digitalWrite(LED_BUILTIN, LOW);
        vTaskDelay(wait/portTICK_PERIOD_MS);
        count++;

        if(count%100 == 0){
            blk_msg.s = blinked;
            blk_msg.t = count;
            xQueueSend(message_queue, (void *)&blk_msg, (TickType_t)0);
        }
    }
}

void setup(){
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    vTaskDelay(1000/portTICK_PERIOD_MS);
    Serial.println("Initialization complete");

    if(NULL != wait_queue && NULL != message_queue){
        Serial.println("Queues created successfully");
        xTaskCreate(
            taskA,
            "Task A",
            3072,
            NULL,
            1,
            NULL
        );

        xTaskCreate(
            taskB,
            "Task B",
            3072,
            NULL,
            1,
            NULL
        );
        Serial.println("Tasks created successfully");
    }
    else{
        Serial.println("Unable to allocate memory for queue");
    }

    vTaskDelete(NULL);
}

void loop(){}