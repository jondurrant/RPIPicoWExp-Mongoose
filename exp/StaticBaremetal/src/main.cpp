/*
 * main.cpp
 *
 * A simple Mongoose Application to pass back static web pages
 * Using Mongoose TCPIP stack and Drivers
 * Listen on port 80
 * Answer unknown HTTP requests with Hello World in JSON
 *  Created on: October-2025
 *      Author: jondurrant
 */

#include <cstdio>
#include <pico/stdlib.h>

#include "mongoose.h"

#define HTTP_URL "http://0.0.0.0:80"

struct mg_mgr g_mgr;

/***
 * Setup credentials for WiFi
 * @param data
 */
void main_setconfig(void *data) {
  struct mg_tcpip_driver_pico_w_data *d = (struct mg_tcpip_driver_pico_w_data *)data;
  d->ssid = (char *)WIFI_SSID;
  d->pass = (char *)WIFI_PASS;
}

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
		} else {
			struct mg_http_serve_opts opts = {
				.root_dir = "/web",
				.fs = &mg_fs_packed
			 };
			mg_http_serve_dir(c, hm, &opts);
		}
	}

}

/***
 * Initialise Mongoose and network
 */
void mongoose_init(void) {
  mg_mgr_init(&g_mgr);      // Initialise event manager
  mg_log_set(MG_LL_DEBUG);  // Set log level to debug

  MG_INFO(("Starting HTTP listener"));
  mg_http_listen(&g_mgr, HTTP_URL, http_ev_handler, NULL);

}

/***
 * Poll Mongoose
 */
void mongoose_poll(void) {
  mg_mgr_poll(&g_mgr, 10);
}

/***
 * Main
 * @return
 */
int main(){
	stdio_init_all();
	sleep_ms(1000);

	printf("Go\n");


	// do not access the CYW43 LED before Mongoose initializes !
	MG_INFO(("Hardware initialised, starting firmware..."));
	// This blocks forever. Call it at the end of main()
	mongoose_init();
	mg_timer_add(&g_mgr, 1000, MG_TIMER_REPEAT, blink_timer, NULL);
	for (;;) {
		mongoose_poll();
	}


	return 0;
}


