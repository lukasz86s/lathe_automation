#ifndef _PICO_BUTTONS_H_
#define _PICO_BUTTONS_H_

#include "pico/stdlib.h"

void timer_callbak(void);
void buttonDebauncer(uint8_t gpio_nr,
                   uint16_t rep_wait,
                   uint16_t rep_time,
                   void (* pushFunc)(void),
                   void (* repFunc)(void) );                   
                                                 
#endif