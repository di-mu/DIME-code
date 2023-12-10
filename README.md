This repo contains the source code used in the evaluation of the paper "Enabling Direct Message Dissemination in Industrial Wireless Networks via Cross-Technology Communication" by Di Mu, Yitian Chen, Xingjian Chen, Junyang Shi, and Mo Sha, published in IEEE International Conference on Computer Communications (INFOCOM), May 2023.

To run experiments, build and deploy two parts of code with the `make` command:
1. build `telosb-testbed` and upload the image file `examples/tsch-testbed/app-rpl-collect-only.sky` to TelosB nodes on a testbed.
2. build `lora-ctc-transmitter` on a Raspberry Pi and run `lora_control`.
