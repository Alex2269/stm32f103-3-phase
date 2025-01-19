#include "timers.h"


uint32_t LL_Pin[16] =
{
  LL_GPIO_PIN_0, LL_GPIO_PIN_1, LL_GPIO_PIN_2, LL_GPIO_PIN_3,
  LL_GPIO_PIN_4, LL_GPIO_PIN_5, LL_GPIO_PIN_6, LL_GPIO_PIN_7,
  LL_GPIO_PIN_8, LL_GPIO_PIN_9, LL_GPIO_PIN_10, LL_GPIO_PIN_11,
  LL_GPIO_PIN_12, LL_GPIO_PIN_13, LL_GPIO_PIN_14, LL_GPIO_PIN_15
};

void GPIO_TIM1_Init(void)
{
  /**
  *
  * ┌───────┬───────────┬──────────┐
  * │ PA8   │ TIM1_CH1  │ Phase 1  │
  * ├───────┼───────────┼──────────┤
  * │ PA9   │ TIM1_CH2  │ Phase 2  │
  * ├───────┼───────────┼──────────┤
  * │ PA10  │ TIM1_CH3  │ Phase 3  │
  * ├───────┼───────────┼──────────┤
  * │ PB13  │ TIM1_CH1N │ Phase 1N │
  * ├───────┼───────────┼──────────┤
  * │ PB14  │ TIM1_CH2N │ Phase 2N │
  * ├───────┼───────────┼──────────┤
  * │ PB15  │ TIM1_CH3N │ Phase 3N │
  * └───────┴───────────┴──────────┘
  *
  **/

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOD);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);

  for(uint8_t i=8;i<=10;i++) { // set pins to input PA8,PA9,PA10
    LL_GPIO_SetPinMode(GPIOA,  LL_Pin[i], LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinSpeed(GPIOA,  LL_Pin[i], LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinOutputType(GPIOA,  LL_Pin[i], LL_GPIO_OUTPUT_PUSHPULL);
  }

  for(uint8_t i=13;i<=15;i++) { // set pins to pwm output PB13,PB14,PB15
    LL_GPIO_SetPinMode(GPIOB,  LL_Pin[i], LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinSpeed(GPIOB,  LL_Pin[i], LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinOutputType(GPIOB,  LL_Pin[i], LL_GPIO_OUTPUT_PUSHPULL);
  }
}

uint32_t LL_TIM_CHANNEL[7] =
{
  0, LL_TIM_CHANNEL_CH1,LL_TIM_CHANNEL_CH2,LL_TIM_CHANNEL_CH3,
  LL_TIM_CHANNEL_CH1N,LL_TIM_CHANNEL_CH2N,LL_TIM_CHANNEL_CH3N
};

void timer_pwm_init(void)
{
  #define Prescaler 4
  #define AutoReload 128
  #define RepetitionCounter 0x00000000U
  #define DeadTime 16
  TIM1->PSC = Prescaler; // LL_TIM_SetPrescaler(TIM1, Prescaler);
  LL_TIM_SetCounterMode(TIM1, LL_TIM_COUNTERMODE_UP);
  TIM1->ARR = AutoReload; // LL_TIM_SetAutoReload(TIM1, AutoReload);
  LL_TIM_SetClockDivision(TIM1, LL_TIM_CLOCKDIVISION_DIV1);
  TIM1->RCR = RepetitionCounter; // LL_TIM_SetRepetitionCounter(TIM1, RepetitionCounter)

  // SET_BIT(TIM1->CR1, TIM_CR1_ARPE); // LL_TIM_EnableARRPreload(TIM_TypeDef *TIMx);
  CLEAR_BIT(TIM1->CR1, TIM_CR1_ARPE); // LL_TIM_DisableARRPreload(TIM_TypeDef *TIMx);

  LL_TIM_SetOffStates(TIM1, LL_TIM_OSSI_DISABLE, LL_TIM_OSSR_DISABLE);
  LL_TIM_CC_SetLockLevel(TIM1, LL_TIM_LOCKLEVEL_OFF);
  LL_TIM_OC_SetDeadTime(TIM1, DeadTime);
  LL_TIM_DisableBRK(TIM1);
  LL_TIM_ConfigBRK(TIM1, LL_TIM_BREAK_POLARITY_HIGH);
  LL_TIM_EnableAutomaticOutput(TIM1);

  for(uint8_t i=1;i<=6;i++) {
    LL_TIM_OC_SetMode(TIM1, LL_TIM_CHANNEL[i], LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetPolarity(TIM1, LL_TIM_CHANNEL[i], LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_OC_SetIdleState(TIM1, LL_TIM_CHANNEL[i], LL_TIM_OCIDLESTATE_LOW);
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL[i]);
  }

  TIM1->CR1 = TIM_CR1_CEN; // LL_TIM_EnableCounter(TIM1);
}
