#ifndef _PICO_BUTTONS_H_
#define _PICO_BUTTONS_H_

#include "pico/stdlib.h"
#include "stepper_control.pio.h"

void timer_callbak(void);
void buttonDebauncer(uint8_t gpio_nr,
                   uint16_t rep_wait,
                   uint16_t rep_time,
                   void (* pushFunc)(void),
                   void (* repFunc)(void) );

void init_encoder(pio_sm_config *c, uint *offset, PIO pio, uint sm, uint e1, uint e2);


                                                 
#endif