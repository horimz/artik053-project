#include "final_project.h"

int main(int argc, char *argv[])
{
	int buttonState;				// for exit
	int touchState1, touchState2;	// for changing mode
	int mode = SETTING_MODE;

	init();
	get_publisher_ready();

	while (true) {
		// if button is pressed turn off
		buttonState = gpio_read(BUTTON_PIN);
		if (!buttonState) {		// if pressed
			printf("OFF\n");
			return(0);
		}

		// if small button 1 is pressed
		if (gpio_read(BUTTON_1) == HIGH) {
			mode = LED_MODE;
			up_mdelay(300);
		}

		// if small button 2 is pressed
		if (gpio_read(BUTTON_2) == HIGH) {
			mode = PROTECT_MODE;
			up_mdelay(300);
		}

		switch (mode) {
		case LED_MODE:
			printf("LED MODE\n");
			mode = led_mode();
			break;
		case PROTECT_MODE:
			printf("PROTECT MODE\n");
			mode = protect_mode();
			break;
		case OFF:
			return(0);
		default:;
		}

		up_mdelay(200);			// 0.2s
	}
}






