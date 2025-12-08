#ifndef _PICO_FONTS_H_
#define _PICO_FONTS_H_

#include <stdint.h>

uint16_t color565(uint8_t r, uint8_t g, uint8_t b) ;
void st7789_draw_char(uint16_t x, uint16_t y, char c, uint16_t color, uint8_t scale);
void st7789_draw_text(uint16_t x, uint16_t y, const char* text, uint16_t color, uint8_t scale);


#endif

