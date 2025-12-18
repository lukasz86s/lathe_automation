#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/st7789.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "stepper_control.pio.h"
#include "fonts.h"
#include "buttons.h"


PIO pio = pio0;
uint sm = 0;
uint pin_nr = 0;
PIO pio_encode = pio1;
uint sm_encode = 0;
uint pin_nr_encode = 18;



void add_square_prog( pio_sm_config *c, uint * offset)
{
    pio_sm_set_enabled(pio, sm, false);
    pio_sm_restart(pio, sm);
    pio_sm_clear_fifos(pio, sm);
    pio_remove_program(pio, &test1_program, *offset);
    *offset = pio_add_program(pio, &squarewave_program);
    *c = squarewave_program_get_default_config(*offset);
    // use of set pins for set instruction
    sm_config_set_set_pins(c, pin_nr, 1);
    //init state machine
    pio_sm_init(pio, sm , *offset, c);
    // change cloc for pio 150 MHz / 150 MHz = 1us
    pio_sm_set_clkdiv(pio, sm, 150.0f);
    pio_sm_clear_fifos(pio, sm);
    pio_sm_set_enabled(pio, sm, true);

}

void add_test_prog( pio_sm_config *c, uint *offset){
    pio_sm_set_enabled(pio, sm, false);
    pio_sm_restart(pio, sm);
    pio_sm_clear_fifos(pio, sm);
    pio_remove_program(pio, &squarewave_program, *offset);
    *offset = pio_add_program(pio, &test1_program);
    *c = test1_program_get_default_config(*offset);
    // use of set pins for set instruction
    sm_config_set_set_pins(c, pin_nr, 1);
    //init state machine
    pio_sm_init(pio, sm , *offset, c);
    // change cloc for pio 150 MHz / 150 MHz = 1us
    pio_sm_set_clkdiv(pio, sm, 150.0f);
    pio_sm_clear_fifos(pio, sm);
    pio_sm_set_enabled(pio, sm, true);
}


void print_test(void){
    printf("heello world\n");
}
bool dbounceCallback(repeating_timer_t*r){
    timer_callbak();
    return true;
}
void print_test_st7789(void){
        st7789_fill(color565(222, 24, 222));
        st7789_draw_text(8, 35, "button test", 0x0000, 3);
}

// lcd configuration
const struct st7789_config lcd_config = {
    .spi      = spi1,
    .gpio_din = 11,
    .gpio_clk = 10,
    .gpio_cs  = -1,
    .gpio_dc  = 13,
    .gpio_rst = 12,
    .gpio_bl  = 14,
};

const int lcd_width = 240;
const int lcd_height = 240;
#define DEBOUNCE_TIM 50
//#define BUTTON_L 16
typedef enum{
    BUTTON_L = 16,
    BUTTON_R= 17,
    NUM_BUTTONS = 2
}button_t;


int main()
{
    stdio_init_all();

    // // Initialise the Wi-Fi chip
    // if (cyw43_arch_init_with_country(CYW43_COUNTRY_POLAND)) {
    //     printf("Wi-Fi init failed\n");
    //     return -1;
    // }
    //     // Example to turn on the Pico W LED
    // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    // initialize the lcd
    st7789_init(&lcd_config, lcd_width, lcd_height);

    //------------- configure buttos ----------------//
    gpio_init(BUTTON_L);
    gpio_set_dir(BUTTON_L, GPIO_IN);
    gpio_pull_up(BUTTON_L);
    gpio_init(BUTTON_R);
    gpio_set_dir(BUTTON_R, GPIO_IN);
    gpio_pull_up(BUTTON_R);
    //-----------------------------------------------//

    uint offset ;
    pio_sm_config c ; 
    uint offset_encode ;
    pio_sm_config c_encode;
    add_square_prog(&c, &offset);
    init_encoder(&c_encode, &offset_encode, pio_encode, sm_encode, 18, 19);
    // initialize the pin for PIO
    pio_gpio_init(pio, pin_nr);

    static repeating_timer_t deb_timer;
    add_repeating_timer_ms(10, dbounceCallback, NULL, &deb_timer);
    pio_sm_put(pio, sm, 30);

    uint16_t pixel_position = 120;
    st7789_fill(0x001F);
    st7789_draw_text(pixel_position, 60, "#", 0x0000, 3);
    while (true) {

        buttonDebauncer(BUTTON_L,0, 0, print_test, print_test);
        buttonDebauncer(BUTTON_R,0, 0, print_test, print_test);
        

        uint len = pio_sm_get_rx_fifo_level(pio_encode, sm_encode);

        if (len){
            uint encoder_mean = 0;
            for (uint i = 0; i < len; i++){
                encoder_mean += pio_sm_get(pio_encode, sm_encode);
            }
            encoder_mean /= len;
            printf("encoder_mean: %d\n", encoder_mean);

            if(encoder_mean >= 4) pixel_position ++;
            else pixel_position --;
            st7789_fill(0x001F);
            st7789_draw_text(pixel_position, 60, "#", 0x0000, 3);
        }
        

    }
}


