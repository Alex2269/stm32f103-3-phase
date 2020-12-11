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
#include "sin256.h"

#define AutoReload 256
#define sinus_points 255

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
float delay_time = 2000.0; // set delay for begin minimal speed
float prescaler = 20.0; // set delay for begin minimal speed
//
//
volatile uint8_t index_a_sin = 0; // Индекс фазы A в таблице синуса
volatile uint8_t index_b_sin = 85; // Индекс фазы B в таблице синуса
volatile uint8_t index_c_sin = 43; // Индекс фазы C в таблице синуса
volatile uint8_t temp_sin; // служебная переменная 1 байт
volatile uint8_t temp_a; // временное хранение значения фазы A
volatile uint8_t temp_b; // временное хранение значения фазы B
volatile uint8_t temp_c; // временное хранение значения фазы C
volatile uint8_t pol_a = 1; // полярность фазы A "1" - больше 126
volatile uint8_t pol_b = 1; // полярность фазы B "0" - меньше 128
volatile uint8_t pol_c = 0; // полярность фазы C
volatile uint8_t index_bc_calc = 1; // "1" - вычислять индексы для фаз В и С
volatile uint8_t inc_ind = 1; // Шаг индекса в таблице синуса
volatile float amp_sin = 0; // амплитуда синуса. 255 это максимум.

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void delay_cycle(volatile uint32_t cycle_count)
{
  while(cycle_count--)
  {
    __asm__ volatile ("nop");
  }
}

void timer_pwm_init(void)
{

  TIM1->ARR = AutoReload; // set auto-reload register

  TIM1->CCER |= TIM_CCER_CC1E;  // LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1);   // start PWM on CH1 pin
  TIM1->CCER |= TIM_CCER_CC2E;  // LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2);   // start PWM on CH2 pin
  TIM1->CCER |= TIM_CCER_CC3E;  // LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3);   // start PWM on CH3 pin
  TIM1->CCER |= TIM_CCER_CC1NE; // LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1N);  // start PWM on CH1N pin
  TIM1->CCER |= TIM_CCER_CC2NE; // LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2N);  // start PWM on CH2N pin
  TIM1->CCER |= TIM_CCER_CC3NE; // LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3N);  // start PWM on CH3N pin

  TIM1->CR1 = TIM_CR1_CEN; //  LL_TIM_EnableCounter(TIM1);

}

void spin(void)
{
  for(int32_t i=0;i<sinus_points;i++)
  {
    prescaler -= 0.01; // повышение частоты таймера
    if(prescaler < 40) prescaler = 40; // limit prescaler
    // --
    delay_time -= 0.1; // уменьшение периода (раскрутка)
    if(delay_time < sinus_points) delay_time = sinus_points; // limit delay_time
    // --
    amp_sin += 0.025; // прирост амплитуды
    if(amp_sin>254) amp_sin=255; // limt amplitude
    // --
    // ============================================
    // Вычисления для Фазы А
    // index_a_sin += inc_ind;
    // ============================================
    // Перекл. полярн. Фазы А и вычисление индекса
    if (index_a_sin > 127)
    {
      index_a_sin -= 128;
      index_bc_calc = 0; // не вычислять индексы для фаз В и С
      if (pol_a)
      {
        // Переключение полярности Фаз
        pol_a = 0; pol_b = 0; pol_c = 1;
        index_b_sin = index_a_sin + 85;
        index_c_sin = index_a_sin + 43;
      }
      else
      {
        pol_a = 1; pol_b = 1; pol_c = 0;
        index_b_sin = index_a_sin + 85;
        index_c_sin = index_a_sin + 43;
      }
    }
    // ============================================
    // Проверка индексов для Фаз B и C и полярности
    if (index_bc_calc)
    {
      // ============================================
      // Вычисл. индекса Фазы B и полярности
      if (index_b_sin > 127)
      {
        index_b_sin -= 128;
        if (pol_b)
        {
          // Переключение полярности Фазы B
          pol_b = 0;
        }
        else
        {
          pol_b = 1;
        }
      }
      // ============================================
      // Вычисл. индекса Фазы С и полярности
      if (index_c_sin > 127)
      {
        index_c_sin -= 128;
        if (pol_c)
        {
          // Переключение полярности Фазы B
          pol_c = 0;
        }
        else
        {
          pol_c = 1;
        }
      }
    }
    // ============================================
    // ТУТ индексы всех фах известны и полярности тоже.
    // ============================================
    // Вычисление амплитуды Фазы А
    // Умножаем значение из таблицы на коэф. amp_sin и берем старший байт.
    temp_sin = (((int16_t)amp_sin * (int16_t)sinus[index_a_sin])) >> 8;
    // --
    if (pol_a)
    {
      // если положительная полуволна Фазы
      temp_a = temp_sin + 127; // PWM CH1 Phase_A
    }
    else
    {
      // если отрицательная полуволна Фазы
      temp_a = 127 - temp_sin;
    }
    // ============================================
    // Вычисление амплитуды Фазы B
    // Умножаем значение из таблицы на коэф. amp_sin и берем старший байт.
    temp_sin = (((int16_t)amp_sin * (int16_t)sinus[index_b_sin])) >> 8;
    // --
    if (pol_b)
    {
      // если положительная полуволна Фазы
      temp_b = temp_sin + 127; // PWM CH2 Phase_B
    }
    else
    {
      // если отрицательная полуволна Фазы
      temp_b = 127 - temp_sin;
    }
    // ========================================================
    // Вычисление амплитуды Фазы C
    // Умножаем значение из таблицы на коэф. amp_sin и берем старший байт.
    temp_sin = (((int16_t)amp_sin * (int16_t)sinus[index_c_sin])) >> 8;
    // --
    if (pol_c)
    {
      // если положительная полуволна Фазы
      temp_c = temp_sin + 127; // PWM CH3 Phase_C
    }
    else
    {
      // если отрицательная полуволна Фазы
      temp_c = 127 - temp_sin;
    }
    // =========================================================
    // Запись значений ШИМ в регистры Capture/compare
    TIM1->CCR1 = temp_a; // PWM CH1 Phase_A
    TIM1->CCR2 = temp_b; // PWM CH2 Phase_B
    TIM1->CCR3 = temp_c; // PWM CH3 Phase_C
    // =========================================================
    // Инкремент индексов
    index_a_sin += inc_ind; // следующий индекс PWM CH1 Phase_A
    index_b_sin += inc_ind; // следующий индекс PWM CH2 Phase_B
    index_c_sin += inc_ind; // следующий индекс PWM CH3 Phase_C
    index_bc_calc = 1; // вычислять индексы для фаз В и С
    // --
    delay_cycle(delay_time);
    TIM1->PSC = (uint16_t)prescaler; // LL_TIM_SetPrescaler(TIM1, (uint16_t)prescaler); // adjust frequency
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
  timer_pwm_init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    spin();
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
