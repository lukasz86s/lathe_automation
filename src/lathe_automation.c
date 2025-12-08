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
typedef struct
{
    volatile uint32_t status_flag;
    volatile uint32_t buttons_on_flag;
    volatile uint8_t gpio_nr[NUM_BUTTONS];
    volatile uint debouce_tims[NUM_BUTTONS];

}button_holder_t;

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
    button_holder_t button_holder = {0};
    button_holder.gpio_nr[0] = BUTTON_L;
    button_holder.gpio_nr[1] = BUTTON_R;

    uint offset ;//= pio_add_program(pio, &squarewave_program);
    pio_sm_config c ; //= squarewave_program_get_default_config(offset);
    add_square_prog(&c, &offset);
    // initialize the pin for PIO
    pio_gpio_init(pio, pin_nr);
    char text[32] = "test\0";
    static repeating_timer_t deb_timer;
    add_repeating_timer_ms(10, dbounceCallback, &button_holder, &deb_timer);
    while (true) {
        // make screen black
        st7789_fill(0x001F);
        //change_sm_program(pio,sm, &squarewave_program, &test1_program, &offset);
        //send number of steps 
        add_test_prog(&c, &offset);
        pio_sm_put(pio, sm, 1000);
        // send period
        pio_sm_put(pio, sm, 100);
        // wait 1 second
        sleep_ms(500);
    
        // make screen red
        st7789_fill(color565(222, 24, 222));
        st7789_draw_text(8, 8, "test A6&8", 0x0000, 3);
        
        //change_sm_program(pio,sm, &test1_program, &squarewave_program,&offset);
        add_square_prog(&c, &offset);
        buttonDebauncer(BUTTON_L,0, 0, print_test, print_test);

        st7789_draw_text(8, 35, text, 0x0000, 3);
        

        // wait 1 second
        pio_sm_put(pio, sm, 30);
        sleep_ms(500);

    }
}


