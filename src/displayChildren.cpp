#ifndef __DISPLAYCHILDREN_H__
#define __DISPLAYCHILDREN_H__

#include <iostream>
#include <cassert>
#include <string>
#include "CoinCore/hdkeys.h"
#include "CoinCore/hashfunc/sph_keccak.h"
#include <sstream>

using namespace Coin;

int goodkey(HDKeychain test, std::string uncompressed) {
    uchar_vector testPub = test.uncompressed_pubkey();
    return (uncompressed.compare(testPub.getHex()) == 0);
}

uchar_vector getChildPubKey(HDKeychain kc, int index) {
    std::stringstream ss; ss << index;
    uchar_vector childPub = kc.getChild(ss.str()).uncompressed_pubkey();
    return childPub;
}

std::string getAddressHash(uchar_vector pubKey) {
    unsigned char *data = new unsigned char[pubKey.size()];
    unsigned char hash[32];
    pubKey.copyToArray(data);

    /* calculate address hash */
    sph_keccak256_context cc;
    sph_keccak256_init(&cc);
    /* drop first byte here '04' */
    sph_keccak256((void *)&cc, (const void *)&data[1], (size_t)(pubKey.size() - 1));
    sph_keccak256_close((void *)&cc, (void *)hash);
    /* keep only the last 20 bytes of keccak256sum */
    uchar_vector addrbytes((const unsigned char *)&hash[12], 20);
    std::string  address = addrbytes.getHex();
    delete[] data;

    return address;
}

void displayChildren(uchar_vector pubKey, uchar_vector cc, int start = 0, int end = 15) {
    std::string compressed;
    std::string uncompressed = pubKey.getHex();
    std::string chaincode    = cc.getHex();
    compressed.append("02");
    compressed.append(uncompressed, 2, 64);
    int i;

    /* Test if even or odd key */
    uchar_vector p;
    p.setHex(compressed);
    HDKeychain test(p, cc, 0, 0, 0);

    if (!goodkey(test, uncompressed)) {
        /* key is odd */
        compressed[1] = '3';
        p.setHex(compressed);
    }

    std::string rootAddress = getAddressHash(pubKey);

    printf("Uncompressed public key:  0x%s\n", pubKey.getHex().c_str());
    printf("Compressed public key:    0x%s\n", compressed.c_str());
	printf("Root Address:             0x%s\n", rootAddress.c_str());
	printf("Chaincode:                0x%s\n", cc.getHex().c_str());

    try {
        HDKeychain kc(p, cc, 0, 0, 0);
        if (!goodkey(kc, uncompressed)) {
            printf("ERROR not good key!\n");
            return;
        }

        for (i = start; i < end; i++) {
            uchar_vector childPub = getChildPubKey(kc, i);
            std::string address = getAddressHash(childPub);
            printf("child[%02i] 0x%s\n", i, address.c_str());
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return;
    }
}

#endif
