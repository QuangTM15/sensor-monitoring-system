#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define SMS_DEVICE_PATH "/dev/sms_sensor"

#define SMS_IOCTL_MAGIC 'S'
#define SMS_IOCTL_SET_INTERVAL _IO(SMS_IOCTL_MAGIC, 1)
#define SMS_IOCTL_GET_INTERVAL _IO(SMS_IOCTL_MAGIC, 2)

#define SMS_DEFAULT_INTERVAL_MS 2000UL

static int parse_interval(const char *text,
                          unsigned long *interval)
{
    char *end_pointer;
    unsigned long parsed_value;

    if ((text == NULL) || (interval == NULL))
    {
        return -1;
    }

    errno = 0;
    end_pointer = NULL;

    parsed_value = strtoul(text,
                           &end_pointer,
                           10);

    if ((errno != 0) ||
        (end_pointer == text) ||
        (*end_pointer != '\0') ||
        (parsed_value > UINT_MAX))
    {
        return -1;
    }

    *interval = parsed_value;

    return 0;
}

int main(int argc,
         char *argv[])
{
    unsigned long requested_interval;
    long current_interval;
    int file_descriptor;
    int result;

    requested_interval = SMS_DEFAULT_INTERVAL_MS;

    if (argc > 2)
    {
        fprintf(stderr,
                "Usage: %s [interval_ms]\n",
                argv[0]);

        return 1;
    }

    if (argc == 2)
    {
        result = parse_interval(argv[1],
                                &requested_interval);

        if (result < 0)
        {
            fprintf(stderr,
                    "Invalid interval value: %s\n",
                    argv[1]);

            return 1;
        }
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