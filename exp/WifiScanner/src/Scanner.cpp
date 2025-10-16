/*
 * Scanner.cpp
 *
 *  Created on: 20 Sept 2025
 *      Author: jondurrant
 */

#include "Scanner.h"
#include "json-maker/json-maker.h"

Scanner::Scanner(){

}

Scanner::~Scanner() {
	// TODO Auto-generated destructor stub
}


void Scanner::doScan(){
	xResultCount = 0;
	cyw43_wifi_scan_options_t scan_options = {0};
	int err = cyw43_wifi_scan(&cyw43_state, &scan_options, this, Scanner::scanCB);
}

int Scanner::scanCB(void *env, const cyw43_ev_scan_result_t *result) {
	Scanner *self = (Scanner *)env;
	return self->scanHandler(env, result);
}

int Scanner::scanHandler(void *env, const cyw43_ev_scan_result_t *result){
	 if (xResultCount < MAX_RESULTS) {
		 memcpy(&xResults[xResultCount++], result, sizeof(cyw43_ev_scan_result_t));
	 } else {
		 return -1;
	 }
	return 0;
}

bool Scanner::isScanning(){
	return cyw43_wifi_scan_active(&cyw43_state);
}

size_t Scanner::getJson(char *buf, size_t len){
	char *dest = buf;
	size_t l = len;

	char temp[35];

	dest = json_objOpen( dest, NULL, &l );

	dest = json_arrOpen( dest,  "header", &l );
	dest = json_str( dest,  NULL, "mac", &l );
	dest = json_str( dest,  NULL, "ssid" , &l );
	dest = json_str( dest,  NULL, "channel" , &l );
	dest = json_str( dest,  NULL, "authentication" , &l );
	dest = json_str( dest,  NULL ,"strength" , &l );
	dest = json_arrClose( dest,  &l );


	dest = json_arrOpen( dest,  "rows", &l );
	for (int i=0; i < xResultCount; i++){
		dest = json_arrOpen( dest,  NULL, &l );

		sprintf(temp, "%02X:%02X:%02X:%02X:%02X:%02X:",
				xResults[i].bssid[0],
				xResults[i].bssid[1],
				xResults[i].bssid[2],
				xResults[i].bssid[3],
				xResults[i].bssid[4],
				xResults[i].bssid[5]
				);
		dest = json_str( dest, NULL,  temp , &l );

		sprintf(temp, "%.*s", xResults[i].ssid_len, xResults[i].ssid);
		dest = json_str( dest,  NULL, temp , &l );

		dest = json_uint( dest, NULL ,  xResults[i].channel, &l );
		dest = json_uint( dest, NULL ,  xResults[i].auth_mode, &l );
		dest = json_int( dest,  NULL,  xResults[i].rssi , &l );

		dest = json_arrClose( dest,  &l );
	}
	dest = json_arrClose( dest,  &l ); //Rows

	dest = json_objClose( dest, &l ); //outer
	dest = json_end( dest, &l );
	return (len - l);
}

