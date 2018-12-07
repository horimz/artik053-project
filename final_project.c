#include "final_project.h"

/*
 *  < LED MODE >
 *	- change color using touch sensor { white(default) -> red -> green -> blue }
 *	- if button module is pressed -> go back to setting mode
 *	- if small button 2 is pressed -> go to protect mode
 */
int led_mode(void) {
	int rpin = R_PIN, gpin = G_PIN, bpin = B_PIN;
	int touchState1, touchState2;
	int count = 0;

	set_led_mode();							// set mode to led mode
	touchState1 = gpio_read(TOUCH_PIN);		// get current state of touch sensor

	while (true) {
		// if button is pressed -> go back to setting mode
		if (gpio_read(BUTTON_PIN) == LOW) {
			printf("back to setting mode\n");
			rgb_led_off(rpin, gpin, bpin);
			return SETTING_MODE;
		}

		// if small button 2 is pressed -> go to protect mode
		if (gpio_read(BUTTON_2) == HIGH) {
			printf("go to protect mode\n");
			rgb_led_off(rpin, gpin, bpin);
			return PROTECT_MODE;
		}

		// If touched -> increment count by 1
		touchState2 = gpio_read(TOUCH_PIN);
		if (touchState1 != touchState2) {
			printf("touched\n");
			count++;
			touchState1 = touchState2;
		}

		// change color
		RGB(count%5, rpin, gpin, bpin);

		up_mdelay(200);		// 0.2s
	}
}

/*
 * < PROTECT MODE: alert me if someone touches my things >
 * - if button module is pressed -> go back to setting mode
 * - if small button 1 is pressed -> change to led mode
 * - if touched -> turn the buzzer on and alert me via artik cloud
 */
int protect_mode(void) {
	int buzzerPin = BUZZER_PIN;
	int touchState1, touchState2;

	set_protect_mode();						// set mode to protect mode
//	get_publisher_ready();
	publishOnce();							// dual color led turns green (for the other device)
	touchState1 = gpio_read(TOUCH_PIN);		// get current state of touch sensor

	while (true) {
		// if button is pressed -> go back to setting mode
		if (gpio_read(BUTTON_PIN) == LOW) {
			printf("back to setting mode\n");
			publishOnce();					// dual color led off (for the other device)
			return SETTING_MODE;
		}

		// if small button 1 is pressed -> change to led mode
		if (gpio_read(BUTTON_1) == HIGH) {
			printf("go to led mode\n");
			publishOnce();					// dual color led off (for the other device)
			return LED_MODE;
		}

		// If touched -> turn the buzzer on and alert me via artik cloud
		touchState2 = gpio_read(TOUCH_PIN);
		if (touchState1 != touchState2) {
			printf("alert!\n");
			buzzerOn(buzzerPin);			// buzzer on
			publishOnce();					// dual color led turns red (for the other device)
			sleep(2);
			buzzerOff(buzzerPin);			// buzzer off
			gpio_write(RESET, HIGH);
			publishOnce();					// dual color led off (for the other device)
			gpio_write(RESET, LOW);
			return OFF;						// turn the device off
		}

		up_mdelay(200);		// 0.2s
	}
}


/* helper functions */

void RGB(int count, int rpin, int gpin, int bpin) {

	switch (count) {
	case 1:		// red
		rgb_led_red(rpin, gpin, bpin);
		break;
	case 2:		// green
		rgb_led_green(rpin, gpin, bpin);
		break;
	case 3:		// blue
		rgb_led_blue(rpin, gpin, bpin);
		break;
	case 4:		// off
		rgb_led_off(rpin, gpin, bpin);
		break;
	default:	// white
		rgb_led_on(rpin, gpin, bpin);
		break;
	}
}

void rgb_led_on(int rpin, int gpin, int bpin) {
	gpio_write(rpin, 0);
	gpio_write(gpin, 0);
	gpio_write(bpin, 0);
}

void rgb_led_off(int rpin, int gpin, int bpin) {
	gpio_write(rpin, 1);
	gpio_write(gpin, 1);
	gpio_write(bpin, 1);
}

void rgb_led_red(int rpin, int gpin, int bpin) {
	gpio_write(rpin, 0);
	gpio_write(gpin, 1);
	gpio_write(bpin, 1);
}

void rgb_led_green(int rpin, int gpin, int bpin) {
	gpio_write(rpin, 1);
	gpio_write(gpin, 0);
	gpio_write(bpin, 1);
}

void rgb_led_blue(int rpin, int gpin, int bpin) {
	gpio_write(rpin, 1);
	gpio_write(gpin, 1);
	gpio_write(bpin, 0);
}

void buzzerOn(int pin) {
	gpio_write(pin, 0);
}

void buzzerOff(int pin) {
	gpio_write(pin, 1);
}

void init(void) {
	gpio_write(SETTING_MODE, LOW);
	gpio_write(LED_MODE, LOW);
	gpio_write(PROTECT_MODE, LOW);
	gpio_write(RESET, LOW);
	buzzerOff(BUZZER_PIN);
}

void set_led_mode(void) {
	gpio_write(SETTING_MODE, LOW);
	gpio_write(LED_MODE, HIGH);
	gpio_write(PROTECT_MODE, LOW);
}

void set_protect_mode(void) {
	gpio_write(SETTING_MODE, LOW);
	gpio_write(LED_MODE, LOW);
	gpio_write(PROTECT_MODE, HIGH);
}
