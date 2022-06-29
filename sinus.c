#define sinus_points 1023 // 127, 255, 511, etc.
#define sinus_value 128 // value: 8, 16, 32 etc.

float PI = 3.14159;

int16_t sinus_table_a[sinus_points];
int16_t sinus_table_b[sinus_points];
int16_t sinus_table_c[sinus_points];

void sinus_init(uint16_t i)
{
  sinus_table_c[(i+(sinus_points/3*1)) & (sinus_points)]=
  sinus_table_b[(i+(sinus_points/3*2)) & (sinus_points)]=
  sinus_table_a[(i+(sinus_points/3*3)) & (sinus_points)]=
  ((sin((float) i*(2*PI/sinus_value)))*(sinus_value/2));
}

static void sinus_fill(void)
{
  for (uint32_t i=0; i<sinus_points; i++)
  {
    sinus_init(i);
  }
}
