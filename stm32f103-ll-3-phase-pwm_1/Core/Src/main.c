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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

float PI = 3.14;
float delay_time = 2000.0; // set delay for begin minimal speed
float prescaler = 80.0; // set delay for begin minimal speed
volatile uint8_t index_a_sin = 0; // phase index A in the sine table
volatile uint8_t index_b_sin = 85; // phase index B in the sine table
volatile uint8_t index_c_sin = 43; // phase index C in the sine table
volatile uint8_t temp_sin; // service variable
volatile uint8_t temp_a; // temporary value storage for phase A
volatile uint8_t temp_b; // temporary value storage for phase B
volatile uint8_t temp_c; // temporary value storage for phase C
volatile uint8_t pol_a = 1; // polarity phase A "1" - it is more 126
volatile uint8_t pol_b = 1; // polarity phase B "0" - it is less 128
volatile uint8_t pol_c = 0; // polarity phase C
volatile uint8_t index_bc_calc = 1; // "1" - calculate indexes for phases B & C
volatile uint8_t inc_ind = 1; // index step in the sine table
volatile float amp_sin = 0; // sinus amplitude 255 the max.

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
  TIM1->CCER |= TIM_CCER_CC1E;  // LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1);   // start pwm on CH1 pin
  TIM1->CCER |= TIM_CCER_CC2E;  // LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2);   // start pwm on CH2 pin
  TIM1->CCER |= TIM_CCER_CC3E;  // LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3);   // start pwm on CH3 pin
  TIM1->CCER |= TIM_CCER_CC1NE; // LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1N);  // start pwm on CH1N pin
  TIM1->CCER |= TIM_CCER_CC2NE; // LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2N);  // start pwm on CH2N pin
  TIM1->CCER |= TIM_CCER_CC3NE; // LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3N);  // start pwm on CH3N pin
  TIM1->CR1 = TIM_CR1_CEN; //  LL_TIM_EnableCounter(TIM1);
}

void spin(void)
{
  for(int32_t i=0;i<sinus_points;i++)
  {
    prescaler -= 0.01; // increasing timer frequency
    if(prescaler < 40) prescaler = 40; // limit prescaler
    // --
    delay_time -= 0.1; // reduction of the period (spindle acceleration)
    if(delay_time < sinus_points) delay_time = sinus_points; // limit delay_time
    // --
    amp_sin += 0.025; // прирост амплитуды
    if(amp_sin>254) amp_sin=255; // limt amplitude
    // --
    // ============================================
    // calculating for the phase A
    // index_a_sin += inc_ind;
    // ============================================
    // switching phase Polarity А and calculate index
    if (index_a_sin > 127)
    {
      index_a_sin -= 128;
      index_bc_calc = 0; // no calculate indexes for phases B & C
      if (pol_a)
      {
        // switching phase Polarity
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
    // checking indexes for phases B and C and polarity
    if (index_bc_calc)
    {
      // ============================================
      // calculating the phase index B and polarity
      if (index_b_sin > 127)
      {
        index_b_sin -= 128;
        if (pol_b)
        {
          // switching phase polarity B
          pol_b = 0;
        }
        else
        {
          pol_b = 1;
        }
      }
      // ============================================
      // calculating the phase index С and polarity
      if (index_c_sin > 127)
      {
        index_c_sin -= 128;
        if (pol_c)
        {
          // switching phase polarity B
          pol_c = 0;
        }
        else
        {
          pol_c = 1;
        }
      }
    }
    // ============================================
    // here the indices of all fach are known and polarities too.
    // ============================================
    // calculation of phase amplitude А
    // multiply the value from the table by coefficient amp_sin and we take the most significant byte.
    temp_sin = (((int16_t)amp_sin * (int16_t)sinus[index_a_sin])) >> 8;
    // --
    if (pol_a)
    {
      // if positive half wave phase
      temp_a = temp_sin + 127;
    }
    else
    {
      // if negative half wave phase
      temp_a = 127 - temp_sin;
    }
    // ============================================
    // calculation of phase amplitude B
    // multiply the value from the table by coefficient amp_sin and we take the most significant byte.
    temp_sin = (((int16_t)amp_sin * (int16_t)sinus[index_b_sin])) >> 8;
    // --
    if (pol_b)
    {
      // if positive half wave phase
      temp_b = temp_sin + 127;
    }
    else
    {
      // if negative half wave phase
      temp_b = 127 - temp_sin;
    }
    // ========================================================
    // calculation of phase amplitude C
    // multiply the value from the table by coefficient amp_sin and we take the most significant byte.
    temp_sin = (((int16_t)amp_sin * (int16_t)sinus[index_c_sin])) >> 8;
    // --
    if (pol_c)
    {
      // if positive half wave phase
      temp_c = temp_sin + 127;
    }
    else
    {
      // if negative half wave phase
      temp_c = 127 - temp_sin;
    }
    // =========================================================
    // write pwm values in registers capture/compare
    TIM1->CCR1 = temp_a; // pwm CH1 phase_A
    TIM1->CCR2 = temp_b; // pwm CH2 phase_B
    TIM1->CCR3 = temp_c; // pwm CH3 phase_C
    // =========================================================
    // index increment
    index_a_sin += inc_ind; // next index phase_A
    index_b_sin += inc_ind; // next index phase_B
    index_c_sin += inc_ind; // next index phase_C
    index_bc_calc = 1; // calculate indexes for phases B & C
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
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
