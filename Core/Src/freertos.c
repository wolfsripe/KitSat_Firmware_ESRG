/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "parser.h"
#include "commands.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* External declarations */

/* USER CODE END Variables */
osThreadId UartParserTaskHandle;
osThreadId defaultTask0Handle;
osThreadId TaskDispatxherHandle;
osMessageQId uartRxQueueHandle;
osMessageQId commandExecQueueHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Uart_Parser_Task(void const * argument);
void defaultTask(void const * argument);
void Dispatcher_Task(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of uartRxQueue */
  osMessageQDef(uartRxQueue, 10, UART_Command_t);
  uartRxQueueHandle = osMessageCreate(osMessageQ(uartRxQueue), NULL);

  /* definition and creation of commandExecQueue */
  osMessageQDef(commandExecQueue, 10, uint16_t);
  commandExecQueueHandle = osMessageCreate(osMessageQ(commandExecQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of UartParserTask */
  osThreadDef(UartParserTask, Uart_Parser_Task, osPriorityNormal, 0, 1024);
  UartParserTaskHandle = osThreadCreate(osThread(UartParserTask), NULL);

  /* definition and creation of defaultTask0 */
  osThreadDef(defaultTask0, defaultTask, osPriorityIdle, 0, 128);
  defaultTask0Handle = osThreadCreate(osThread(defaultTask0), NULL);

  /* definition and creation of TaskDispatxher */
  osThreadDef(TaskDispatxher, Dispatcher_Task, osPriorityIdle, 0, 1024);
  TaskDispatxherHandle = osThreadCreate(osThread(TaskDispatxher), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Uart_Parser_Task */
/**
  * @brief  Function implementing the UartParserTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Uart_Parser_Task */
void Uart_Parser_Task(void const * argument)
{
  /* USER CODE BEGIN Uart_Parser_Task */
	char commandBuffer[MSG_BUFFER_SIZE];
	//HAL_UART_Receive_IT(&huart3, (uint8_t*)&rxBuffer, 1);
	HAL_UART_Receive_IT(&huart6, (uint8_t*)&rxBuffer, 1);
  /* Infinite loop */
  for(;;)
  {
	if(complete6) {
	  strncpy(commandBuffer, (char*)receivedData, MSG_BUFFER_SIZE);
	  complete6 = 0;
	  receivedIndex6 = 0;
	  if(validateCommand(commandBuffer)) {
		parseCommand(commandBuffer);
	  }
	}else{
		LoRa_Receive();
		strncpy(commandBuffer, (char*)receivedData, MSG_BUFFER_SIZE);
		receivedIndex6 = 0;
		if(validateCommand(commandBuffer)) {
			parseCommand(commandBuffer);
		}
	}
	osDelay(1);
  }
  /* USER CODE END Uart_Parser_Task */
}

/* USER CODE BEGIN Header_defaultTask */
/**
* @brief Function implementing the defaultTask0 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_defaultTask */
void defaultTask(void const * argument)
{
  /* USER CODE BEGIN defaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END defaultTask */
}

/* USER CODE BEGIN Header_Dispatcher_Task */
/**
* @brief Function implementing the ManagmentTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Dispatcher_Task */
void Dispatcher_Task(void const * argument)
{
  /* USER CODE BEGIN Dispatcher_Task */
	osEvent event;
	char *command;
  /* Infinite loop */
  for(;;)
  {
	  event = osMessageGet(commandExecQueueHandle, osWaitForever);
		  if(event.status == osEventMessage) {
			  command = (char *)event.value.v;

			  for(int i = 0; commandTable[i].cmd != NULL; i++) {
				  if(strncmp(command, commandTable[i].cmd, strlen(commandTable[i].cmd)) == 0) {
					  osThreadDef_t cmdTaskDef = {
						  .name = commandTable[i].cmd,
						  .pthread = commandTable[i].handler,
						  .tpriority = osPriorityNormal,
						  .instances = 1,
						  .stacksize = 256
					  };
					  osThreadCreate(&cmdTaskDef, NULL);
					  break;
				  }
			  }
		  }
    osDelay(1);
  }
  /* USER CODE END Dispatcher_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

