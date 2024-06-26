/**
 * FreeRtos Dining Philosophers using Dijkstra hierarchy/priority assignment to solve livelock.
 * 
 * Date: February 8, 2021
 * Author: Shawn Hymel
 * License: 0BSD
 * 
 * Modified by: Abhay HM
 */

// Settings
enum { NUM_TASKS = 5 };           // Number of tasks (philosophers)
enum { TASK_STACK_SIZE = 2048 };  // Bytes in ESP32, words in vanilla FreeRTOS

// Globals
static SemaphoreHandle_t bin_sem;   // Wait for parameters to be read
static SemaphoreHandle_t done_sem;  // Notifies main task when done
static SemaphoreHandle_t chopstick[NUM_TASKS];

// The only task: eating
void eat(void *parameters) {
    int num;
    int num_plus_1;
    char buf[50];

    // Copy parameter and increment semaphore count
    num = *(int *)parameters;
    xSemaphoreGive(bin_sem);
    
    // Solution:
    // Force tasks to take lower number mutex first
    if(num < (num+1)%NUM_TASKS){
        num_plus_1 = (num+1)%NUM_TASKS;
    }
    else{
        num_plus_1 = num;
        num = (num+1)%NUM_TASKS;
    }

    // Take left chopstick
    xSemaphoreTake(chopstick[num], portMAX_DELAY);
    sprintf(buf, "Philosopher %i took chopstick %i", num, num);
    Serial.println(buf);

    // Add some delay to force deadlock
    vTaskDelay(1 / portTICK_PERIOD_MS);

    // Take right chopstick
    xSemaphoreTake(chopstick[num_plus_1], portMAX_DELAY);
    sprintf(buf, "Philosopher %i took chopstick %i", num, (num+1)%NUM_TASKS);
    Serial.println(buf);

    // Do some eating
    sprintf(buf, "Philosopher %i is eating", num);
    Serial.println(buf);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    // Put down right chopstick
    xSemaphoreGive(chopstick[(num+1)%NUM_TASKS]);
    sprintf(buf, "Philosopher %i returned chopstick %i", num, (num+1)%NUM_TASKS);
    Serial.println(buf);

    // Put down left chopstick
    xSemaphoreGive(chopstick[num]);
    sprintf(buf, "Philosopher %i returned chopstick %i", num, num);
    Serial.println(buf);

    // Notify main task and delete self
    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);
}

void setup() {
    char task_name[20];

    // Configure Serial
    Serial.begin(115200);

    // Wait a moment to start (so we don't miss Serial output)
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.println();
    Serial.println("---FreeRTOS Dining Philosophers problem---");

    // Create kernel objects before starting tasks
    bin_sem = xSemaphoreCreateBinary();
    done_sem = xSemaphoreCreateCounting(NUM_TASKS, 0);
    for (int i = 0; i < NUM_TASKS; i++) {
        chopstick[i] = xSemaphoreCreateMutex();
    }

    // Have the philosphers start eating
    for (int i = 0; i < NUM_TASKS; i++) {
        sprintf(task_name, "Philosopher %i", i);
        xTaskCreate(eat,
                    task_name,
                    TASK_STACK_SIZE,
                    (void *)&i,
                    1,
                    NULL);
        xSemaphoreTake(bin_sem, portMAX_DELAY);
    }


    // Wait until all the philosophers are done
    for (int i = 0; i < NUM_TASKS; i++) {
        xSemaphoreTake(done_sem, portMAX_DELAY);
    }

    // Say that we made it through without deadlock
    Serial.println("Done! No deadlock occurred!");
}

void loop() {}