/* USER CODE BEGIN Includes */
#include "math.h"
#define sinus_points 255
#define minimal_amplitude 3
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

float PI = 3.14;
uint16_t sin_table_a[sinus_points];
uint16_t sin_table_b[sinus_points];
uint16_t sin_table_c[sinus_points];
float tmp = 0; // acceleration time
uint16_t speed = sinus_points/2+2; // set delay for begin minimal speed
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void sin_init(uint16_t i);

void sin_init(uint16_t i)
{
  sin_table_c[(i+(sinus_points/3*1)) & (sinus_points-1)]=
  sin_table_b[(i+(sinus_points/3*2)) & (sinus_points-1)]=
  sin_table_a[(i+(sinus_points/3*3)) & (sinus_points-1)]=
  minimal_amplitude+(uint16_t)((sin((float) i*(2*PI/sinus_points))+1)*(sinus_points/2-1));
}

void delay_cycle(uint32_t cycle_count)
{
  uint32_t count=0;
  for(count=0;count<cycle_count;count++)
  {
    __ASM volatile ("NOP");
  }
}

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
  uint32_t i;
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
  MX_TIM1_Init();

  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);    //starts PWM on CH1 pin
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);    //starts PWM on CH2 pin
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);    //starts PWM on CH3 pin

  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1); //starts PWM on CH1N pin
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2); //starts PWM on CH2N pin
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3); //starts PWM on CH3N pin
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
  tmp += 0.05; // acceleration
  if (tmp>=1) { tmp = 0; speed--; }
  if(speed<sinus_points/16) speed = sinus_points/16;

  for(i=0;i<=sinus_points;i++)
    {
      TIM1->CCR1=sin_table_a[i & (sinus_points-1)];
      TIM1->CCR2=sin_table_b[i & (sinus_points-1)];
      TIM1->CCR3=sin_table_c[i & (sinus_points-1)];
      for(uint16_t d=0;d<speed;d++)
      {
        delay_cycle(1);
      }
    }
  }
  /* USER CODE END 3 */

}
