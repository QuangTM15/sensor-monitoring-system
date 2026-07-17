#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include "sms_procfs.h"
#include "sms_sensor.h"

static struct proc_dir_entry *sms_proc_entry;

static int sms_proc_show(struct seq_file *file,
                         void *data)
{
    (void)data;

    seq_printf(file,
               "driver_name: %s\n",
               SMS_DRIVER_NAME);

    seq_printf(file,
               "device_node: /dev/%s\n",
               SMS_DEVICE_NAME);

    seq_printf(file,
               "interval_ms: %u\n",
               sms_stats.interval_ms);

    seq_printf(file,
               "open_count: %u\n",
               sms_stats.open_count);

    seq_printf(file,
               "read_count: %u\n",
               sms_stats.read_count);

    seq_printf(file,
               "ioctl_count: %u\n",
               sms_stats.ioctl_count);

    return 0;
}

static int sms_proc_open(struct inode *inode,
                         struct file *file)
{
    (void)inode;

    return single_open(file,
                       sms_proc_show,
                       NULL);
}

static const struct proc_ops sms_proc_ops =
    {
        .proc_open = sms_proc_open,
        .proc_read = seq_read,
        .proc_lseek = seq_lseek,
        .proc_release = single_release};

int sms_procfs_init(void)
{
    sms_proc_entry = proc_create(SMS_PROC_NAME,
                                 0444,
                                 NULL,
                                 &sms_proc_ops);

    if (sms_proc_entry == NULL)
    {
        pr_err("[sms_sensor] Failed to create /proc/%s.\n",
               SMS_PROC_NAME);

        return -ENOMEM;
    }

    return 0;
}

void sms_procfs_exit(void)
{
    if (sms_proc_entry != NULL)
    {
        proc_remove(sms_proc_entry);
        sms_proc_entry = NULL;
    }
}