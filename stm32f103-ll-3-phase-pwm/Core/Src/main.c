/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
#define AutoReload 256
#define sinus_points 255
#define minimal_amplitude 3
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

/* USER CODE BEGIN PV */

float PI = 3.14;
uint16_t sin_table_a[sinus_points];
uint16_t sin_table_b[sinus_points];
uint16_t sin_table_c[sinus_points];
float delay_time = 2000.0; // set delay for begin minimal speed
float prescaler = 200.0; // set delay for begin minimal speed

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void sin_init(uint16_t i);

void sin_init(uint16_t i)
{
  sin_table_c[(i+(sinus_points/3*1)) & (sinus_points-1)]=
  sin_table_b[(i+(sinus_points/3*2)) & (sinus_points-1)]=
  sin_table_a[(i+(sinus_points/3*3)) & (sinus_points-1)]=
  minimal_amplitude+(uint16_t)((sin((float) i*(2*PI/sinus_points))+1)*(sinus_points/2-1));
}

void delay_cycle(volatile uint32_t cycle_count)
{
  while(cycle_count--)
  {
    __ASM volatile ("NOP");
  }
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/

  /** NOJTAG: JTAG-DP Disabled and SW-DP Enabled
  */
  LL_GPIO_AF_Remap_SWJ_NOJTAG();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

  TIM1->ARR = AutoReload; // set auto-reload register

  TIM1->CCER |= TIM_CCER_CC1E;  // LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1);   //starts PWM on CH1 pin
  TIM1->CCER |= TIM_CCER_CC2E;  // LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2);   //starts PWM on CH2 pin
  TIM1->CCER |= TIM_CCER_CC3E;  // LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3);   //starts PWM on CH3 pin
  TIM1->CCER |= TIM_CCER_CC1NE; // LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1N);  //starts PWM on CH1N pin
  TIM1->CCER |= TIM_CCER_CC2NE; // LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2N);  //starts PWM on CH2N pin
  TIM1->CCER |= TIM_CCER_CC3NE; // LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3N);  //starts PWM on CH3N pin

  TIM1->CR1 = TIM_CR1_CEN; //  LL_TIM_EnableCounter(TIM1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  for (uint32_t i=0; i<sinus_points; i++)
  {
    sin_init(i);
  }

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

   for(uint32_t i=0;i<=sinus_points;i++)
   {
     delay_time = delay_time - 0.2;
     prescaler = prescaler - 0.02;
     if(delay_time < sinus_points) delay_time = sinus_points;

     if(delay_time <= sinus_points)
     {
       TIM1->CCR1=(sin_table_a[i & (sinus_points-1)]);
       TIM1->CCR2=(sin_table_b[i & (sinus_points-1)]);
       TIM1->CCR3=(sin_table_c[i & (sinus_points-1)]);
       delay_cycle(delay_time);
     }

     /*
      * Reduction of effective amplitude
      * at low spindle rpm
      */
     if(delay_time > sinus_points)
     {
       TIM1->CCR1=(sin_table_a[i & (sinus_points-1)]-(delay_time/10));
       TIM1->CCR2=(sin_table_b[i & (sinus_points-1)]-(delay_time/10));
       TIM1->CCR3=(sin_table_c[i & (sinus_points-1)]-(delay_time/10));
       delay_cycle(delay_time);
       TIM1->PSC = (uint16_t)prescaler; //  LL_TIM_SetPrescaler(TIM1, (uint16_t)prescaler);
     }
   }
 }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_2)
  {
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_Init1msTick(72000000);
  LL_SetSystemCoreClock(72000000);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
