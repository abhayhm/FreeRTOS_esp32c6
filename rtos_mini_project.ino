/**
 * This example demonstrates FreeRTOS software timer.
 * Goal is to implement a hardware timer in the ESP32 that samples from an ADC pin once every 100 ms. 
 * This sampled data is copied to a double buffer. Whenever one of the buffers is full, the ISR will notify Task A.
 * Task A, when it receives notification from the ISR, will wake up and compute the average of the previously collected 10 samples.
 * When Task A is finished, it will update a global floating point variable that contains the newly computed average.
 * Task B will echo any characters received over the serial port back to the same serial port.
 * If the command “average” is entered, it will display whatever is in the global average variable.
 */

// Required handles
hw_timer_t* timer = NULL;
const uint16_t timer_divider = 8;
const uint64_t timer_max_count = 1000000;

QueueHandle_t writeq = NULL;
QueueHandle_t readq = NULL;

SemaphoreHandle_t buff_sem_read = NULL;
SemaphoreHandle_t buff_sem_write = NULL;
SemaphoreHandle_t mutex_average = NULL;

double average = 0.0;

// Task to compute average
void compute_average(void* pvParameters){
    double average_local = 0.0;
    uint16_t value = 0.0;

    while(1){
        average_local = 0.0;
        xSemaphoreTake(buff_sem_read, portMAX_DELAY);
        for(int i = 0; i < 10; i++){
            xQueueReceive(readq, (void*)&value, portMAX_DELAY);
            average_local += value;
        }
        xSemaphoreGive(buff_sem_write);

        average_local /= 10.0;
        xSemaphoreTake(mutex_average, portMAX_DELAY);
        average = average_local;
        xSemaphoreGive(mutex_average);
    }
}

// Task to handle user input
void serial_handler(void* pvParameters){
    String buffer;

    while(1){
        if(Serial.available()>0){
            buffer = Serial.readStringUntil('\n');
            if(buffer.equalsIgnoreCase("average")){
                xSemaphoreTake(mutex_average, portMAX_DELAY);
                Serial.print("ADC average: ");
                Serial.println(average);
                xSemaphoreGive(mutex_average);
            }
        }
    }
}

// Timer callback function
void ARDUINO_ISR_ATTR onTrimer(void){
    static uint8_t overrun = 0;
    uint16_t adc_value = analogRead(A1);
    Serial.println("Read analog");
    if(overrun >= 10){
        QueueHandle_t temp = writeq;
        writeq = readq;
        readq = temp;
        overrun = 0;
        xSemaphoreGiveFromISR(buff_sem_read, NULL);
    }
    Serial.println("Swapped");
    if(pdTRUE == xSemaphoreTakeFromISR(buff_sem_write,NULL)){
        xQueueSendFromISR(writeq, &adc_value, NULL);
        overrun++;
    }
}

void setup(){    
    pinMode(A1, INPUT);
    Serial.begin(115200);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    
    // Initializing required data structures
    Serial.println("Initializing");

    writeq = xQueueCreate(10, sizeof(uint16_t));
    readq = xQueueCreate(10, sizeof(uint16_t));

    buff_sem_read = xSemaphoreCreateBinary();
    buff_sem_write = xSemaphoreCreateBinary();

    mutex_average = xSemaphoreCreateMutex();

    Serial.println("Initialized Queues, Semaphores and Mutex");
    vTaskDelay(100/portTICK_PERIOD_MS);

    // Task creation
    xTaskCreate(
        compute_average,
        "Compute Average",
        1024,
        NULL,
        1,
        NULL
    );
    xTaskCreate(
        serial_handler,
        "Serial Input Handler",
        1024,
        NULL,
        1,
        NULL
    );
    Serial.println("Initialized Tasks");
    vTaskDelay(100/portTICK_PERIOD_MS); 
    
    // Setting up and starting the timer
    timer = timerBegin(1600);
    timerAttachInterrupt(timer, &onTrimer);
    timerAlarm(timer, timer_max_count, true, 0);

    Serial.println("Initialized Timer");
    vTaskDelay(100/portTICK_PERIOD_MS);
    
    vTaskDelete(NULL);
}

void loop(){}