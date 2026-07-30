#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/natos.kernel isodir/boot/natos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "natos" {
	multiboot /boot/natos.kernel
}
EOF
grub-mkrescue -o natos.iso isodir
