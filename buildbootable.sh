#!/bin/bash
# grub-mkrescue is used to create a bootable ISO image from the compiled kernel and the GRUB configuration.
# The script first assembles the bootloader and compiles the kernel, then it sets up
mkdir -p isodir/boot/grub
cp myos isodir/boot/myos
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o myos.iso isodir
