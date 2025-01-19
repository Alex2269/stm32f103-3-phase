/* bldc spindle */
#ifndef __SPINDLE_H
#define __SPINDLE_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f103xb.h"
#include "math.h"
#include "sin256.h"

void delay_cycle(volatile uint32_t cycle_count);
void spin(void);

#ifdef __cplusplus
}
#endif

#endif /* __SPINDLE_H */
