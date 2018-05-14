/*
*******************************************************************************    
*   BTChip Bitcoin Hardware Wallet C test interface
*   (c) 2014 BTChip - 1BTChip7VfTnrPra5jqci7ejnMguuHogTn
*   
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*   limitations under the License.
********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "dongleComm.h"
#include "hexUtils.h"
#include "btchipApdu.h"
#include "btchipArgs.h"
#include "btchipUtils.h"

void char2hex(unsigned char *in, char *hex) {
    while(*in != '\0') {
        sprintf(hex, "%02x", *in);
        in++; hex += 2;
    }
    hex[0] = '\0';
}

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
    char pubKey[512];
	char address[512];
    char pubHex[544];

	if (argc < 2) {
		fprintf(stderr, "Usage : %s [key path in a/b/c format using n' for hardened nodes]\n", argv[0]);
		return 0;
	}
	keyPathLength = convertPath(argv[1], keyPath);
	if (keyPathLength < 0) {
		fprintf(stderr, "Invalid key path\n");
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
	apduSize = 0;
	printf("Uncompressed public key : ");
	displayBinary(out + 1, out[0]);

    int publicKeyLength = (int)out[0];
    int addressLength   = (int)out[1 + publicKeyLength];
    memcpy((unsigned char*)&pubKey[0], &out[1], publicKeyLength); pubKey[publicKeyLength] = '\0';
    memcpy((unsigned char*)&address[0], &out[2+publicKeyLength], addressLength); address[addressLength] = '\0';
/*
			var publicKeyLength = response[0];
			var addressLength = response[1 + publicKeyLength];
			result['publicKey'] = response.slice(1, 1 + publicKeyLength).toString('hex');
			result['address'] = "0x" + response.slice(1 + publicKeyLength + 1, 1 + publicKeyLength + 1 + addressLength).toString('ascii');

	apduSize += out[0] + 1;
	memcpy((unsigned char*)address, out + apduSize + 1, out[apduSize]);
	address[out[apduSize]] = '\0';
	apduSize += out[apduSize] + 1;
	printf("Address : %s\n", address);
	printf("Chaincode : ");
	displayBinary(out + apduSize, 32);
*/
    char2hex((unsigned char *)pubKey, pubHex);
    printf("pubKey:    %s\n", pubHex);
	printf("Address:   %s\n", address);
	printf("Chaincode: ");
	displayBinary(&out[2+publicKeyLength+addressLength], 32);

	return 1;
}