#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "btchip-c-api/dongleComm.h"
#include "btchip-c-api/hexUtils.h"
#include "btchip-c-api/btchipApdu.h"
#include "btchip-c-api/btchipArgs.h"
#include "btchip-c-api/btchipUtils.h"

#include <iostream>
#include <cassert>
#include "hdkeys.h"
#include "displayChildren.h"

#include "getLedgerCmdline.h"

using namespace Coin;


int main(int argc, char **argv) {
	dongleHandle dongle;
	unsigned int keyPath[10];
	int keyPathLength;
	unsigned char in[260];
	unsigned char out[260];
	int result;
	int sw;
	int apduSize;
	int i;
	char rootAddress[256];

	gengetopt_args_info args;
	if (cmdline_parser(argc, argv, &args) != 0)
		exit(1);

	keyPathLength = convertPath(args.keypath_arg, keyPath);
	if (keyPathLength < 0) {
		fprintf(stderr, "Invalid key path: '%s'\n", args.keypath_arg);
		return 0;
	}

	initDongle();
	dongle = getFirstDongle();
	if (dongle == NULL) {
		fprintf(stderr, "No dongle found\n");
		return 0;
	}

	apduSize = 0;
	in[apduSize++] = 0xe0;
	in[apduSize++] = 0x02;
	in[apduSize++] = 0x00;
	in[apduSize++] = 0x01;
	in[apduSize++] = 0x01 + (4 * keyPathLength);
	in[apduSize++] = keyPathLength;
	for (i=0; i<keyPathLength; i++) {
		writeUint32BE(in + apduSize, keyPath[i]);
		apduSize += 4;
	}
	printf("Computing public key, please wait ...\n");
	result = sendApduDongle(dongle, in, apduSize, out, sizeof(out), &sw);
	closeDongle(dongle);
	exitDongle();
	if (result < 0) {
		fprintf(stderr, "I/O error\n");
		return 0;
	}
	if (sw != SW_OK) {
		fprintf(stderr, "Dongle application error : %.4x\n", sw);
		return 0;
	}

    int publicKeyLength = (int)out[0];
    int addressLength   = (int)out[1 + publicKeyLength];
    memcpy((unsigned char*)&rootAddress[0], &out[2+publicKeyLength], addressLength); rootAddress[addressLength] = '\0';

    uchar_vector pubKey((const unsigned char *)&out[1], publicKeyLength);
    uchar_vector cc((const unsigned char *)&out[ ( 2 + publicKeyLength + addressLength ) ] , 32);

    displayChildren(pubKey, cc, args.start_arg, (args.start_arg + args.addresses_arg));

	return 1;
}
