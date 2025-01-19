#include "spindle.h"

#define sinus_points 255

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
