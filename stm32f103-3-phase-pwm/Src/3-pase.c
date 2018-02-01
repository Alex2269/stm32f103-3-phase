#include "math.h"
#define sinus_points 1024
#define minimal_amplitude 3

float PI = 3.14;
uint16_t sin_table_a[sinus_points];
uint16_t sin_table_b[sinus_points];
uint16_t sin_table_c[sinus_points];
float tmp = 0; // delay for acceleration
uint16_t speed = 1024;

void sin_init(uint16_t i);

void sin_init(uint16_t i)
{
  sin_table_c[(i+(sinus_points/3*1)) & (sinus_points-1)]=
  sin_table_b[(i+(sinus_points/3*2)) & (sinus_points-1)]=
  sin_table_a[(i+(sinus_points/3*3)) & (sinus_points-1)]=
  minimal_amplitude+(uint16_t)((sin((float) i*(2*PI/sinus_points))+1)*(sinus_points/2-1));
}

int main(void)
{

  /* USER CODE BEGIN 1 */
  uint32_t i,d;
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();

  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  for (i=0; i<sinus_points; i++)
  {
    sin_init(i);
  }

  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
  tmp += 0.5; // acceleration
  if (tmp>=1) { tmp = 0; speed--; }
  if(speed<400) speed = 400;

  for(i=0;i<=sinus_points;i++)
    {
      TIM3->CCR1=sin_table_a[i & (sinus_points-1)];
      TIM3->CCR2=sin_table_b[i & (sinus_points-1)];
      TIM3->CCR3=sin_table_c[i & (sinus_points-1)];
      for(d=0;d<speed;d++)
      {
      }
    }
  }
  /* USER CODE END 3 */

}
