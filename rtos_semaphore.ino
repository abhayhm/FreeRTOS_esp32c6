/**
 * This example demonstrates FreeRTOS Counting Semaphore.
 * 
 * Goal is to use a mutex and counting semaphores to protect the shared buffer 
 * so that each number (0 throguh 4) is printed exactly 3 times to the Serial 
 * monitor (in any order).
 * 
 * Date: January 24, 2021
 * Author: Shawn Hymel
 * License: 0BSD
 * 
 * Modified by: Abhay HM
 * 
 */

// Required config
enum {BUF_SIZE = 5};                  // Size of buffer array
static const int num_prod_tasks = 5;  // Number of producer tasks
static const int num_cons_tasks = 2;  // Number of consumer tasks
static const int num_writes = 3;      // Num times each producer writes to buf

// Globals
static int buf[BUF_SIZE];             // Shared buffer
static int head = 0;                  // Writing index to buffer
static int tail = 0;                  // Reading index to buffer
static SemaphoreHandle_t bin_sem;     // Waits for parameter to be read
static SemaphoreHandle_t mutux;       // For critical section
static SemaphoreHandle_t empty_sem;   // For empty slots in queue
static SemaphoreHandle_t filled_sem;  // For filled slots in queue 

// Producer task: write a given number of times to shared buffer
void producer(void *parameters) {

    // Copy the parameters into a local variable
    int num = *(int *)parameters;

    // Release the binary semaphore
    xSemaphoreGive(bin_sem);
    vTaskDelay(20 / portTICK_PERIOD_MS);
    // Fill shared buffer with task number
    for (int i = 0; i < num_writes; i++) {
        xSemaphoreTake(empty_sem, portMAX_DELAY);
        xSemaphoreTake(mutux,portMAX_DELAY);
        
        // Critical section (accessing shared buffer)
        buf[head] = num;
        head = (head + 1) % BUF_SIZE;
        xSemaphoreGive(mutux);
        xSemaphoreGive(filled_sem);
    }

    // Delete self task
    vTaskDelete(NULL);
}

// Consumer: continuously read from shared buffer
void consumer(void *parameters) {
    int val;

    // Read from buffer
    while (1) {
        xSemaphoreTake(filled_sem, portMAX_DELAY);
        // Critical section (accessing shared buffer and Serial)
        xSemaphoreTake(mutux, portMAX_DELAY);
        val = buf[tail];
        tail = (tail + 1) % BUF_SIZE;
        Serial.println(val);
        xSemaphoreGive(mutux);
        xSemaphoreGive(empty_sem);
    }
}

//*****************************************************************************
// Main (runs as its own task with priority 1 on core 1)

void setup() {

    char task_name[12];
    
    // Configure Serial
    Serial.begin(115200);

    // Wait a moment to start (so we don't miss Serial output)
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.println();
    Serial.println("---FreeRTOS Semaphore Alternate Solution---");

    // Create mutexes and semaphores before starting tasks
    bin_sem = xSemaphoreCreateBinary();
    mutux = xSemaphoreCreateMutex();
    empty_sem = xSemaphoreCreateCounting(5, 5);
    filled_sem = xSemaphoreCreateCounting(5, 0);

    // Start producer tasks (wait for each to read argument)
    for (int i = 0; i < num_prod_tasks; i++) {
        sprintf(task_name, "Producer %i", i);
        xTaskCreate(producer,
                                task_name,
                                1024,
                                (void *)&i,
                                1,
                                NULL
                                );
        xSemaphoreTake(bin_sem, portMAX_DELAY);
    }

    // Start consumer tasks
    for (int i = 0; i < num_cons_tasks; i++) {
        sprintf(task_name, "Consumer %i", i);
        xTaskCreate(consumer,
                                task_name,
                                1024,
                                NULL,
                                1,
                                NULL
                                );
    }

    // Notify that all tasks have been created
    Serial.println("All tasks created");
}

void loop() {    
    // Do nothing but allow yielding to lower-priority tasks
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}