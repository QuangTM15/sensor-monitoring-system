#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "sms_fops.h"
#include "sms_ioctl.h"
#include "sms_procfs.h"
#include "sms_sensor.h"

static struct sms_device sms_dev;

struct sms_driver_stats sms_stats;

int sms_generate_sensor_data(struct sms_sensor_data *data)
{
    unsigned long tick;

    if (data == NULL)
    {
        return -EINVAL;
    }

    tick = jiffies;

    data->temperature = 20 + (int)(tick % 16UL);
    data->humidity = 40 + (int)((tick / 7UL) % 41UL);

    return 0;
}

unsigned int sms_get_interval(void)
{
    return sms_stats.interval_ms;
}

int sms_set_interval(unsigned int interval_ms)
{
    if ((interval_ms < SMS_INTERVAL_MIN_MS) ||
        (interval_ms > SMS_INTERVAL_MAX_MS))
    {
        return -EINVAL;
    }

    sms_stats.interval_ms = interval_ms;

    return 0;
}

static void sms_stats_init(void)
{
    sms_stats.open_count = 0U;
    sms_stats.read_count = 0U;
    sms_stats.ioctl_count = 0U;
    sms_stats.interval_ms = SMS_INTERVAL_DEFAULT_MS;
}

static int sms_chrdev_init(void)
{
    int result;

    result = alloc_chrdev_region(&sms_dev.dev_number,
                                 0,
                                 1,
                                 SMS_DRIVER_NAME);

    if (result < 0)
    {
        pr_err("[sms_sensor] Failed to allocate device number.\n");

        return result;
    }

    cdev_init(&sms_dev.cdev, &sms_fops);
    sms_dev.cdev.owner = THIS_MODULE;

    result = cdev_add(&sms_dev.cdev,
                      sms_dev.dev_number,
                      1);

    if (result < 0)
    {
        pr_err("[sms_sensor] Failed to add character device.\n");

        unregister_chrdev_region(sms_dev.dev_number, 1);

        return result;
    }

    sms_dev.class = class_create(THIS_MODULE,
                                 SMS_CLASS_NAME);

    if (IS_ERR(sms_dev.class))
    {
        result = PTR_ERR(sms_dev.class);

        pr_err("[sms_sensor] Failed to create device class.\n");

        cdev_del(&sms_dev.cdev);
        unregister_chrdev_region(sms_dev.dev_number, 1);

        return result;
    }

    sms_dev.device = device_create(sms_dev.class,
                                   NULL,
                                   sms_dev.dev_number,
                                   NULL,
                                   SMS_DEVICE_NAME);

    if (IS_ERR(sms_dev.device))
    {
        result = PTR_ERR(sms_dev.device);

        pr_err("[sms_sensor] Failed to create device node.\n");

        class_destroy(sms_dev.class);
        cdev_del(&sms_dev.cdev);
        unregister_chrdev_region(sms_dev.dev_number, 1);

        return result;
    }

    return 0;
}

static void sms_chrdev_exit(void)
{
    device_destroy(sms_dev.class,
                   sms_dev.dev_number);

    class_destroy(sms_dev.class);

    cdev_del(&sms_dev.cdev);

    unregister_chrdev_region(sms_dev.dev_number,
                             1);
}

static int __init sms_sensor_init(void)
{
    int result;

    sms_stats_init();

    result = sms_chrdev_init();

    if (result < 0)
    {
        return result;
    }

    result = sms_procfs_init();

    if (result < 0)
    {
        sms_chrdev_exit();

        return result;
    }

    pr_info("[sms_sensor] Driver initialized.\n");
    pr_info("[sms_sensor] Major number: %d\n",
            MAJOR(sms_dev.dev_number));
    pr_info("[sms_sensor] Device created: /dev/%s\n",
            SMS_DEVICE_NAME);
    pr_info("[sms_sensor] Proc entry created: /proc/%s\n",
            SMS_PROC_NAME);
    pr_info("[sms_sensor] Default interval: %u ms.\n",
            sms_get_interval());

    return 0;
}

static void __exit sms_sensor_exit(void)
{
    sms_procfs_exit();

    sms_chrdev_exit();

    pr_info("[sms_sensor] Driver removed.\n");
}

module_init(sms_sensor_init);
module_exit(sms_sensor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("QuangTM");
MODULE_DESCRIPTION("Sensor Monitoring System character device driver");
MODULE_VERSION("1.1");