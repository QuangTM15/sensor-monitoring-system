#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define SMS_DEVICE_PATH "/dev/sms_sensor"

#define SMS_IOCTL_MAGIC 'S'
#define SMS_IOCTL_SET_INTERVAL _IO(SMS_IOCTL_MAGIC, 1)
#define SMS_IOCTL_GET_INTERVAL _IO(SMS_IOCTL_MAGIC, 2)

int main(int argc,
         char *argv[])
{
    unsigned long requested_interval;
    long current_interval;
    int file_descriptor;
    int result;

    requested_interval = 2000UL;

    if (argc >= 2)
    {
        requested_interval = strtoul(argv[1],
                                     NULL,
                                     10);
    }

    file_descriptor = open(SMS_DEVICE_PATH,
                           O_RDONLY);

    if (file_descriptor < 0)
    {
        fprintf(stderr,
                "Failed to open %s: %s\n",
                SMS_DEVICE_PATH,
                strerror(errno));

        return 1;
    }

    result = ioctl(file_descriptor,
                   SMS_IOCTL_SET_INTERVAL,
                   requested_interval);

    if (result < 0)
    {
        fprintf(stderr,
                "SET_INTERVAL failed: %s\n",
                strerror(errno));

        close(file_descriptor);

        return 1;
    }

    current_interval = ioctl(file_descriptor,
                             SMS_IOCTL_GET_INTERVAL,
                             0UL);

    if (current_interval < 0)
    {
        fprintf(stderr,
                "GET_INTERVAL failed: %s\n",
                strerror(errno));

        close(file_descriptor);

        return 1;
    }

    printf("Requested interval: %lu ms\n",
           requested_interval);

    printf("Current interval  : %ld ms\n",
           current_interval);

    close(file_descriptor);

    return 0;
}