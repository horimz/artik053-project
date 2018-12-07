#ifndef FINAL_PROJECT_H_
#define FINAL_PROJECT_H_

/* headers */
#include <stdio.h>
#include <stdbool.h>
#include <apps/shell/tash.h>
#include <tinyara/config.h>
#include <tinyara/gpio.h>
#include <tinyara/analog/adc.h>
#include <tinyara/analog/ioctl.h>
#include <tinyara/pwm.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include "basic_functions.h"
//#include "cloud.h"

/* mode */
#define SETTING_MODE 		39
#define LED_MODE 			38
#define PROTECT_MODE 		40
#define RESET				41		// to reset protect mode
#define OFF					0

/* pins */
#define TOUCH_PIN	37		// touch sensor
#define BUTTON_PIN 	31		// button module
#define BUTTON_1 	30		// small button
#define BUTTON_2	32		// small button
#define BUZZER_PIN  55		// active buzzer
#define R_PIN 		52		// rgb led red pin
#define G_PIN 		51		// rgb led green pin
#define B_PIN 		57		// rgb led blue pin

void init(void);			// set mode to setting mode
int led_mode(void);
int protect_mode(void);

#endif


