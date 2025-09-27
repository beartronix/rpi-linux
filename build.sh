#!/bin/bash

set -e

# Optional: export cross-compiler
KARCH=arm64
CROSS=aarch64-linux-gnu-

# Go to kernel source directory
make ARCH=$KARCH CROSS_COMPILE=$CROSS bcm2711_defconfig

cat .config | grep "VL050=y"

make ARCH=$KARCH CROSS_COMPILE=$CROSS -j32 Image dtbs modules

# make

