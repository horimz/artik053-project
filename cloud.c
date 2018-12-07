#include "cloud.h"

static void ntp_link_error(void)
{
  printf("NTP error: stopping client\n");
  ntpc_stop();
}

// Write (toggle) the value of given gpio port.
void gpio_write_toggle(int port) {
  int value = 0;
  char str[4];
  static char devpath[16];
  int curValue = gpio_read(port);
  printf("Current value of the gpio port%d is %d\n",port,curValue);
  if (curValue <= 0)
    value = 1;
  else if (curValue == 1)
    value = 0;

  snprintf(devpath, 16, "/dev/gpio%d", port);
  int fd = open(devpath, O_RDWR);

  ioctl(fd, GPIOIOC_SET_DIRECTION, GPIO_DIRECTION_OUT);

  write(fd, str, snprintf(str, 4, "%d", value != 0) + 1);

  close(fd);
}

// mqtt client on connect callback
void onConnect(void* client, int result) {
  printf("mqtt client connected to the server\n");
}

// mqtt client on disconnect callback
void onDisconnect(void* client, int result) {
  printf("mqtt client disconnected from the server\n");
}

// mqtt client on message callback
void onMessage(void* client, mqtt_msg_t *msg) {
  printf("mqtt client on message received\n");
  mqtt_client_t *mqtt_client = (mqtt_client_t *)client;

  if (mqtt_client == NULL || mqtt_client->config == NULL) {
    printf("message callback: is NULL.\n");
    return;
  }

  if (msg->payload_len) {
    printf("Topic - %s , payload -  %s\n", msg->topic, msg->payload);
  } else {
    printf(">>> message callback: payload_len is 0\n");
  }
  char buf[msg->payload_len];
  strcpy(buf,msg->payload);

  // Check if the action is related to GPIO
  if (strstr(buf,"Gpio") != NULL) {
    // Extract the gpio port number
    int ref = 0;
    char tempBuf[3];
    char* p = strstr(buf,"Gpio");
    while (*p) {
      if (isdigit(*p))
        tempBuf[ref++] = *p;
      p++;
    }
    tempBuf[ref] = '\0';
    int port = atoi(tempBuf);
    printf("port is %d\n",port);
    gpio_write_toggle(port);	// Write (toggle) the value of given gpio port
  }
}

// Utility function to configure mqtt client
void initializeConfigUtil(void) {
  uint8_t macId[IFHWADDRLEN];
  int result = netlib_getmacaddr("wl1", macId);
  if (result < 0) {
    printf("Get MAC Address failed. Assigning \
        Client ID as 123456789");
    clientConfig.client_id =
        DEFAULT_CLIENT_ID; // MAC id Artik 053
  } else {
    printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
        ((uint8_t *) macId)[0],
        ((uint8_t *) macId)[1], ((uint8_t *) macId)[2],
        ((uint8_t *) macId)[3], ((uint8_t *) macId)[4],
        ((uint8_t *) macId)[5]);
    char buf[12];
    sprintf(buf, "%02x%02x%02x%02x%02x%02x",
        ((uint8_t *) macId)[0],
        ((uint8_t *) macId)[1], ((uint8_t *) macId)[2],
        ((uint8_t *) macId)[3], ((uint8_t *) macId)[4],
        ((uint8_t *) macId)[5]);
    clientConfig.client_id = buf; // MAC id Artik 053
    clientConfig.user_name = DEVICE_ID;
    clientConfig.password = DEVICE_TOKEN;
    clientConfig.clean_session = true; // check

    clientConfig.protocol_version = MQTT_PROTOCOL_VERSION_31;
    g_tls.ca_cert = mqtt_get_ca_certificate();  //the pointer of ca_cert buffer
    g_tls.ca_cert_len = mqtt_get_ca_certificate_size(); // the length of ca_cert buffer


    g_tls.cert = NULL;
    g_tls.cert_len = 0;


    g_tls.key = NULL;
    g_tls.key_len = 0;

    printf("Registering mqtt client with id = %s\n", buf);
  }
  clientConfig.on_connect = (void*) onConnect;
  clientConfig.on_disconnect = (void*) onDisconnect;
  clientConfig.on_message = (void*) onMessage;
  clientConfig.tls = &g_tls;
  //printf("Size of CA is %d\n",g_tls.ca_cert_len);
}

int get_publisher_ready(void) {
	  bool wifiConnected = false;
	  gpio_write_toggle(RED_ON_BOARD_LED); // Turn on on board Red LED to indicate no WiFi connection is established

	  #ifdef CONFIG_CTRL_IFACE_FIFO

	  while(!wifiConnected) {
	    ret = mkfifo(CONFIG_WPA_CTRL_FIFO_DEV_REQ,
	        CONFIG_WPA_CTRL_FIFO_MK_MODE);
	    if (ret != 0 && ret != -EEXIST) {
	      printf("mkfifo error for %s: %s",
	          CONFIG_WPA_CTRL_FIFO_DEV_REQ,
	          strerror(errno));
	    }
	    ret = mkfifo(CONFIG_WPA_CTRL_FIFO_DEV_CFM,
	        CONFIG_WPA_CTRL_FIFO_MK_MODE);
	    if (ret != 0 && ret != -EEXIST) {
	      printf("mkfifo error for %s: %s",
	          CONFIG_WPA_CTRL_FIFO_DEV_CFM,
	          strerror(errno));
	    }

	    ret = mkfifo(CONFIG_WPA_MONITOR_FIFO_DEV,
	        CONFIG_WPA_CTRL_FIFO_MK_MODE);
	    if (ret != 0 && ret != -EEXIST) {
	      printf("mkfifo error for %s: %s",
	          CONFIG_WPA_MONITOR_FIFO_DEV,
	          strerror(errno));
	    }
	  #endif

	    if (start_wifi_interface() == SLSI_STATUS_ERROR) {
	      printf("Connect Wi-Fi failed. Try Again.\n");
	    } else {
	      wifiConnected = true;
	      gpio_write_toggle(RED_ON_BOARD_LED); // Turn off Red LED to indicate WiFi connection is established
	    }
	  }

	  printf("Connect to Wi-Fi success\n");

	  mqttConnected = false;
	  bool ipObtained = false;
	  printf("Get IP address\n");

	  struct dhcpc_state state;
	  void *dhcp_handle;

	  while(!ipObtained) {
	    dhcp_handle = dhcpc_open(NET_DEVNAME);
	    ret = dhcpc_request(dhcp_handle, &state);
	    dhcpc_close(dhcp_handle);

	    if (ret != OK) {
	      printf("Failed to get IP address\n");
	      printf("Try again\n");
	      sleep(1);
	    } else {
	      ipObtained = true;
	    }
	  }
	  netlib_set_ipv4addr(NET_DEVNAME, &state.ipaddr);
	  netlib_set_ipv4netmask(NET_DEVNAME, &state.netmask);
	  netlib_set_dripv4addr(NET_DEVNAME, &state.default_router);

	  printf("IP address  %s\n", inet_ntoa(state.ipaddr));

	  // NTP
	  sleep(1);
	  ntp_server_conn[0].hostname = "0.pool.ntp.org";
	  if (ntpc_start(ntp_server_conn, ARRAY_SIZE(ntp_server_conn), NTP_REFRESH_PERIOD, ntp_link_error) < 0) {
	    printf("Failed to start NTP client.\n"
	        "The date may be incorrect and lead to undefined behavior\n");
	  } else {
	    int num_retries = 10;

	    /* Wait for the date to be set */
	    while ((ntpc_get_link_status() != NTP_LINK_UP) && --num_retries) {
	      sleep(2);
	    }

	    if (!num_retries) {
	      printf("Failed to reach NTP server.\n"
	          "The date may be incorrect and lead to undefined behavior\n");
	    }
	  }

	  initializeConfigUtil();

	  pClientHandle = mqtt_init_client(&clientConfig);
	  if (pClientHandle == NULL) {
	    printf("mqtt client handle initialization fail\n");
	    return 0;
	  }
	  return 0;
}

/* publish state once */
int publishOnce(void) {
	while (mqttConnected == false ) {
		sleep(2);
		// Connect mqtt client to server
		int result = mqtt_connect(pClientHandle,
		    SERVER_ADDR, 8883, 60);
		if (result < 0) {
		  printf("mqtt client connect to server fail - %d\n ",result);
		  continue;
		}
		mqttConnected = true;
	}

	bool mqttSubscribe = false;

	// Subscribe to topic of interest
	while (mqttSubscribe == false) {
		up_mdelay(500);
		int result = mqtt_subscribe(pClientHandle,
		   ACTION_TOPIC, 0);
		if (result < 0) {
		  printf("mqtt client subscribe to topic \
		      failed\n");
		  continue;
		}
		mqttSubscribe = true;
		printf("mqtt client Subscribed to the topic \
		    successfully\n");

		sleep(2);
		bool tempFlag = false;

		while (1) {
		  up_mdelay(500); // Publish message to ARTIK cloud every 0.5s
		  // Publish ADC port readings
		  char str[600];
		  bool adcFlag = false;
		  strcpy(str,"{\"ADC\":{");
		  int fd;
		  struct adc_msg_s samples[ADC_MAX_SAMPLES];
		  ssize_t nbytes;

		  fd = open("/dev/adc0", O_RDONLY);
		  if (fd < 0) {
			  printf("fd open failed: %d\n", errno);
		  }

		  ret = ioctl(fd, ANIOC_TRIGGER, 0);
		  if (ret < 0) {
	      printf("ioctl failed: %d\n",errno);
	      close(fd);
		  }

		  sleep(1); //wait after trigger before reading samples

		  nbytes = read(fd, samples, sizeof(samples));
		  if (nbytes < 0) {
			  if (errno != EINTR) {
				  printf("fd read failed: %d\n", errno);
				  close(fd);
		      }
		  } else if (nbytes == 0) {
			  printf("No data read, Ignoring\n");
		  } else {
			  int nsamples = nbytes / sizeof(struct adc_msg_s);
			  if (nsamples * sizeof(struct adc_msg_s) != nbytes) {
		      printf("read size=%ld is not a multiple of sample size=%d, Ignoring\n",(long)nbytes, sizeof(struct adc_msg_s));
		      } else {
		    	  printf("ADC Sample:\n");
		          int i;
		          for (i = 0; i < nsamples; i++) {
		            printf("%d: channel: %d, value: %d\n", i + 1, samples[i].am_channel, samples[i].am_data);
		            if (adcFlag)
		              strcat(str,",");
		            char tempBuf[20];
		            sprintf(tempBuf,"\"Channel%d\":%d",samples[i].am_channel, samples[i].am_data);
		            strcat(str,tempBuf);
		            adcFlag = true;
		          }
		      }
		      close(fd);
		  }
		  strcat(str,"},");

		  // Publish GPIO port readings

		  strcat(str,"\"GPIO\":{");
		  int j;
		  for ( j = 37; j <= 55; j++) {
		    strcat(str,"\"gpio");
		    char buf[10];
		    sprintf(buf,"%d\":%d",j,gpio_read(j));
		    strcat(str,buf);
		    if (j != 55)
		    strcat(str,",");
		  }
		  strcat(str,"}}");
		  if (mqtt_publish(pClientHandle, MESSAGE_TOPIC, str, sizeof(str), 0, 0) != 0) {
		    printf("Error: mqtt_publish() failed.\n");
		  } else {
		    printf("Published %s\n",str);
		    return 0;		// publish data for one time
		  }
		}
	}
	return 0;
}


