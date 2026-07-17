#!/bin/sh

MODULE_PATH="/lib/modules/5.15.0/sms_sensor.ko"
DEVICE_PATH="/dev/sms_sensor"
PROC_PATH="/proc/sms_stats"
IOCTL_TEST="/usr/bin/test_ioctl"

PASS_COUNT=0
FAIL_COUNT=0

print_title()
{
    echo
    echo "==========================================="
    echo " Sensor Monitoring System Driver Test"
    echo "==========================================="
}

print_step()
{
    echo
    echo "-------------------------------------------"
    echo "$1"
    echo "-------------------------------------------"
}

pass_test()
{
    echo "[PASS] $1"
    PASS_COUNT=$((PASS_COUNT + 1))
}

fail_test()
{
    echo "[FAIL] $1"
    FAIL_COUNT=$((FAIL_COUNT + 1))
}

print_title

print_step "[1] Insert kernel module"

if insmod "$MODULE_PATH"; then
    pass_test "Kernel module loaded successfully"
else
    fail_test "Unable to load kernel module"
fi

print_step "[2] Verify device and procfs entries"

if [ -c "$DEVICE_PATH" ]; then
    ls -l "$DEVICE_PATH"
    pass_test "$DEVICE_PATH exists"
else
    fail_test "$DEVICE_PATH was not created"
fi

if [ -f "$PROC_PATH" ]; then
    ls -l "$PROC_PATH"
    pass_test "$PROC_PATH exists"
else
    fail_test "$PROC_PATH was not created"
fi

print_step "[3] Display initial driver statistics"

if cat "$PROC_PATH"; then
    pass_test "Initial statistics were read successfully"
else
    fail_test "Unable to read initial statistics"
fi

print_step "[4] Read simulated sensor data"

READ_SUCCESS=1

for READ_INDEX in 1 2 3
do
    echo "Sensor read $READ_INDEX:"

    if ! cat "$DEVICE_PATH"; then
        READ_SUCCESS=0
    fi
done

if [ "$READ_SUCCESS" -eq 1 ]; then
    pass_test "Sensor data read three times"
else
    fail_test "One or more sensor reads failed"
fi

print_step "[5] Set sampling interval to 2000 ms"

if "$IOCTL_TEST" 2000; then
    pass_test "Valid interval accepted"
else
    fail_test "Valid interval was rejected"
fi

print_step "[6] Verify interval and counters"

if cat "$PROC_PATH"; then
    pass_test "Updated statistics were read successfully"
else
    fail_test "Unable to read updated statistics"
fi

CURRENT_INTERVAL="$(sed -n 's/^interval_ms: //p' "$PROC_PATH")"
OPEN_COUNT="$(sed -n 's/^open_count: //p' "$PROC_PATH")"
READ_COUNT="$(sed -n 's/^read_count: //p' "$PROC_PATH")"
IOCTL_COUNT="$(sed -n 's/^ioctl_count: //p' "$PROC_PATH")"

if [ "$CURRENT_INTERVAL" = "2000" ]; then
    pass_test "Interval changed to 2000 ms"
else
    fail_test "Unexpected interval: $CURRENT_INTERVAL"
fi

if [ "$OPEN_COUNT" = "4" ]; then
    pass_test "Open counter is correct"
else
    fail_test "Unexpected open counter: $OPEN_COUNT"
fi

if [ "$READ_COUNT" = "3" ]; then
    pass_test "Read counter is correct"
else
    fail_test "Unexpected read counter: $READ_COUNT"
fi

if [ "$IOCTL_COUNT" = "2" ]; then
    pass_test "ioctl counter is correct after SET and GET"
else
    fail_test "Unexpected ioctl counter: $IOCTL_COUNT"
fi

print_step "[7] Test invalid interval"

if "$IOCTL_TEST" 50; then
    fail_test "Invalid interval was incorrectly accepted"
else
    pass_test "Invalid interval was correctly rejected"
fi

CURRENT_INTERVAL="$(sed -n 's/^interval_ms: //p' "$PROC_PATH")"
IOCTL_COUNT="$(sed -n 's/^ioctl_count: //p' "$PROC_PATH")"

if [ "$CURRENT_INTERVAL" = "2000" ]; then
    pass_test "Interval remained unchanged after invalid request"
else
    fail_test "Interval changed unexpectedly: $CURRENT_INTERVAL"
fi

if [ "$IOCTL_COUNT" = "3" ]; then
    pass_test "Invalid ioctl request was counted"
else
    fail_test "Unexpected final ioctl counter: $IOCTL_COUNT"
fi

print_step "[8] Final driver statistics"

cat "$PROC_PATH"

print_step "[9] Recent kernel messages"

dmesg | tail -30

print_step "[10] Remove kernel module"

if rmmod sms_sensor; then
    pass_test "Kernel module removed successfully"
else
    fail_test "Unable to remove kernel module"
fi

print_step "[11] Verify driver cleanup"

if [ ! -e "$DEVICE_PATH" ]; then
    pass_test "$DEVICE_PATH was removed"
else
    fail_test "$DEVICE_PATH still exists"
fi

if [ ! -e "$PROC_PATH" ]; then
    pass_test "$PROC_PATH was removed"
else
    fail_test "$PROC_PATH still exists"
fi

echo
echo "==========================================="
echo " Test Summary"
echo "==========================================="
echo "Passed: $PASS_COUNT"
echo "Failed: $FAIL_COUNT"
echo "==========================================="

if [ "$FAIL_COUNT" -eq 0 ]; then
    echo "RESULT: ALL TESTS PASSED"
    exit 0
fi

echo "RESULT: TEST FAILED"
exit 1