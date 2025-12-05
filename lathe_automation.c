#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/st7789.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "stepper_control.pio.h"
#include "fonts.h"


PIO pio = pio0;
uint sm = 0;
uint pin_nr = 0;

#define BLINK_PIN 0      // tutaj numer GPIO, np. 0 -> GPIO0
PIO  PIO_INSTANCE = pio0;
#define SM 0             // PIO state machine 0

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
int main()

{
    stdio_init_all();

    // Initialise the Wi-Fi chip
    if (cyw43_arch_init_with_country(CYW43_COUNTRY_POLAND)) {
        printf("Wi-Fi init failed\n");
        return -1;
    }
    // initialize the lcd
    st7789_init(&lcd_config, lcd_width, lcd_height);

    // Example to turn on the Pico W LED
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);



    uint offset = pio_add_program(pio, &squarewave_program);
    pio_sm_config c = squarewave_program_get_default_config(offset);
    // use of set pins for set instruction
    sm_config_set_set_pins(&c, pin_nr, 1);
    // initialize the pin for PIO
    pio_gpio_init(pio, pin_nr);

    //init state machine
    pio_sm_init(pio, sm , offset, &c);
    // change cloc for pio 150 MHz / 150 MHz = 1us
    pio_sm_set_clkdiv(pio, sm, 150.0f);
    pio_sm_clear_fifos(pio, sm);
    pio_sm_set_enabled(pio, sm, true);

    while (true) {
        printf("Hello, world!\n");
        // make screen black
        st7789_fill(0x001F);
        pio_sm_put(pio, sm, 10);

        // wait 1 second
        sleep_ms(1000);
        //pio_sm_put(pio,sm , okres);
        // make screen red
        st7789_fill(color565(222, 24, 222));
        st7789_draw_text(8, 8, "test A6&8", 0x0000, 5);
        // wait 1 second
        pio_sm_put(pio, sm, 3);
        sleep_ms(1000);

    }
}
