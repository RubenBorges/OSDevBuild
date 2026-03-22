#!/bin/bash
qemu-system-i386 -cdrom myos.iso

#Additionally, QEMU supports booting multiboot kernels directly without bootable medium:
#       qemu-system-i386 -kernel myos

#OR TEST ON HARDWARE by burning the image to a USB drive (replace /dev/sdx with your actual USB device):

#   sudo dd if=myos.iso of=/dev/sdx && sync