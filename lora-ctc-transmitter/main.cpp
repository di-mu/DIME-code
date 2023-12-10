#include "MainWindow2.h"
#include <unistd.h>
#include <stdint.h>
#include <iostream>
#include <time.h>

UINT8 txMessage[255];
TMainWindow w;

#define ASN_ENCODING 0
#define N_CODE sizeof(payloadlen)
#define ABS(x) ((x) > 0 ? (x) : -1 * (x))

const uint8_t payloadlen[] = {32,39,47,54,62,69,77,84,92,99,107}; //9,17,23
const uint8_t ctc_slot[] = {0, 0, 50};
const uint8_t ctc_len[] = {1, 0, 1};
const uint32_t HEADWAIT = 12795; //11150: rough constant for all codes; 12795: max
const uint32_t SLOT_LEN = 15011; //was 15021
const uint32_t fr_len[] = {397, 31, 101};
const char* fr_name[] = {"SYNC", "RPL", "APP"};
const uint8_t SYNC_CODES = 4, SYNC_PERIOD = 8;
enum frame_type {FR_EB, FR_RPL, FR_APP, N_FR};
enum timer_id {T_SLOT, N_TIMER};

uint32_t stopwatch(uint8_t num, uint8_t cmd) {
	static struct timespec ts[N_TIMER][2];
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts[num][cmd]);
	return cmd ? (ts[num][1].tv_sec - ts[num][0].tv_sec) * 1000000
		+ (ts[num][1].tv_nsec - ts[num][0].tv_nsec) / 1000 : 0;
}

void wait_slot() {
	stopwatch(T_SLOT, 0);
	while (stopwatch(T_SLOT, 1) < SLOT_LEN);
}

void encode(uint8_t code) {
	stopwatch(T_SLOT, 0);
	while (stopwatch(T_SLOT, 1) < HEADWAIT - code * 300);
	w.cmdRadioLink_SendUMessage2(payloadlen[code]);
	while (stopwatch(T_SLOT, 1) < SLOT_LEN);
}

void num2pay(uint32_t num, uint8_t base, uint8_t len, uint8_t* payload) {
	uint8_t ii;
	for (ii = 1; ii <= len; ii++) {
		payload[len - ii] = num % base;
		num /= base;
	}
}

void framecontrol() {
	uint32_t asn = 0, fr_num;
	uint8_t ff, payload[SYNC_PERIOD - 1];
	for (;;) for (ff = 0; ; ff++) {
		if (ff == N_FR) { //not an active slot
			wait_slot(); asn += 1; break;
		}
		if (asn % fr_len[ff] != ctc_slot[ff]) continue;
		//printf("** %s slot, ASN = %d\n", fr_name[ff], asn);
		if (!ctc_len[ff]) { //pass slot
			wait_slot(); asn += 1; break;
		}
		fr_num = asn / fr_len[ff];
		if (ff == FR_EB) {
#if ASN_ENCODING
			if (fr_num % SYNC_PERIOD == 0) {
				encode(N_CODE - 1); //bell
				num2pay(fr_num / SYNC_PERIOD + 1, SYNC_CODES - 1, SYNC_PERIOD - 1, payload); //new payload
			}
			else {
				encode(N_CODE - SYNC_CODES + payload[fr_num % SYNC_PERIOD - 1]);
			}
#else
			encode(N_CODE - 1);
#endif
		}
		else if (ff == FR_APP) encode(5);
			//encode(fr_num % ((N_CODE - EB_CODES) / 2) * 2);
			//0,1,2,3 -> 0,2,4,6
		asn += ctc_len[ff];
		break;
	}
}

void measure(uint16_t cnt) {
	while (cnt--) encode(N_CODE - 1);
	sleep(20);
}

void init() {
	w.cmdConnection_Open();         //open serial port
	w.cmdDevice_Ping();             //ping device
	w.cmdDevice_FactoryReset();
	sleep(1);

	w.cmdDevice_SetRadioConfiguration(15); //set radio configuration
	w.cmdDevice_GetRadioConfiguration();    //get current radio configuration

	// take default groupaddress
	UINT8 destGroupAddress = 0x10;

	// take default device address
	UINT16 destDeviceAddress = 0x1234;

	// pre-init TxPayload fields

	// add 8-Bit destination radio group address
	txMessage[0] = destGroupAddress;

	// add 16-Bit destination radio address
	HTON16(&txMessage[1], destDeviceAddress);

	// prepare example payload
	memset(txMessage+3, 'A', sizeof(txMessage)-3);
}

int main() {
	init();
	measure(4000);
	framecontrol();
}

#if 0

void encode(uint32_t word, int8_t len, int32_t offset) {
	uint32_t code, mod; //precode
	int8_t ii;
	if (len < 0) { //delay only
		stopwatch(T_SLOT, 0);
		while (stopwatch(T_SLOT, 1) < SLOT_LEN * (uint32_t)ABS(len));
		return;
	}
	mod = 1;
	for (ii=0; ii<len; ii+=1) mod *= N_CODE;
	for (ii=0; ii<len; ii+=1) {
		stopwatch(T_SLOT, 0);
		code = (word % mod) / (mod / N_CODE); //precode
		//code = (code & 1) ? N_CODE - 1 - precode : precode; //one-step payload length change
		mod /= N_CODE;
		while (stopwatch(T_SLOT, 1) < HEADWAIT + offset - code * 300);
		w.cmdRadioLink_SendUMessage2(payloadlen[code]);
		while (stopwatch(T_SLOT, 1) < SLOT_LEN);
		//printf("code sent: %d\n", code);
	}
}

#endif
