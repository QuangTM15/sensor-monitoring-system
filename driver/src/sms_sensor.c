/*
 * Project : Sensor Monitoring System
 * File    : sms_driver.c
 * Brief   : Kernel module entry and exit.
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "sms_sensor.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("To Minh Quang");
MODULE_DESCRIPTION("Sensor Monitoring System Driver");
MODULE_VERSION("1.0");

/*---------------------------------------------------------------------------*/
/* Module Init                                                               */
/*---------------------------------------------------------------------------*/

static int __init sms_driver_init(void)
{
    pr_info("[%s] Driver initialized.\n", SMS_DRIVER_NAME);

    return 0;
}

/*---------------------------------------------------------------------------*/
/* Module Exit                                                               */
/*---------------------------------------------------------------------------*/

static void __exit sms_driver_exit(void)
{
    pr_info("[%s] Driver removed.\n", SMS_DRIVER_NAME);
}

module_init(sms_driver_init);
module_exit(sms_driver_exit);