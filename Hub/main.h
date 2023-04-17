/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Row_1_Pin GPIO_PIN_2
#define Row_1_GPIO_Port GPIOE
#define Row_1_EXTI_IRQn EXTI2_IRQn
#define Col_1_Pin GPIO_PIN_3
#define Col_1_GPIO_Port GPIOE
#define Row_2_Pin GPIO_PIN_4
#define Row_2_GPIO_Port GPIOE
#define Row_2_EXTI_IRQn EXTI4_IRQn
#define Row_3_Pin GPIO_PIN_5
#define Row_3_GPIO_Port GPIOE
#define Row_3_EXTI_IRQn EXTI9_5_IRQn
#define Row_4_Pin GPIO_PIN_6
#define Row_4_GPIO_Port GPIOE
#define Row_4_EXTI_IRQn EXTI9_5_IRQn
#define Col_3_Pin GPIO_PIN_7
#define Col_3_GPIO_Port GPIOF
#define Col_2_Pin GPIO_PIN_8
#define Col_2_GPIO_Port GPIOF
#define Col_4_Pin GPIO_PIN_9
#define Col_4_GPIO_Port GPIOF
#define RS_Pin GPIO_PIN_12
#define RS_GPIO_Port GPIOF
#define RST_Pin GPIO_PIN_13
#define RST_GPIO_Port GPIOF
#define LED_Pin GPIO_PIN_14
#define LED_GPIO_Port GPIOF

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
