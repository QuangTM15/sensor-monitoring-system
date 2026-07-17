#ifndef SMS_SENSOR_H
#define SMS_SENSOR_H

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/types.h>

#define SMS_DRIVER_NAME "sms_sensor"
#define SMS_DEVICE_NAME "sms_sensor"
#define SMS_CLASS_NAME "sms_sensor_class"
#define SMS_PROC_NAME "sms_stats"

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

struct sms_driver_stats
{
    unsigned int open_count;
    unsigned int read_count;
    unsigned int ioctl_count;
    unsigned int interval_ms;
};

extern struct sms_driver_stats sms_stats;

int sms_generate_sensor_data(struct sms_sensor_data *data);

unsigned int sms_get_interval(void);

int sms_set_interval(unsigned int interval_ms);

#endif