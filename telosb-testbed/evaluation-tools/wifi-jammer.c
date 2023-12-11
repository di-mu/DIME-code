#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <pcap.h>

int main() {
	int ii;
	pcap_t *pcap;
	char pcap_error[PCAP_ERRBUF_SIZE];
	uint8_t packet[256] = {0xff,0xff,0xff,0xff,0xff,0xff};
	for(ii = 12; ii < sizeof(packet); ii++) packet[ii] = rand();
	pcap = pcap_open_live("wlan0", 1024, 0, 250, pcap_error);
	if(pcap == NULL) return 1;
	for (;;) pcap_inject(pcap, packet, sizeof(packet));
	return 0;
}