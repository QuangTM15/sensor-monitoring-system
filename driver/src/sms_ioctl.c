#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/kernel.h>

#include "sms_ioctl.h"
#include "sms_sensor.h"

long sms_ioctl(struct file *file,
               unsigned int command,
               unsigned long argument)
{
    long result;

    (void)file;

    sms_stats.ioctl_count++;

    result = 0;

    switch (command)
    {
    case SMS_IOCTL_SET_INTERVAL:
        result = sms_set_interval((unsigned int)argument);

        if (result == 0)
        {
            pr_info("[sms_sensor] Interval changed to %lu ms.\n",
                    argument);
        }
        else
        {
            pr_err("[sms_sensor] Invalid interval: %lu ms.\n",
                   argument);
        }
        break;

    case SMS_IOCTL_GET_INTERVAL:
        result = (long)sms_get_interval();
        break;

    default:
        result = -ENOTTY;
        break;
    }

    return result;
}