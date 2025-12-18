#include "buttons.h"



volatile uint32_t timer_buttons;

void add_encoder_porg(pio_sm_config *c, uint *offset,PIO pio, uint sm, uint e1, uint e2);

void timer_callbak(void){
    
    if(timer_buttons) timer_buttons --;
}

/* @brief  Debounce button press with optional repeat functionality.
 *
 * This routine should be called periodically (e.g., from a timer interrupt)
 * to handle debouncing and optional auto‑repeat behaviour for a single GPIO
 * input pin.  The function uses an internal state machine to filter out
 * mechanical bounce, detect a clean button press, optionally invoke a
 * push callback once per press, and if repeat is requested it will call
 * the repeat callback at a user‑defined rate.
 *
 * @param gpio_nr   GPIO number (pin) that is being monitored.  The function
 *                  will ignore presses on other pins until the current pin
 *                  has been released.
 * @param rep_wait  Time in milliseconds to wait after an initial press
 *                  before starting auto‑repeat.  If set to zero, a default
 *                  of 100 ms (1 s) is used.
 * @param rep_time  Interval in milliseconds between successive repeat
 *                  callback invocations once auto‑repeat has started.
 *                  If zero, a default of 20 ms (200 Hz) is used.
 * @param pushFunc  Function pointer to be called once when the button
 *                  is pressed and released (no repeat).  May be NULL.
 * @param repFunc   Function pointer to be called repeatedly while the
 *                  button remains held down after the initial delay.  May
 *                  be NULL.
 */
void buttonDebauncer(uint8_t gpio_nr,
                   uint16_t rep_wait,
                   uint16_t rep_time,
                   void (* pushFunc)(void),
                   void (* repFunc)(void) ){

  enum KSTATE {idle , debounce, go_rep, wait_rep, rep};
  
  static enum KSTATE button_state;
  static uint8_t last_button;

  uint8_t button_press;
  // check if the same butto is pushed
  if( last_button && last_button != gpio_nr )return;

  button_press = !(gpio_get(gpio_nr));

  if(button_press && !button_state){
    button_state = debounce;
    timer_buttons = 5; // wait 50ms

  }else if(button_state){
    
    if(button_press && button_state == debounce && !timer_buttons){
      button_state = go_rep;
      timer_buttons = 3;      // wait 30ms
      last_button = gpio_nr;
      
      // if button was release
    }else if(!button_press && button_state > debounce && button_state < rep){
      if(pushFunc) pushFunc(); //callback if funciton is assigned
      button_state = idle;
      last_button = 0;

    }else if(button_press && button_state == go_rep && !timer_buttons){
      //default settings if rep_wait or rep_time is 0
      if(!rep_wait) rep_wait = 100; // 1s
      if(!rep_time) rep_time = 20;    // 200ms repeat function five times per second
      button_state = wait_rep;
      timer_buttons = rep_wait;

    }else if(button_press && button_state == wait_rep && !timer_buttons){
      button_state = rep;

    }else if( button_press && button_state == rep && !timer_buttons){
      timer_buttons = rep_time;
      if(repFunc) repFunc();
    }
  }
  if(button_state >= wait_rep && !button_press){
    button_state = idle;
    last_button = 0;
  }
}

void init_encoder(pio_sm_config *c, uint *offset, PIO pio, uint sm, uint e1, uint e2){
    // initialize the pin for PIO
    pio_gpio_init(pio, e1);
    pio_gpio_init(pio, e2);

    add_encoder_porg(c, offset, pio, sm, e1, e2);
};

void add_encoder_porg( pio_sm_config *c, uint *offset,PIO pio, uint sm, uint e1, uint e2){
    *offset = pio_add_program(pio, &encoder_handler_program);
    *c = encoder_handler_program_get_default_config(*offset);
    // use of set pins for set instruction
    sm_config_set_in_pins(c, e1);
    sm_config_set_in_pin_count(c, 2);
    sm_config_set_jmp_pin(c, e2);
    //init state machine
    pio_sm_init(pio, sm , *offset, c);
    // change cloc for pio 150 MHz / 150 KHz = 1MS
    
    pio_sm_set_clkdiv(pio, sm, 150000.0f);
    pio_sm_clear_fifos(pio, sm);
    pio_sm_set_enabled(pio, sm, true);

}