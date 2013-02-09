/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>

#define NOP __asm__ __volatile__ ("cp r0,r0\n");

#define NOP2 NOP NOP
#define NOP1 NOP
#define NOP3 NOP NOP2
#define NOP4 NOP NOP3
#define NOP5 NOP NOP4
#define NOP6 NOP NOP5
#define NOP7 NOP NOP6
#define NOP8 NOP NOP7
#define NOP9 NOP NOP8
#define NOP10 NOP NOP9
#define NOP11 NOP NOP10
#define NOP12 NOP NOP11
#define NOP13 NOP NOP12
#define NOP14 NOP NOP13
#define NOP15 NOP NOP14

#define NOP_SHORT NOP4
#define NOP_LONG NOP12

#define OUTPORT PORTC
#define OUTPIN PIN0_bm

#define WS2811_BIT_SET(X, N) if ( X & (1<<N)) { OUTPORT.OUTSET = OUTPIN; NOP_LONG; OUTPORT.OUTCLR = OUTPIN; NOP_SHORT; } else \
	{ OUTPORT.OUTSET = OUTPIN; NOP_SHORT; OUTPORT.OUTCLR = OUTPIN; NOP_LONG; }

#define WS2811_BIT_ALL() \
WS2811_BIT_SET(cur_byte, 7); \
WS2811_BIT_SET(cur_byte, 6); \
WS2811_BIT_SET(cur_byte, 5); \
WS2811_BIT_SET(cur_byte, 4); \
WS2811_BIT_SET(cur_byte, 3); \
WS2811_BIT_SET(cur_byte, 2); \
WS2811_BIT_SET(cur_byte, 1); \
WS2811_BIT_SET(cur_byte, 0);

#define WS2811_ALL(PTR, END) \
	while(PTR != END) { register unsigned char cur_byte = *PTR++; WS2811_BIT_ALL(); \
	cur_byte = *PTR++; WS2811_BIT_ALL(); cur_byte = *PTR++; WS2811_BIT_ALL(); }

#define F_CPU 32000000
#include <util/delay.h>

struct rgb_t {
	uint8_t g;
	uint8_t r;
	uint8_t b;
};

#define NUM_LEDS 150
struct rgb_t leds[NUM_LEDS];

void put_leds() {
	unsigned char *led_ptr = (unsigned char *)leds;
	unsigned char *led_end = (unsigned char *)leds + NUM_LEDS*3;
	// Reset
	OUTPORT.OUTCLR = OUTPIN;
	_delay_us(50);
	WS2811_ALL(led_ptr, led_end);
	OUTPORT.OUTSET = OUTPIN;
}

void led_color(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
	struct rgb_t *led = leds + i;
	led->r = r;
	led->g = g;
	led->b = b;
}

void led_color_add(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
	struct rgb_t *led = leds + i;
	led->r += r;
	led->g += g;
	led->b += b;
}

void led_color_all(uint8_t r, uint8_t g, uint8_t b) {
	for (int i = 0; i < NUM_LEDS; i++) led_color(i, r, g, b);
}

#define SW0_PRESSED (!(PORTE.IN & PIN5_bm))
#define SW1_PRESSED (!(PORTF.IN & PIN1_bm))
#define SW2_PRESSED (!(PORTF.IN & PIN2_bm))

int main (void)
{
	board_init();
	sysclk_init();
	
	PORTR.DIRSET = PIN0_bm;
	
	PORTE.DIRCLR = PIN5_bm; // SW0 set to input
	PORTF.DIRCLR = PIN1_bm | PIN2_bm; // SW1, SW2 set to input
	
	OUTPORT.DIRSET = OUTPIN;
	OUTPORT.OUTSET = OUTPIN;
	
	led_color_all(0, 0, 0);
	put_leds();
	
	bool led_changed = false;
	bool red_on = false;
	bool blu_on = false;
	bool grn_on = false;
	while (1) {
		led_changed = false;
		if (SW0_PRESSED) {
			red_on = !red_on;
			led_changed = true;	
			
		}
		if (SW1_PRESSED) {
			blu_on = !blu_on;
			led_changed = true;
		}
		if (SW2_PRESSED) {
			grn_on = !grn_on;
			led_changed = true;
		}
		
		if (led_changed) {
			struct rgb_t new_color = {0, 0, 0};
			if (red_on) new_color.r = 255;
			if (blu_on) new_color.b = 255;	
			if (grn_on) new_color.g = 255;
			led_color_all(new_color.r, new_color.g, new_color.b);
			put_leds();
		}
		while (SW0_PRESSED || SW1_PRESSED || SW2_PRESSED) {
			_delay_ms(10);
		}
		_delay_ms(10);
	}
	
	
}
