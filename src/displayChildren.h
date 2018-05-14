#ifndef __DISPLAYCHILDREN_H__
#define __DISPLAYCHILDREN_H__

#include <iostream>
#include <cassert>
#include <string>
#include "CoinCore/hdkeys.h"
#include "CoinCore/hashfunc/sph_keccak.h"
#include <sstream>

using namespace Coin;

int goodkey(HDKeychain test, std::string uncompressed);
uchar_vector getChildPubKey(HDKeychain kc, int index);
std::string getAddressHash(uchar_vector pubKey);
void displayChildren(uchar_vector pubKey, uchar_vector cc, int start=0, int end=15);

#endif
