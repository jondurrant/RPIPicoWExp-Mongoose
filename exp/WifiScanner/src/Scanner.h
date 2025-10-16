/*
 * Scanner.h
 *
 *  Created on: 20 Sept 2025
 *      Author: jondurrant
 */

#ifndef FIRMWARE_PICO2W_SRC_SCANNER_H_
#define FIRMWARE_PICO2W_SRC_SCANNER_H_

#include "pico/stdlib.h"

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#define MAX_RESULTS 20

class Scanner  {
public:
	Scanner();
	virtual ~Scanner();

	virtual size_t getJson(char *buf, size_t len);

	void doScan();
	bool isScanning();

private:
	static int scanCB(void *env, const cyw43_ev_scan_result_t *result) ;
	int scanHandler(void *env, const cyw43_ev_scan_result_t *result) ;

	cyw43_ev_scan_result_t xResults[MAX_RESULTS];
	int xResultCount = 0;
};

#endif /* FIRMWARE_PICO2W_SRC_SCANNER_H_ */
