# 1 "C:\\rtos_esp32c6\\rtos_esp32c6.ino"
/**

 * ESP32 Dining Philosophers using Dijkstra hierarchy/priority assignment to solve livelock

 * 

 * The classic "Dining Philosophers" problem in FreeRTOS form.

 * 

 * Based on http://www.cs.virginia.edu/luther/COA2/S2019/pa05-dp.html

 * 

 * Date: February 8, 2021

 * Author: Shawn Hymel

 * License: 0BSD

 */
# 13 "C:\\rtos_esp32c6\\rtos_esp32c6.ino"
// Settings
enum { NUM_TASKS = 5 }; // Number of tasks (philosophers)
enum { TASK_STACK_SIZE = 2048 }; // Bytes in ESP32, words in vanilla FreeRTOS

// Globals
static SemaphoreHandle_t bin_sem; // Wait for parameters to be read
static SemaphoreHandle_t done_sem; // Notifies main task when done
static SemaphoreHandle_t chopstick[NUM_TASKS];

//*****************************************************************************
// Tasks

// The only task: eating
void eat(void *parameters) {

  int num;
  int num_plus_1;
  char buf[50];

  // Copy parameter and increment semaphore count
  num = *(int *)parameters;
  xQueueGenericSend( ( QueueHandle_t ) ( bin_sem ), 
# 34 "C:\\rtos_esp32c6\\rtos_esp32c6.ino" 3 4
 __null
# 34 "C:\\rtos_esp32c6\\rtos_esp32c6.ino"
 , ( ( TickType_t ) 0U ), ( ( BaseType_t ) 0 ) );

  if(num < (num+1)%NUM_TASKS){
    num_plus_1 = (num+1)%NUM_TASKS;
  }
  else{
    num_plus_1 = num;
    num = (num+1)%NUM_TASKS;
  }

  // Take left chopstick
  xQueueSemaphoreTake( ( chopstick[num] ), ( (TickType_t) 0xffffffffUL ) );
  sprintf(buf, "Philosopher %i took chopstick %i", num, num);
  HWCDCSerial.println(buf);

  // Add some delay to force deadlock
  vTaskDelay(1 / ((TickType_t) (1000 / 
# 50 "C:\\rtos_esp32c6\\rtos_esp32c6.ino" 3 4
                1000
# 50 "C:\\rtos_esp32c6\\rtos_esp32c6.ino"
                )));

  // Take right chopstick
  xQueueSemaphoreTake( ( chopstick[num_plus_1] ), ( (TickType_t) 0xffffffffUL ) );
  sprintf(buf, "Philosopher %i took chopstick %i", num, (num+1)%NUM_TASKS);
  HWCDCSerial.println(buf);

  // Do some eating
  sprintf(buf, "Philosopher %i is eating", num);
  HWCDCSerial.println(buf);
  vTaskDelay(10 / ((TickType_t) (1000 / 
# 60 "C:\\rtos_esp32c6\\rtos_esp32c6.ino" 3 4
                 1000
# 60 "C:\\rtos_esp32c6\\rtos_esp32c6.ino"
                 )));

  // Put down right chopstick
  xQueueGenericSend( ( QueueHandle_t ) ( chopstick[(num+1)%NUM_TASKS] ), 
# 63 "C:\\rtos_esp32c6\\rtos_esp32c6.ino" 3 4
 __null
# 63 "C:\\rtos_esp32c6\\rtos_esp32c6.ino"
 , ( ( TickType_t ) 0U ), ( ( BaseType_t ) 0 ) );
  sprintf(buf, "Philosopher %i returned chopstick %i", num, (num+1)%NUM_TASKS);
  HWCDCSerial.println(buf);

  // Put down left chopstick
  xQueueGenericSend( ( QueueHandle_t ) ( chopstick[num] ), 
# 68 "C:\\rtos_esp32c6\\rtos_esp32c6.ino" 3 4
 __null
# 68 "C:\\rtos_esp32c6\\rtos_esp32c6.ino"
 , ( ( TickType_t ) 0U ), ( ( BaseType_t ) 0 ) );
  sprintf(buf, "Philosopher %i returned chopstick %i", num, num);
  HWCDCSerial.println(buf);

  // Notify main task and delete self
  xQueueGenericSend( ( QueueHandle_t ) ( done_sem ), 
# 73 "C:\\rtos_esp32c6\\rtos_esp32c6.ino" 3 4
 __null
# 73 "C:\\rtos_esp32c6\\rtos_esp32c6.ino"
 , ( ( TickType_t ) 0U ), ( ( BaseType_t ) 0 ) );
  vTaskDelete(
# 74 "C:\\rtos_esp32c6\\rtos_esp32c6.ino" 3 4
             __null
# 74 "C:\\rtos_esp32c6\\rtos_esp32c6.ino"
                 );
}

//*****************************************************************************
// Main (runs as its own task with priority 1 on core 1)

void setup() {

  char task_name[20];

  // Configure Serial
  HWCDCSerial.begin(115200);

  // Wait a moment to start (so we don't miss Serial output)
  vTaskDelay(1000 / ((TickType_t) (1000 / 
# 88 "C:\\rtos_esp32c6\\rtos_esp32c6.ino" 3 4
                   1000
# 88 "C:\\rtos_esp32c6\\rtos_esp32c6.ino"
                   )));
  HWCDCSerial.println();
  HWCDCSerial.println("---FreeRTOS Dining Philosophers Challenge---");

  // Create kernel objects before starting tasks
  bin_sem = xQueueGenericCreate( ( UBaseType_t ) 1, ( ( uint8_t ) 0U ), ( ( uint8_t ) 3U ) );
  done_sem = xQueueCreateCountingSemaphore( ( NUM_TASKS ), ( 0 ) );
  for (int i = 0; i < NUM_TASKS; i++) {
    chopstick[i] = xQueueCreateMutex( ( ( uint8_t ) 1U ) );
  }

  // Have the philosphers start eating
  for (int i = 0; i < NUM_TASKS; i++) {
    sprintf(task_name, "Philosopher %i", i);
    xTaskCreate(eat,
                task_name,
                TASK_STACK_SIZE,
                (void *)&i,
                1,
                
# 107 "C:\\rtos_esp32c6\\rtos_esp32c6.ino" 3 4
               __null
# 107 "C:\\rtos_esp32c6\\rtos_esp32c6.ino"
                   );
    xQueueSemaphoreTake( ( bin_sem ), ( (TickType_t) 0xffffffffUL ) );
  }


  // Wait until all the philosophers are done
  for (int i = 0; i < NUM_TASKS; i++) {
    xQueueSemaphoreTake( ( done_sem ), ( (TickType_t) 0xffffffffUL ) );
  }

  // Say that we made it through without deadlock
  HWCDCSerial.println("Done! No deadlock occurred!");
}

void loop() {
  // Do nothing in this task
}
