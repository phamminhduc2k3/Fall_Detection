#ifndef TIMER_H
#define TIMER_H

#include "stm32f10x.h"

void timer_init(void);
void delay_us(unsigned long t);
void delay_ms(unsigned long t);

#endif // TIMER_H
