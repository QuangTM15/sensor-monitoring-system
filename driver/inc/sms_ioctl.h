#ifndef SMS_IOCTL_H
#define SMS_IOCTL_H

#include <linux/fs.h>
#include <linux/ioctl.h>

#define SMS_IOCTL_MAGIC 'S'

#define SMS_IOCTL_SET_INTERVAL _IO(SMS_IOCTL_MAGIC, 1)
#define SMS_IOCTL_GET_INTERVAL _IO(SMS_IOCTL_MAGIC, 2)

#define SMS_INTERVAL_MIN_MS 100U
#define SMS_INTERVAL_MAX_MS 60000U
#define SMS_INTERVAL_DEFAULT_MS 1000U

long sms_ioctl(struct file *file,
               unsigned int command,
               unsigned long argument);

#endif