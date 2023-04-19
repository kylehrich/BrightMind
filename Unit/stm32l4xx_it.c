/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32l4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "main.h"
#include "stm32l4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Unit.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint32_t micState = 0;
uint32_t watchDawgCount = 0;
#define DEBOUNCE_DELAY 100

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
//TIM_HandleTypeDef htim4;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern ADC_HandleTypeDef hadc1;
/* USER CODE BEGIN EV */
extern TIM_HandleTypeDef htim4;
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32L4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles ADC1 global interrupt.
  */
void ADC1_IRQHandler(void)
{
  /* USER CODE BEGIN ADC1_IRQn 0 */
	//HAL_Delay(100);
	//HAL_NVIC_DisableIRQ(ADC1_IRQn);
  /* USER CODE END ADC1_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc1);
  /* USER CODE BEGIN ADC1_IRQn 1 */
  watchDawgCount++;
  static uint32_t lastInterruptTime = 0;
  uint32_t curTime = HAL_GetTick();

  /*
  if (watchDawgCount < 5)
  {
	  watchDawgCount = 5;
  }
  */
  if ((curTime - lastInterruptTime) >= DEBOUNCE_DELAY)
  {
	  lastInterruptTime = curTime;

  if (micState == 0) /*(watchDawgCount == 1)*/
  {
	  micState = 1;
	  //HAL_Delay(100);
	  timerStart();
	  //NVIC_EnableIRQ(ADC1_IRQn);
  }
  else if (micState == 1 && TIM4->CNT <= 50000) /*(watchDawgCount == 2 && TIM4->CNT <= 50000)*/
  {
	  micState = 0;
	  //watchDawgCount = 0;
	  toggle_switch_interrupt();
	  //NVIC_EnableIRQ(ADC1_IRQn);
  }
  if (TIM4->CNT > 50000)
  {
	  micState = 0;
	  watchDawgCount = 0;
	  TIM4->CNT = 0;
	  //NVIC_EnableIRQ(ADC1_IRQn);
	  // RESET TIMER!!!
  }
  }
  //HAL_NVIC_EnableIRQ(ADC1_IRQn);

  /*
  uint32_t ADC_VAL = 0;
  double ADC_V = 0.0;
  uint32_t state = 1;
  uint32_t next_state = 0;
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
  while (1)
  {

  HAL_ADC_Start(&hadc1);//start conversion
  	  HAL_ADC_PollForConversion(&hadc1, 0xFFFFFFFF);//wait for conversion to finish
  	  ADC_VAL = HAL_ADC_GetValue(&hadc1);//retrieve value

  	  ADC_V = (ADC_VAL * 3.3 / (1 << 12));
  	  int i = 0;
  	  double amplitude = fabs(ADC_V - (3.3/2));

  	  if(amplitude > 1){
  		  //printf("Clap!\r\n");
  		  if(state == 0) next_state = 1;
  		  if(state == 2) next_state = 3;
  	  }
  	  if(state == 2 && TIM4->CNT > 50000){
  		  next_state = 0;
  		  //printf("No second clap detected\r\n");
  	  }
  	  if(state == 1){
  		  HAL_Delay(100);
  		  next_state = 2;
  		  TIM4->CNT = 0;
  	  }
  	  if(state == 3){
  		  HAL_Delay(2000);
  		  //printf("Toggle switch!\r\n");
  		  toggle_switch_interrupt(); // flip the switch
  		  next_state = 0;
  		  break; //break loop to end interrupt
  	  }
  	  state = next_state;
  }
	*/

  /* USER CODE END ADC1_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */
  toggle_switch_interrupt();
  /* USER CODE END EXTI15_10_IRQn 1 */
}

/* USER CODE BEGIN 1 */

void timerStart()
{
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
}

/* USER CODE END 1 */
