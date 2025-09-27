#!/bin/bash

set -e

remote=${1:-"10.0.21.67"}


KARCH=arm64
CROSS=aarch64-linux-gnu-

tmpDir=/tmp/rpicm5kernel
rm -rf $tmpDir/*
echo $tmpDir
target=$tmpDir
# target=/media/stefan

./build.sh

#deploy to temp
make ARCH=$KARCH CROSS_COMPILE=$CROSS INSTALL_PATH=$target/boot install > /dev/null
make ARCH=$KARCH CROSS_COMPILE=$CROSS INSTALL_DTBS_PATH=$target/boot/firmware dtbs_install > /dev/null
make ARCH=$KARCH CROSS_COMPILE=$CROSS INSTALL_MOD_PATH=$target modules_install > /dev/null
cp arch/arm64/boot/Image $target/boot/firmware/kernel_2712.img

# rm symlink, we don't wanna copy everything over there...
rm /tmp/rpicm5kernel/lib/modules/6.12.47-v8+/build

scp -q -r $target/boot			root@$remote:/
scp -q -r $target/lib			root@$remote:/usr
# rpi: /lib is symlink to /usr/lib

ssh root@$remote "reboot"