#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int8_t  EN_CTC;
int32_t MINSEQ, MAXSEQ;

void readfile(char *filename, int32_t *asn_seq) {
	int32_t asn, seq, seq_last, asn_last;
	int res;
	FILE *fp;
	fp = fopen(filename, "r");
	if (!fp) {
		printf("[error] %s: can't open\n", filename);
		exit(2);
	}
	seq_last = asn_last = 0;
	do {
		do {
			res = fscanf(fp, "%d,%d", &asn, &seq);
			if (res < 2 || seq > 0xfffe || seq < seq_last || asn < asn_last) {
				printf("[error] %s: seq=%d\n", filename, seq_last);
				exit(3);
			}
		} while (seq == seq_last);
		seq_last = seq;
		asn_last = asn;
		asn_seq[seq] = asn;
	} while (seq < MAXSEQ);
	fclose(fp);
}

/*
void readfile_DR(char *filename, int32_t *asn_seq) {
	int32_t asn, seq, seq_last, asn_last;
	int res;
	FILE *fp;
	fp = fopen(filename, "r");
	if (!fp) {
		printf("[error] %s: can't open\n", filename);
		exit(2);
	}
	seq_last = asn_last = 0;
	do {
		do {
			res = fscanf(fp, "%d,%d", &asn, &seq);
			if (res < 2 || seq > 0xfffe || seq < seq_last || asn < asn_last) {
				printf("[error] %s: seq=%d\n", filename, seq_last);
				exit(3);
			}
		} while (seq == seq_last);
		seq_last = seq;
		asn_last = asn;
		asn_seq[seq] = asn;
	} while (seq < 59);
	fclose(fp);
}
*/

enum Transmissions {UT, UR, DR, N_TRANS};

int main(int argn, char *argv[]) {
	int32_t asn_ctc, asn_ctc_last, asn_dr;
	int32_t seq, att, scs;
	int32_t asn[N_TRANS][0xffff] = {0};
	int64_t laten_ee, laten_dl, laten_seg;
	uint16_t seglen, scsseg, scssegmin = -1, scssegmax = 0;
	double latseg, latsegmin = 1e10, latsegmax = 0.0;
	int res;
	FILE *fp_ctc;
	if (argn != 5) {
		printf("usage: %s <CTC:0/1> <SEGLEN (pkts)> <MINSEQ> <MAXSEQ>\n", argv[0]);
		return 1;
	}
	sscanf(argv[1], "%hhd", &EN_CTC);
	sscanf(argv[2], "%hd", &seglen);
	sscanf(argv[3], "%d", &MINSEQ);
	sscanf(argv[4], "%d", &MAXSEQ);
	readfile("UL-TX.csv", asn[UT]);
	readfile("UL-RX.csv", asn[UR]);
	readfile("DL-RX.csv", asn[DR]);
	//printf("file loaded\n");
	if (EN_CTC) fp_ctc = fopen("DL-CTC.csv", "r");
	att = scs = 0; scsseg = 0;
	asn_dr = asn_ctc = asn_ctc_last = 0;
	laten_ee = laten_dl = laten_seg = 0;
	for (seq = MINSEQ; seq < MAXSEQ; seq++) {
		if (!asn[UT][seq]) continue;
		//printf("seq=%d\n", seq);
		att++;
		if (att > seglen && att % seglen == 1) {
			if (!scsseg) printf("seq %d, pdr 0.00, lat -\n", seq);
			else {
				latseg = (double)laten_seg * 15 / scsseg;
				if (scsseg < scssegmin) scssegmin = scsseg;
				if (scsseg > scssegmax) scssegmax = scsseg;
				if (latseg < latsegmin) latsegmin = latseg;
				if (latseg > latsegmax) latsegmax = latseg;
				printf("seq %d, pdr %f, lat %.2f ms\n", seq, (float)scsseg / seglen, latseg);
				scsseg = 0;
				laten_seg = 0;
			}
		}
		//printf("att=%d\n", att);
		if (!asn[UR][seq]) continue;
		if (!EN_CTC) goto NO_CTC;
		while (asn_ctc <= asn[UR][seq]) {
			res = fscanf(fp_ctc, "%d", &asn_ctc);
			if (res < 1 || asn_ctc < asn_ctc_last) {
				printf("[error] DL-CTC: ASN = %d\n", asn_ctc);
				exit(3);
			}
		}
		asn_ctc_last = asn_ctc;
		if (asn_ctc <= asn[UR][seq+1]) {
			asn_dr = (asn[DR][seq] && asn[DR][seq] < asn_ctc) ? asn[DR][seq] : asn_ctc;
		}
		else {
NO_CTC:
			if (!asn[DR][seq]) continue;
			asn_dr = asn[DR][seq];
		}
		laten_ee += asn_dr - asn[UT][seq];
		laten_dl += asn_dr - asn[UR][seq];
		laten_seg += asn_dr - asn[UT][seq];
		//printf("seq %d, ee %d, dl %d\n", seq, asn_dr - asn[UT][seq], asn_dr - asn[UR][seq]);
		scs++;
		scsseg++;
		//printf("scsseg=%d\n", scsseg);
	}
	if (EN_CTC) fclose(fp_ctc);
	fflush(stdout);
	printf("- - - - - - - - - -\n");
	assert(seglen);
	assert(att);
	assert(scs);
	printf("PDR[min,max,overall] = %f,%f,%f (%d/%d)\n", (float)scssegmin / seglen, (float)scssegmax / seglen, (double)scs / att, scs, att);
	printf("Latency[min,max,avg] = %.2f,%.2f,%.2f (ms)\n", latsegmin, latsegmax, (double)laten_ee * 15 / scs);
	//printf("Downlink Latency (mean) = %.2lf ms\n", (double)laten_dl * 15 / scs);
	return 0;
}
