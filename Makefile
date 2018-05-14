CXX 		= g++
CC  		= gcc
CFLAGS      = -g -Wall -Werror -Os -DHAVE_HIDAPI -DEXTRA_DEBUG -I. -I/usr/local/include
CXXFLAGS 	= -std=c++11 -Wall -g
LIBS 		= -L/usr/local/lib -lcrypto
HIDAPI      = hidapi-hidraw

ROOTDIR = ./src
OBJDIR  = ./obj
MSIGDIR = $(ROOTDIR)/mSIGNA
COINDIR = $(MSIGDIR)/deps/CoinCore/src
COININC = $(MSIGDIR)/deps/CoinCore/include/
BTCHDIR = $(ROOTDIR)/btchip-c-api

BTSRCS = \
	bitcoinAmount.c \
	ledgerLayer.c \
	dongleCommHidHidapi.c \
	dongleCommHid.c \
	bitcoinVarint.c \
	bitcoinTransaction.c \
	btchipUtils.c \
	dongleComm.c \
	hexUtils.c \
	btchipArgs.c

BTOBJS=\
	$(addprefix $(OBJDIR)/btchip/, $(BTSRCS:.c=.o))

COINSRCS = \
	secp256k1_openssl.cpp \
	hdkeys.cpp

COINOBJS=\
	$(addprefix $(OBJDIR)/CoinCore/, $(COINSRCS:.cpp=.o))


all: bin/btchip_getEtherPublicKey bin/hdkeychain bin/getLedgerAddresses

bin/btchip_getEtherPublicKey: $(ROOTDIR)/btchip_getEtherPublicKey.c $(BTOBJS)
	$(CXX) -I$(BTCHDIR) $(CXXFLAGS) $(BTOBJS) -o $@ $< -l $(HIDAPI) $(LIBS)

bin/hdkeychain: $(MSIGDIR)/deps/CoinCore/examples/hdkeychain/hdkeychain.cpp $(COINOBJS) $(OBJDIR)/CoinCore/keccak.o $(OBJDIR)/displayChildren.o
	$(CXX) $(CXXFLAGS) -I$(COININC) -o $@ $(OBJDIR)/displayChildren.o $< $(COINOBJS) $(OBJDIR)/CoinCore/keccak.o $(LIBS)

bin/getLedgerAddresses: $(ROOTDIR)/getLedgerAddresses.cpp $(BTOBJS) $(COINOBJS) $(OBJDIR)/displayChildren.o $(OBJDIR)/CoinCore/keccak.o $(OBJDIR)/getLedgerCmdline.o
	$(CXX) $(CXXFLAGS) $(BTOBJS) $(COINOBJS) $(OBJDIR)/CoinCore/keccak.o $(OBJDIR)/displayChildren.o $(OBJDIR)/getLedgerCmdline.o -I$(COININC) -I$(COINDIR) -o $@  $< -l $(HIDAPI) $(LIBS)

$(OBJDIR)/displayChildren.o: $(ROOTDIR)/displayChildren.cpp
	$(CXX) $(CXXFLAGS) -I$(COININC) -o $@ -c $<

$(OBJDIR)/getLedgerCmdline.o: $(ROOTDIR)/getLedgerCmdline.c $(ROOTDIR)/getLedgerCmdline.h
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(ROOTDIR)/getLedgerCmdline.c: $(ROOTDIR)/gengetopt/getLedgerAddresses_cmd.ggo
	gengetopt -i $< -F getLedgerCmdline --include-getopt --output-dir=$(ROOTDIR)

$(ROOTDIR)/getLedgerCmdline.h: $(ROOTDIR)/gengetopt/getLedgerAddresses_cmd.ggo
	gengetopt -i $< -F getLedgerCmdline --include-getopt --output-dir=$(ROOTDIR)

$(OBJDIR)/btchip/%.o: $(BTCHDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(COINDIR)/%.cpp: $(MSIGDIR)/deps/CoinCore/examples/hdkeychain/hdkeychain.cpp
	
$(OBJDIR)/CoinCore/%.o: $(COINDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(COININC) -o $@ -c $<

$(OBJDIR)/CoinCore/keccak.o: $(COINDIR)/hashfunc/keccak.c
	$(CC) $(CFLAGS) -o $@ -c $<


clean:
	rm -rf bin/* $(OBJDIR)/*.o $(OBJDIR)/btchip/*.o $(OBJDIR)/CoinCore/*.o


.SECONDARY:

$(BTCHDIR)/%.c: $(ROOTDIR)/btchip.patch $(MSIGDIR)/deps/CoinCore/examples/hdkeychain/hdkeychain.cpp
	cd $(ROOTDIR) && git clone https://github.com/LedgerHQ/btchip-c-api && patch -p0 < ./btchip.patch && cd ..

$(MSIGDIR)/deps/CoinCore/examples/hdkeychain/hdkeychain.cpp: $(ROOTDIR)/symlinks.sh $(ROOTDIR)/hdkeychain.patch
	cd $(ROOTDIR) && git clone https://github.com/ciphrex/mSIGNA.git && patch -p0 < ./hdkeychain.patch && ./symlinks.sh && cd ..
