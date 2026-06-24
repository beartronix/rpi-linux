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

# rm symlink to kernel source tree — don't copy the whole tree to the Pi
find $tmpDir/lib/modules/ -maxdepth 2 -name build -type l -delete

scp -q -r $target/boot			root@$remote:/
scp -q -r $target/lib			root@$remote:/usr
# rpi: /lib is symlink to /usr/lib

ssh root@$remote "reboot"