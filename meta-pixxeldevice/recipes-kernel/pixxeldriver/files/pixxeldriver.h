#ifndef PIXXELDRIVER_H
#define PIXXELDRIVER_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/uaccess.h>  
#include <linux/cdev.h>      // struct cdev, cdev_init(), cdev_add(), cdev_del()
#include <linux/device.h>    // class_create(), device_create()
#include <linux/platform_device.h>
#include <linux/io.h>       // ioremap(), iounmap()
#include <linux/slab.h>     // kmalloc(), kfree()
#include <linux/errno.h>    // error codes
#include <linux/idr.h>      // IDA (ID Allocator)
#include <linux/mod_devicetable.h>

#define PIXXEL_DRIVER_NAME "pixxelDriver"

#define DEFAULT_PIXXEL_DEVICES 4

#define MAX_PIXXEL_DEVICES 16

#define PIXXEL_DEFAULT_MAJOR 0

#define REG_ENABLE 0x00
#define REG_STATUS 0x04

typedef struct pixxelDevice_t pixxelDevice_t;

struct pixxelDevice_t {
    void __iomem *base;
    u8 minorNumber;

    struct cdev controlFile;

    pixxelDevice_t *next;
};


#endif
