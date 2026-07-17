#ifndef SMS_SENSOR_H
#define SMS_SENSOR_H

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/types.h>

#define SMS_DRIVER_NAME "sms_sensor"
#define SMS_DEVICE_NAME "sms_sensor"
#define SMS_CLASS_NAME  "sms_sensor_class"

struct sms_sensor_data
{
    int temperature;
    int humidity;
};

struct sms_device
{
    dev_t dev_number;
    struct cdev cdev;
    struct class *class;
    struct device *device;
};

int sms_generate_sensor_data(struct sms_sensor_data *data);

#endif