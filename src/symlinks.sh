#!/bin/bash

## checkout btchip-c-api
#git clone https://github.com/LedgerHQ/btchip-c-api
## patch with changes
#patch -p0 < ./btchip.patch
#
## checkout CoinCore
#git clone https://github.com/ciphrex/mSIGNA.git
## patch
#patch -p0 < ./hdkeychain.patch

# set up include dir for coincore's insane directory
# structure just so the compiler can find everything
mkdir ./mSIGNA/deps/CoinCore/include/
cd ./mSIGNA/deps/CoinCore/include/
ln -s ../../stdutils/src ./stdutils
ln -s ../../CoinCore/src ./CoinCore
ln -s ../../CoinCore/src/hashfunc ./hashfunc
