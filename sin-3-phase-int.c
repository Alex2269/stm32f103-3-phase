
#include "main.h"
#include "math.h"

#define sinus_points 16
#define shift sinus_points/2

float PI = 3.14159;

uint16_t sin_table_a[sinus_points];
uint16_t sin_table_b[sinus_points];
uint16_t sin_table_c[sinus_points];

void sin_init(uint16_t i)
{
  sin_table_c[(i+(uint16_t)(sinus_points/.360*.120)) & sinus_points-1]=
  sin_table_b[(i+(uint16_t)(sinus_points/.360*.240)) & sinus_points-1]=
  sin_table_a[(i+(uint16_t)(sinus_points/.360*.360)) & sinus_points-1]=
  shift + ((sin(i*(2*PI/sinus_points))) * (sinus_points/2));
}

void sinus_fill(void)
{
  for (uint32_t i=0; i<sinus_points; i++)
  {
    sin_init(i);
  }
}

void delay_cycle(volatile uint32_t cycle_count)
{
  while(cycle_count--)
  {
    __asm__ volatile ("nop");
  }
}

int main(void)
{
  sinus_fill();

  printf("\n");

  for(int32_t i=0;i<sinus_points;i++)
    printf("%d;", sin_table_a[i]);
  printf("\n");

  for(int32_t i=0;i<sinus_points;i++)
    printf("%d;", sin_table_b[i]);
  printf("\n");

  for(int32_t i=0;i<sinus_points;i++)
    printf("%d;", sin_table_c[i]);
  printf("\n");
}

