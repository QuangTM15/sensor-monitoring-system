#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "sms_fops.h"
#include "sms_sensor.h"
#include <linux/jiffies.h>

static struct sms_device sms_dev;

int sms_generate_sensor_data(struct sms_sensor_data *data)
{
    unsigned long tick;

    if (data == NULL)
    {
        return -EINVAL;
    }

    tick = jiffies;

    data->temperature = 20 + (tick % 16);
    data->humidity = 40 + ((tick / 7) % 41);

    return 0;
}

static int __init sms_sensor_init(void)
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

    result = cdev_add(&sms_dev.cdev, sms_dev.dev_number, 1);
    if (result < 0)
    {
        pr_err("[sms_sensor] Failed to add cdev.\n");
        unregister_chrdev_region(sms_dev.dev_number, 1);
        return result;
    }

    sms_dev.class = class_create(THIS_MODULE, SMS_CLASS_NAME);
    if (IS_ERR(sms_dev.class))
    {
        result = PTR_ERR(sms_dev.class);
        pr_err("[sms_sensor] Failed to create class.\n");

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
        pr_err("[sms_sensor] Failed to create device.\n");

        class_destroy(sms_dev.class);
        cdev_del(&sms_dev.cdev);
        unregister_chrdev_region(sms_dev.dev_number, 1);

        return result;
    }

    pr_info("[sms_sensor] Driver initialized.\n");
    pr_info("[sms_sensor] Major number: %d\n",
            MAJOR(sms_dev.dev_number));
    pr_info("[sms_sensor] Device created: /dev/%s\n",
            SMS_DEVICE_NAME);

    return 0;
}

static void __exit sms_sensor_exit(void)
{
    device_destroy(sms_dev.class, sms_dev.dev_number);
    class_destroy(sms_dev.class);
    cdev_del(&sms_dev.cdev);
    unregister_chrdev_region(sms_dev.dev_number, 1);

    pr_info("[sms_sensor] Driver removed.\n");
}

module_init(sms_sensor_init);
module_exit(sms_sensor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("To Minh Quang - HE200224");
MODULE_DESCRIPTION("Sensor Monitoring System character device driver");
MODULE_VERSION("1.0");