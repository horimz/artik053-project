#ifndef CLOUD_H_
#define CLOUD_H_

#include <tinyara/config.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <tinyara/gpio.h>
#include <tinyara/pwm.h>
#include <tinyara/analog/adc.h>
#include <tinyara/analog/ioctl.h>
#include <apps/netutils/dhcpc.h>
#include <apps/netutils/mqtt_api.h>
#include <apps/netutils/ntpclient.h>
#include "wifi.h"
#include "keys.h"

// NTP
#define NTP_REFRESH_PERIOD  (60 * 60) /* seconds */
#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))

#define RED_ON_BOARD_LED 45
#define ADC_MAX_SAMPLES 4
#define NET_DEVNAME "wl1"
#define DEFAULT_CLIENT_ID "123456789"
#define SERVER_ADDR "52.200.124.224"
#define DEVICE_ID "e674c9ade5e74f4ba27eb42e3448b69f" //FIX
#define DEVICE_TOKEN "c7dbbeb48b49416a9efe88ff5f3903e9" // FIX
#define ACTION_TOPIC "/v1.1/actions/e674c9ade5e74f4ba27eb42e3448b69f" // FIX
#define MESSAGE_TOPIC "/v1.1/messages/e674c9ade5e74f4ba27eb42e3448b69f" //FIX

bool mqttConnected;
int ret;

// NTP
static struct ntpc_server_conn_s ntp_server_conn[] = {{ NULL, 123 },};

static void ntp_link_error(void);

// mqtt tls config handle
static mqtt_tls_param_t g_tls;

// mqtt client handle
mqtt_client_t* pClientHandle = NULL;

// mqtt client parameters
mqtt_client_config_t clientConfig;

/* get publisher ready */
int get_publisher_ready(void);

/* publish state once */
int publishOnce(void);

#endif
