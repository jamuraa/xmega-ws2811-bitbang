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

#include "ws2811.h"

#define OUTPORT PORTC
#define OUTPIN PIN0_bm

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
	cli();
	unsigned char *led_ptr = (unsigned char *)leds;
	unsigned char *led_end = (unsigned char *)leds + NUM_LEDS*3;
	// Reset
	OUTPORT.OUTCLR = OUTPIN;
	_delay_us(50);
	WS2811_ALL(OUTPORT, OUTPIN, led_ptr, led_end);
	OUTPORT.OUTSET = OUTPIN;
	sei();
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

void led_color_all(struct rgb_t color) {
	for (int i = 0; i < NUM_LEDS; i++) led_color(i, color.r, color.g, color.b);
}

#define SW0_PRESSED (!(PORTE.IN & PIN5_bm))
#define SW1_PRESSED (!(PORTF.IN & PIN1_bm))
#define SW2_PRESSED (!(PORTF.IN & PIN2_bm))

static bool my_flag_authorize_cdc_transfert = false;

void my_vbus_action(bool b_high)
{
	if (b_high) {
		// Attach USB Device
		udc_attach();
	} else {
		// Vbus not present
		udc_detach();
	}
}


struct rgb_t target_color, current_color;
bool color_step;
uint8_t steps_left;

void go_to_color(uint8_t red, uint8_t green, uint8_t blue) {
	target_color.r = red; target_color.g = green; target_color.b = blue;
	steps_left = 100;
}

void handle_buttons() {
	static bool sw0_held_down, sw1_held_down, sw2_held_down;
	if (SW0_PRESSED) {
		if (!sw0_held_down) {
			go_to_color(0, 255, 255);
			sw0_held_down = true;
		}
	} else {
		sw0_held_down = false;
	}
	if (SW1_PRESSED) {
		if (!sw1_held_down) {
			go_to_color(255, 255, 255);
			sw1_held_down = true;
		}
	} else {
		sw1_held_down = false;
	}
	if (SW2_PRESSED && !sw2_held_down) {
		if (!sw2_held_down) {
			go_to_color(0, 0, 0);
			sw2_held_down = true;
		}
	} else {
		sw2_held_down = false;
	}
}

uint8_t hex_to_int(char h) {
	if (h >= '0' && h <= '9') {
		return (h - '0');
	}
	if (h >= 'A' && h <= 'F') {
		return 10 + (h - 'A');
	}
	if (h >= 'a' && h <= 'f') {
		return 10 + (h - 'a');
	}
	return 0;
}

uint8_t color_from_hex_chars(char top, char bottom) {
	return (hex_to_int(top) << 4) + hex_to_int(bottom);
}

void process_hex_color_string(char *hex_string) {
	uint8_t red = color_from_hex_chars(hex_string[0], hex_string[1]);
	uint8_t green = color_from_hex_chars(hex_string[2], hex_string[3]);
	uint8_t blue = color_from_hex_chars(hex_string[4], hex_string[5]);
	go_to_color(red, green, blue);
}

void handle_usb_serial() {
	static char command_buffer[40];
	static uint8_t buffer_pos = 0;
	if (my_flag_authorize_cdc_transfert) {
		if (udi_cdc_is_rx_ready()) {
			int recvd = udi_cdc_getc();
			command_buffer[buffer_pos++] = recvd;
			udi_cdc_putc(recvd);
			if (recvd == '\r') {
				udi_cdc_putc('\n');
				command_buffer[buffer_pos] = 0;
				process_hex_color_string(command_buffer);
				buffer_pos = 0;
			}
		}
	}
}

uint8_t next_step_towards(uint8_t current, uint8_t target, uint8_t steps) {
	int16_t diff = current - target;
	if (diff > 0) {
		return current - (diff / steps);
	} 
	return current + (-diff / steps);
}

void step_towards_target_color() {
	current_color.r = next_step_towards(current_color.r, target_color.r, steps_left);
	current_color.g = next_step_towards(current_color.g, target_color.g, steps_left);
	current_color.b = next_step_towards(current_color.b, target_color.b, steps_left);
	steps_left--;
}

int main (void) {
	sysclk_init();
	irq_initialize_vectors();
	cpu_irq_enable();
	board_init();
	udc_start();
	pmic_init();
	
	my_vbus_action(true);
	PORTR.DIRSET = PIN0_bm;
	
	PORTE.DIRCLR = PIN5_bm; // SW0 set to input
	PORTF.DIRCLR = PIN1_bm | PIN2_bm; // SW1, SW2 set to input
	
	OUTPORT.DIRSET = OUTPIN;
	OUTPORT.OUTSET = OUTPIN;
	
	steps_left = 0;
	current_color.r = 0; current_color.g = 0; current_color.b = 0;
	target_color = current_color;
	led_color_all(current_color);
	put_leds();
	
	go_to_color(255, 255, 255);
	
	while (1) {
		handle_buttons();
		if (steps_left != 0) {
			PORTR.OUTCLR = PIN0_bm;
			step_towards_target_color();
			led_color_all(current_color);
			put_leds();
			put_leds();
		} else {
			PORTR.OUTSET = PIN0_bm;
		}
		handle_usb_serial();
		_delay_ms(10);
	}	
}

bool my_callback_cdc_enable(void) {
	my_flag_authorize_cdc_transfert = true;
	return true;
}

void my_callback_cdc_disable(void) {
	my_flag_authorize_cdc_transfert = false;
}
