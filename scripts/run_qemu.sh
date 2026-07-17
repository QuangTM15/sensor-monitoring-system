#!/bin/bash

set -e

# ============================================================
# Sensor Monitoring System
# Build Driver + Boot QEMU
# Version 1.0
# ============================================================

PROJECT_DIR="$HOME/sensor-monitoring-system"

DRIVER_DIR="$PROJECT_DIR/driver"

ROOTFS_DIR="$PROJECT_DIR/rootfs/initramfs"

OUTPUT_DIR="$PROJECT_DIR/output"
LOG_DIR="$PROJECT_DIR/logs"

KERNEL_VERSION="5.15.0"
MODULE_DIR="$ROOTFS_DIR/lib/modules/$KERNEL_VERSION"

mkdir -p "$OUTPUT_DIR"
mkdir -p "$LOG_DIR"
mkdir -p "$MODULE_DIR"

echo "========================================="
echo " Sensor Monitoring System"
echo "========================================="

echo
echo "[1/5] Building kernel driver..."
make -C "$DRIVER_DIR"

echo
echo "[2/5] Copying kernel module..."
cp "$DRIVER_DIR/sms_sensor.ko" \
   "$MODULE_DIR/"

ls -lh "$MODULE_DIR/sms_sensor.ko"

echo
echo "[3/5] Building initramfs..."

cd "$ROOTFS_DIR"

find . -print0 \
    | cpio --null -H newc -o \
    | gzip -9 \
    > "$OUTPUT_DIR/initramfs_sms.cpio.gz"

echo
echo "[4/5] Starting QEMU..."

qemu-system-arm \
    -M vexpress-a9 \
    -cpu cortex-a9 \
    -m 512M \
    -nographic \
    -kernel "$HOME/embedded_lab1/output/zImage" \
    -dtb "$HOME/embedded_lab1/output/vexpress-v2p-ca9.dtb" \
    -initrd "$OUTPUT_DIR/initramfs_sms.cpio.gz" \
    -append "console=ttyAMA0,115200 rdinit=/init" \
    2>&1 | tee "$LOG_DIR/qemu_$(date +%Y%m%d_%H%M%S).log"

echo
echo "[5/5] QEMU exited."