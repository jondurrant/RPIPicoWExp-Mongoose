/*
 * main.cpp
 *
 * A simple Mongoose Application to pass back static web pages
 * Using Mongoose LWIP
 * Listen on port 80
 *  Created on: October-2025
 *      Author: jondurrant
 */

#include <cstdio>
#include <pico/stdlib.h>
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include <task.h>


#include "mongoose.h"

#include "Scanner.h"

#define HTTP_URL "http://0.0.0.0:80"

struct mg_mgr g_mgr;

#define BUFLEN 2048
char buf[BUFLEN];



/***
 * Blink Status LED as call back from Mongoose
 * @param arg
 */
static void blink_timer(void *arg) {
  (void) arg;
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, !cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN));
}

/***
 * Main event callback handler for Mongoose
 * @param c
 * @param ev
 * @param ev_data
 */
static void http_ev_handler(struct mg_connection *c, int ev, void *ev_data) {

	if (ev == MG_EV_HTTP_MSG){
		struct mg_http_message *hm = (struct mg_http_message *) ev_data;  // Parsed HTTP request
		if (mg_match(hm->uri, mg_str("/hw"), NULL)) {
			mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{\"result\": %m}", MG_ESC("Hello World"));
		} else if (mg_match(hm->uri, mg_str("/scan"), NULL)) {
			Scanner scan;
			scan.doScan();
			while (scan.isScanning()){
				vTaskDelay(100);
			}
			scan.getJson(buf, BUFLEN);
			mg_http_reply(c, 200, "Content-Type: application/json\r\n", buf);
		} 	else {
			struct mg_http_serve_opts opts = {
				.root_dir = "/web",
				.fs = &mg_fs_packed
			 };
			mg_http_serve_dir(c, hm, &opts);
		}
	}

}

void mongoose(void *args) {
  struct mg_mgr mgr;        // Initialise Mongoose event manager
  mg_mgr_init(&mgr);        // and attach it to the interface
  mg_log_set(MG_LL_DEBUG);  // Set log level

  cyw43_arch_init();
  cyw43_arch_enable_sta_mode();
  cyw43_arch_wifi_connect_blocking(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK);

  printf("Connected with IP: %s\n",
		  ipaddr_ntoa(netif_ip4_addr(&cyw43_state.netif[0]))
		  );

  MG_INFO(("Initialising application..."));
  mg_http_listen(&mgr, HTTP_URL, http_ev_handler, NULL);

  MG_INFO(("Starting event loop"));
  for (;;) {
    mg_mgr_poll(&mgr, 10);
  }

  (void) args;
}


/***
 * Launch the tasks and scheduler
 */
void vLaunch( void) {

	//Start blink task
    TaskHandle_t task;
    xTaskCreate(mongoose, "mongoose", 2048, NULL, configMAX_PRIORITIES-1, &task);

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}


/***
 * Main
 * @return
 */
int main( void )
{
	//Setup serial over USB and give a few seconds to settle before we start
    stdio_init_all();
    sleep_ms(2000);
    printf("GO\n");

    //Start tasks and scheduler
    const char *rtos_name = "FreeRTOS";
    printf("Starting %s on core 0:\n", rtos_name);
    vLaunch();


    return 0;
}



