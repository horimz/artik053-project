#ifndef BASIC_FUNCTIONS_H_
#define BASIC_FUNCTIONS_H_

#include <apps/shell/tash.h>
#include <tinyara/config.h>
#include <stdio.h>
#include <tinyara/gpio.h>
#include <tinyara/analog/adc.h>
#include <tinyara/analog/ioctl.h>
#include <tinyara/pwm.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

#define HIGH 					1
#define LOW						0
#define TRUE					1
#define FALSE 					0

#define S5J_ADC_MAX_CHANNELS 	6
#define MAX_PIN_NUM 			3

/* global variables */
int adc_fd;
struct adc_msg_s *sample;
struct pwm_info_s pwm_info[4];
int pwm_fd[6];

int analagInit(void);
int analogRead(int port);
void analogFinish(void);
void gpio_write(int port, int value);
int gpio_read(int port);
int pwmInit(int port, int frequency, int duty);
void pwmSet(int port, int duty);
void pwmSetpin(int pin, int *fd);
void pwmFinish(int port);

#endif
