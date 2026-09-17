#include "pixxeldriver.h"

pixxelDevice_t *pixxelDeviceHead, *pixxelDeviceTail;

int no_of_pixxelDevices = DEFAULT_PIXXEL_DEVICES;

int pixxelDriver_major = PIXXEL_DEFAULT_MAJOR;

int result;

dev_t dev;

static struct class *pixxelDriver_class;

static DEFINE_IDA(pixxel_ida);

module_param(pixxelDriver_major, int, S_IRUGO);
module_param(no_of_pixxelDevices, int, S_IRUGO);

static int pixxel_open(struct inode *inode, struct file *file)
{
    pixxelDevice_t *pixxelDevice;

    pixxelDevice = container_of(inode->i_cdev,
                                pixxelDevice_t,
                                controlFile);

    file->private_data = pixxelDevice;
    printk(KERN_INFO "pixxel_Driver: pixxel_open called\n");

    return 0;
}

static ssize_t pixxel_read(struct file *file,
                          char __user *buf,
                          size_t count,
                          loff_t *ppos)
{
    printk(KERN_INFO "pixxelDriver: pixxel_read called, count: %zu\n", count);

    pixxelDevice_t *pixxelDevice;

    pixxelDevice = (pixxelDevice_t *)file->private_data;

    u32 status = ioread32(pixxelDevice->base + REG_STATUS);

    if (count < sizeof(status)) {
        return -EINVAL;
    }
        
    if (copy_to_user(buf, &status, sizeof(status))) {
        return -EFAULT;
    }

    return sizeof(status);
}

static ssize_t pixxel_write(struct file *file,
                       const char __user *buf,
                       size_t count,
                       loff_t *ppos)
{
    printk(KERN_INFO "pixxelDriver: pixxel_write called, count: %zu\n", count);

    pixxelDevice_t *pixxelDevice;

    pixxelDevice = (pixxelDevice_t *)file->private_data;

    u32 status;

    copy_from_user(&status, buf, sizeof(status));

    iowrite32(status, pixxelDevice->base + REG_ENABLE);

    return sizeof(status);
}

struct file_operations pixxel_fops = {
    .owner = THIS_MODULE,
    .open = pixxel_open,
    .read = pixxel_read,
    .write = pixxel_write,
};

static int pixxel_device_probe(struct platform_device *pdev)
{
    struct resource *res;
    void __iomem *base;
    u32 status;
    int minorNumber;

    pixxelDevice_t *pixxelDevice;

    pr_info("pixxelDevice: probe called\n");

    //get a minor number for the device
    minorNumber = ida_alloc(&pixxel_ida, GFP_KERNEL);
    if (minorNumber < 0) {
        printk(KERN_ALERT "pixxelDevice: Failed to allocate minor number\n");
        return minorNumber;
    }

    /* Get memory resource from Device Tree */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res){
        ida_free(&pixxel_ida, minorNumber);
        return -ENODEV;
    }

    printk(KERN_INFO "pixxelDevice: physical address obtained\n");

    /* Map registers */
    base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(base)) {
        ida_free(&pixxel_ida, minorNumber);
        return PTR_ERR(base);
    }
        

    //allocate memory for pixxelDevice_t structure
    pixxelDevice = devm_kzalloc(&pdev->dev, sizeof(pixxelDevice_t), GFP_KERNEL);
    if (!pixxelDevice) {
        printk(KERN_ALERT "pixxelDevice: Failed to allocate memory for pixxelDevice\n");
        ida_free(&pixxel_ida, minorNumber);
        return -ENOMEM;
    }

    //set the base address and minor number in the pixxelDevice structure
    pixxelDevice->base = base;
    pixxelDevice->minorNumber = minorNumber;

    platform_set_drvdata(pdev, pixxelDevice);

    //initialize the character device
    cdev_init(&pixxelDevice->controlFile, &pixxel_fops);
    cdev_add(&pixxelDevice->controlFile, MKDEV(pixxelDriver_major, minorNumber), 1);

    //create the device file in /dev
    device_create(pixxelDriver_class, &pdev->dev, MKDEV(pixxelDriver_major, minorNumber), pixxelDevice, "pixxelDevice%d", minorNumber);

    return 0;
}

static void pixxel_device_remove(struct platform_device *pdev)
{
    pixxelDevice_t *pixxelDevice;

    pixxelDevice = platform_get_drvdata(pdev);

    device_destroy(pixxelDriver_class, MKDEV(pixxelDriver_major, pixxelDevice->minorNumber));
    cdev_del(&pixxelDevice->controlFile);
    ida_free(&pixxel_ida, pixxelDevice->minorNumber);

    pr_info("pixxelDevice: remove called\n");
    return;
}

static const struct of_device_id my_of_match[] = {
    { .compatible = "pixxel,virt-dev" },
    { }
};

MODULE_DEVICE_TABLE(of, my_of_match);

static struct platform_driver pixxel_driver = {
    .probe = pixxel_device_probe,
    .remove = pixxel_device_remove,

    .driver = {
        .name = "pixxel_driver",
        .of_match_table = my_of_match,
    },
};

static int __init pixxel_init(void){
    printk(KERN_INFO "pixxelDriver: Test module loaded\n");

    if (no_of_pixxelDevices > MAX_PIXXEL_DEVICES) {
        printk(KERN_ALERT "pixxelDriver: Number of devices exceeds maximum limit\n");
        return -EINVAL;
    }

    //allocate memory for pixxelDevice
    // pixxelDevice = kmalloc(no_of_pixxelDevices * sizeof(pixxelDevice_t), GFP_KERNEL);
    // if (!pixxelDevice) {
    //     printk(KERN_ALERT "pixxelDriver: Failed to allocate memory for pixxelDevice");
    //     return -ENOMEM;
    // }

    //get major number and range of minor numbers

    result = alloc_chrdev_region(&dev, 0, no_of_pixxelDevices, PIXXEL_DRIVER_NAME);
    if (result < 0) {
        printk(KERN_ALERT "pixxelDriver: Failed to allocate major number\n");
        return result;
    }

    pixxelDriver_major = MAJOR(dev);

    //create the device class
    pixxelDriver_class = class_create(PIXXEL_DRIVER_NAME);
    if (IS_ERR(pixxelDriver_class)) {
        printk(KERN_ALERT "pixxelDriver: Failed to create device class\n");
        unregister_chrdev_region(dev, no_of_pixxelDevices);
        return PTR_ERR(pixxelDriver_class);
    }

    

    //register the platform driver
    result = platform_driver_register(&pixxel_driver);
    if (result < 0) {
        printk(KERN_ALERT "pixxelDriver: Failed to register platform driver\n");
        class_destroy(pixxelDriver_class);
        unregister_chrdev_region(dev, no_of_pixxelDevices);
        return result;
    }

    //set all pointers to NULL
    // pixxelDeviceHead = NULL;
    // pixxelDeviceTail = NULL;
    // pixxelDevice = NULL;

    printk(KERN_INFO "pixxelDriver: Character device registered successfully\n");
    return 0;
}

static void __exit pixxel_exit(void){
    platform_driver_unregister(&pixxel_driver);
    class_destroy(pixxelDriver_class);
    
    unregister_chrdev_region(dev, no_of_pixxelDevices);
    ida_destroy(&pixxel_ida);
    
    printk(KERN_INFO "pixxelDriver: Test module unloaded\n");
}

module_init(pixxel_init);
module_exit(pixxel_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Srinidhi");
MODULE_DESCRIPTION("Example kernel module");