                            LedgerETHAddress

At the time of writing, the default wallet software that comes with the
Ledger Nano S does not have the capability to use or display more than
one Ethereum address.  The only wallet that has this capability is on
MyEtherWallet.com.

This leaves open a potential security vulnerability: how does one know
that the addresses being displayed by MyEtherWallet.com are truely the
addresses stored on their ledger?

That was the motivation for writing this simple tool. Currently this
tool simply allows you to view the addresses stored on your ledger and
check them against the addresses displayed by MyEtherWallet.


DEPENDENCIES:
    Linux:      You could probably figure out how to build this
                on Windows/Mac but that doesn't sound fun..

    hidapi:     https://github.com/signal11/hidapi
                Used for communicating with usb devices.

    git:        Used to check out source code for:
            mSIGNA:     https://github.com/ciphrex/mSIGNA.git 
                        Much of the source in this project is required
                        to do the elliptic curve math to generate the
                        hierarchical addresses as detailed in BIP0032.

            btchip:     https://github.com/LedgerHQ/btchip-c-api
                        C API maintained by the Ledger team to interact
                        with the ledger.


