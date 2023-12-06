#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

/************************************************/
//              DEFINES                         //
/************************************************/

// LED
#define LED_PIO      PIOC
#define LED_PIO_ID   ID_PIOC
#define LED_IDX      8
#define LED_IDX_MASK (1 << LED_IDX)

// BUT 1
#define BUT1_PIO           PIOD
#define BUT1_PIO_ID        ID_PIOD
#define BUT1_PIO_IDX       28
#define BUT1_PIO_IDX_MASK  (1 << BUT1_PIO_IDX)

// BUT 2
#define BUT2_PIO           PIOC
#define BUT2_PIO_ID        ID_PIOC
#define BUT2_PIO_IDX       31
#define BUT2_PIO_IDX_MASK  (1 << BUT2_PIO_IDX)

// BUT 3
#define BUT3_PIO           PIOA
#define BUT3_PIO_ID        ID_PIOA
#define BUT3_PIO_IDX       19
#define BUT3_PIO_IDX_MASK  (1 << BUT3_PIO_IDX)

// BUT PLACA
#define BUT_PIO      PIOA
#define BUT_PIO_ID   ID_PIOA
#define BUT_IDX  11
#define BUT_IDX_MASK (1 << BUT_IDX)

volatile int delay = 50;

volatile int update_oled = 1;
volatile int is_but1_pressed = 0;
volatile int is_but2_pressed = 0;
volatile int is_but3_pressed = 0;

volatile int but1_pressed_for = 0;
volatile int but3_pressed_for = 0;

volatile int remaining_blinks = 0;
volatile int reset_progress_bar = 0;

int max_blinks = 30;

void but_callback(void)
{
	remaining_blinks = max_blinks;
	reset_progress_bar = 1;
}

void but1_callback(void)
{
	if (pio_get(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK)) {
        is_but1_pressed = 0;
		but1_pressed_for = 0;
    } else {
		is_but1_pressed = 1;
		
		delay -= 10;
		if (delay <= 0) {
			delay = 0;
		} 
		update_oled = 1;
    } 
}

void but2_callback(void)
{
	remaining_blinks = 0;
	update_oled = 1;
	reset_progress_bar = 1;
}

void but3_callback(void)
{
	if (pio_get(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK)) {
        is_but3_pressed = 0;
		but3_pressed_for = 0;
    } else {
		is_but3_pressed = 1;
		
		delay += 10;
		update_oled = 1;  
    }
}

void pisca_led(int n){
	for (int i=0;i<n;i++){
		pio_clear(LED_PIO, LED_IDX_MASK);
		delay_ms(delay);
		pio_set(LED_PIO, LED_IDX_MASK);
		delay_ms(delay);
	}
}

int main (void)
{
	board_init();
	sysclk_init();
	delay_init();
	
	pmc_enable_periph_clk(LED_PIO_ID);
	pmc_enable_periph_clk(BUT_PIO_ID);
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pmc_enable_periph_clk(BUT2_PIO_ID);
	pmc_enable_periph_clk(BUT3_PIO_ID);
	
	// LED
	pio_configure(LED_PIO, PIO_OUTPUT_0, LED_IDX_MASK, PIO_DEFAULT);
	
	// BUT 1
	pio_configure(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK, (PIO_PULLUP | PIO_DEBOUNCE));
	pio_set_debounce_filter(BUT1_PIO, BUT1_PIO_IDX_MASK, 60);
	
	// BUT 2
	pio_configure(BUT2_PIO, PIO_INPUT, BUT2_PIO_IDX_MASK, (PIO_PULLUP | PIO_DEBOUNCE));
	pio_set_debounce_filter(BUT2_PIO, BUT1_PIO_IDX_MASK, 60);
	
	// BUT 3
	pio_configure(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK, (PIO_PULLUP | PIO_DEBOUNCE));
	pio_set_debounce_filter(BUT3_PIO, BUT3_PIO_IDX_MASK, 60);
	
	// BUT PLACA
	pio_configure(BUT_PIO, PIO_INPUT, BUT_IDX_MASK, (PIO_PULLUP | PIO_DEBOUNCE));
	pio_set_debounce_filter(BUT_PIO, BUT_IDX_MASK, 60);
	
	pio_handler_set(BUT1_PIO,
                  BUT1_PIO_ID,
                  BUT1_PIO_IDX_MASK,
                  PIO_IT_EDGE,
                  but1_callback);
				  
	pio_handler_set(BUT2_PIO,
                  BUT2_PIO_ID,
                  BUT2_PIO_IDX_MASK,
                  PIO_IT_RISE_EDGE,
                  but2_callback);
				  
	pio_handler_set(BUT3_PIO,
                  BUT3_PIO_ID,
                  BUT3_PIO_IDX_MASK,
                  PIO_IT_EDGE,
                  but3_callback);
				  
	pio_handler_set(BUT_PIO,
					BUT_PIO_ID,
					BUT_IDX_MASK,
					PIO_IT_RISE_EDGE,
					but_callback);
					
	// BUT 1
	pio_enable_interrupt(BUT1_PIO, BUT1_PIO_IDX_MASK);
	pio_get_interrupt_status(BUT1_PIO);
	  
	NVIC_EnableIRQ(BUT1_PIO_ID);
	NVIC_SetPriority(BUT1_PIO_ID, 4); // Prioridade 4
	
	// BUT 2
	pio_enable_interrupt(BUT2_PIO, BUT2_PIO_IDX_MASK);
	pio_get_interrupt_status(BUT2_PIO);
	  
	NVIC_EnableIRQ(BUT2_PIO_ID);
	NVIC_SetPriority(BUT2_PIO_ID, 4); // Prioridade 4
	
	// BUT 3
	pio_enable_interrupt(BUT3_PIO, BUT3_PIO_IDX_MASK);
	pio_get_interrupt_status(BUT3_PIO);
	  
	NVIC_EnableIRQ(BUT3_PIO_ID);
	NVIC_SetPriority(BUT3_PIO_ID, 4); // Prioridade 4
	
	// BUT PLACA			
	pio_enable_interrupt(BUT_PIO, BUT_IDX_MASK);
	pio_get_interrupt_status(BUT_PIO);
	  
	NVIC_EnableIRQ(BUT_PIO_ID);
	NVIC_SetPriority(BUT_PIO_ID, 4); // Prioridade 4

  // Init OLED
	gfx_mono_ssd1306_init();  

	int led_counter = 0;
	int led_state = 0;
	int toggle_led = 0;

  /* Insert application code here, after the board has been initialized. */
	while(1) {
			int should_sleep = 1;
		
			if (is_but1_pressed) {
				but1_pressed_for += 1;
				
				if (but1_pressed_for >= 200) {
					delay -= 10;
					update_oled = 1;
					but1_pressed_for = 0;
				}
				
				should_sleep = 0;
			}
			
			if (is_but3_pressed) {
				but3_pressed_for += 1; 
				
				if (but3_pressed_for >= 200) {
					delay += 10;
					update_oled = 1;
					but3_pressed_for = 0;
				}
				
				should_sleep = 0;
			}
		
			if (remaining_blinks > 0) {
				if (led_counter > 0) {
					led_counter--;
				} else {
					led_counter = delay/2;
					toggle_led = 1;
				}
				should_sleep = 0;
			}
			
			if (toggle_led) {
				if (led_state) {
					pio_clear(LED_PIO, LED_IDX_MASK);
					remaining_blinks--;
					update_oled = 1;
				} else {
					pio_set(LED_PIO, LED_IDX_MASK);
				}
				led_state = !led_state;
				toggle_led = 0;
				should_sleep = 0;
			}
			
			if (reset_progress_bar) {
				if (remaining_blinks > 0) {
					gfx_mono_draw_filled_rect(0, 16, 128, 16, GFX_PIXEL_SET);
				} else {
					gfx_mono_draw_filled_rect(0, 16, 128, 16, GFX_PIXEL_CLR);
				}
				reset_progress_bar = 0;
				should_sleep = 0;
			}
			
			// Update da tela
			if (update_oled) {
				char str[128];
				char str2[128];
				sprintf(str, "DELAY: %dms", delay);
				sprintf(str2, "%d", remaining_blinks);
				gfx_mono_draw_filled_rect(0, 0, 128, 16, GFX_PIXEL_CLR);
				gfx_mono_draw_filled_rect((double) remaining_blinks/max_blinks*128.0, 16, 1.5/30.0*128.0, 16, GFX_PIXEL_CLR);
				
				gfx_mono_draw_string(str, 0, 0, &sysfont);
				// gfx_mono_draw_string(str2, 0, 16, &sysfont);
				update_oled = 0;
				should_sleep = 0;
			}
			
			delay_ms(1);
			
			if (should_sleep) {
				pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
			}
	}
}
