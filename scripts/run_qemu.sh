#!/bin/bash

set -euo pipefail

# ============================================================
# Sensor Monitoring System
# Build Driver, Prepare RootFS and Boot QEMU
# ============================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

DRIVER_DIR="$PROJECT_DIR/driver"
ROOTFS_DIR="$PROJECT_DIR/rootfs/initramfs"
OUTPUT_DIR="$PROJECT_DIR/output"
LOG_DIR="$PROJECT_DIR/logs"

KERNEL_DIR="$HOME/embedded_lab1"
KERNEL_IMAGE="$KERNEL_DIR/output/zImage"
DEVICE_TREE="$KERNEL_DIR/output/vexpress-v2p-ca9.dtb"

KERNEL_VERSION="5.15.0"
MODULE_DIR="$ROOTFS_DIR/lib/modules/$KERNEL_VERSION"
USER_BIN_DIR="$ROOTFS_DIR/usr/bin"

MODULE_SOURCE="$DRIVER_DIR/sms_sensor.ko"
MODULE_DESTINATION="$MODULE_DIR/sms_sensor.ko"

IOCTL_SOURCE="$DRIVER_DIR/test/test_ioctl.c"
IOCTL_BINARY="$DRIVER_DIR/test/test_ioctl"
IOCTL_DESTINATION="$USER_BIN_DIR/test_ioctl"

TEST_SCRIPT_SOURCE="$PROJECT_DIR/scripts/test_driver.sh"
TEST_SCRIPT_DESTINATION="$USER_BIN_DIR/test_driver.sh"

INITRAMFS_IMAGE="$OUTPUT_DIR/initramfs_sms.cpio.gz"
LOG_FILE="$LOG_DIR/qemu_$(date +%Y%m%d_%H%M%S).log"

echo "========================================="
echo " Sensor Monitoring System"
echo "========================================="

mkdir -p "$OUTPUT_DIR"
mkdir -p "$LOG_DIR"
mkdir -p "$MODULE_DIR"
mkdir -p "$USER_BIN_DIR"
mkdir -p "$DRIVER_DIR/test"

if [[ ! -f "$KERNEL_IMAGE" ]]; then
    echo "ERROR: Kernel image not found:"
    echo "       $KERNEL_IMAGE"
    exit 1
fi

if [[ ! -f "$DEVICE_TREE" ]]; then
    echo "ERROR: Device tree not found:"
    echo "       $DEVICE_TREE"
    exit 1
fi

if [[ ! -f "$IOCTL_SOURCE" ]]; then
    echo "ERROR: ioctl test source not found:"
    echo "       $IOCTL_SOURCE"
    exit 1
fi

if [[ ! -f "$TEST_SCRIPT_SOURCE" ]]; then
    echo "ERROR: driver test script not found:"
    echo "       $TEST_SCRIPT_SOURCE"
    exit 1
fi

echo
echo "[1/7] Building kernel driver..."
make -C "$DRIVER_DIR"

if [[ ! -f "$MODULE_SOURCE" ]]; then
    echo "ERROR: Kernel module was not generated."
    exit 1
fi

echo
echo "[2/7] Building ioctl test application..."
arm-linux-gnueabihf-gcc \
    -static \
    -Wall \
    -Wextra \
    -Werror \
    -o "$IOCTL_BINARY" \
    "$IOCTL_SOURCE"

echo
echo "[3/7] Installing files into rootfs..."

cp "$MODULE_SOURCE" "$MODULE_DESTINATION"
cp "$IOCTL_BINARY" "$IOCTL_DESTINATION"
cp "$TEST_SCRIPT_SOURCE" "$TEST_SCRIPT_DESTINATION"

chmod +x "$IOCTL_DESTINATION"
chmod +x "$TEST_SCRIPT_DESTINATION"
chmod +x "$ROOTFS_DIR/init"

echo "Installed module:"
ls -lh "$MODULE_DESTINATION"

echo "Installed test application:"
ls -lh "$IOCTL_DESTINATION"

echo "Installed test script:"
ls -lh "$TEST_SCRIPT_DESTINATION"

echo
echo "[4/7] Creating initramfs..."

rm -f "$INITRAMFS_IMAGE"

(
    cd "$ROOTFS_DIR"

    find . -print0 \
        | cpio --null -H newc -o \
        | gzip -9
) > "$INITRAMFS_IMAGE"

echo "Created initramfs:"
ls -lh "$INITRAMFS_IMAGE"

echo
echo "[5/7] Starting QEMU..."
echo "Log file: $LOG_FILE"
echo
echo "Press Ctrl+A, then X to exit QEMU."
echo

echo "[6/7] Booting Linux and running automatic tests..."

qemu-system-arm \
    -M vexpress-a9 \
    -cpu cortex-a9 \
    -m 512M \
    -nographic \
    -kernel "$KERNEL_IMAGE" \
    -dtb "$DEVICE_TREE" \
    -initrd "$INITRAMFS_IMAGE" \
    -append "console=ttyAMA0,115200 rdinit=/init" \
    2>&1 | tee "$LOG_FILE"

echo
echo "[7/7] QEMU exited."
echo "Saved log: $LOG_FILE"