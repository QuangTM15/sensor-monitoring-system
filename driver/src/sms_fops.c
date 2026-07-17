#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "sms_fops.h"
#include "sms_sensor.h"

static int sms_open(struct inode *inode, struct file *file)
{
    pr_info("[sms_sensor] Device opened.\n");

    return 0;
}

static int sms_release(struct inode *inode, struct file *file)
{
    pr_info("[sms_sensor] Device closed.\n");

    return 0;
}

static ssize_t sms_read(struct file *file,
                        char __user *user_buffer,
                        size_t buffer_length,
                        loff_t *offset)
{
    struct sms_sensor_data data;
    char kernel_buffer[64];
    int message_length;
    int result;

    result = sms_generate_sensor_data(&data);
    if (result < 0)
    {
        return result;
    }

    message_length = scnprintf(kernel_buffer,
                               sizeof(kernel_buffer),
                               "temperature=%d humidity=%d\n",
                               data.temperature,
                               data.humidity);

    return simple_read_from_buffer(user_buffer,
                                   buffer_length,
                                   offset,
                                   kernel_buffer,
                                   message_length);
}

const struct file_operations sms_fops =
    {
        .owner = THIS_MODULE,
        .open = sms_open,
        .read = sms_read,
        .release = sms_release};