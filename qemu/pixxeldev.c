#include "qemu/osdep.h"

#include "hw/core/sysbus.h"
#include "system/memory.h"
#include "qemu/log.h"

#define TYPE_PIXXELDEV "pixxeldev"

OBJECT_DECLARE_SIMPLE_TYPE(PixxelDevState, PIXXELDEV)

#define PIXXELDEV_REG_ENABLE  0x00
#define PIXXELDEV_REG_STATUS   0x04

#define PIXXELDEV_MMIO_SIZE    0x1000

struct PixxelDevState {
    SysBusDevice parent_obj;

    MemoryRegion mmio;

    uint32_t enable;
    uint32_t status;
};

static uint64_t pixxeldev_read(void *opaque,
                           hwaddr offset,
                           unsigned size)
{
    PixxelDevState *s = opaque;

    switch (offset) {
    case PIXXELDEV_REG_ENABLE:
        qemu_log_mask(LOG_TRACE,"PIXXELDEV: READ ENABLE\n");
        return s->enable;

    case PIXXELDEV_REG_STATUS:
        qemu_log_mask(LOG_TRACE,"PIXXELDEV: READ STATUS\n");
        return s->status;

    default:
        qemu_log_mask(LOG_TRACE,"PIXXELDEV: invalid READ offset=0x%" HWADDR_PRIx "\n",offset);
        return 0;
    }
}

static void pixxeldev_write(void *opaque,
                        hwaddr offset,
                        uint64_t value,
                        unsigned size)
{
    PixxelDevState *s = opaque;

    switch (offset) {

    case PIXXELDEV_REG_ENABLE:
        s->enable = value;
        s->status = value;
        qemu_log_mask(LOG_TRACE,"PIXXELDEV: WRITE ENABLE = 0x%08" PRIx64 "\n",value);
        break;

    case PIXXELDEV_REG_STATUS:
        s->status = value;
        qemu_log_mask(LOG_TRACE,"PIXXELDEV: WRITE STATUS = 0x%08" PRIx64 "\n",value);
        break;

    default:
        qemu_log_mask(LOG_TRACE,"PIXXELDEV: invalid WRITE ""offset=0x%" HWADDR_PRIx" value=0x%08" PRIx64 "\n",offset, value);
        break;
    }
}

static const MemoryRegionOps pixxeldev_ops = {
    .read = pixxeldev_read,
    .write = pixxeldev_write,

    .endianness = DEVICE_LITTLE_ENDIAN,

    .valid = {
        .min_access_size = 4,
        .max_access_size = 4,
    },
};

static void pixxeldev_init(Object *obj)
{
    PixxelDevState *s = PIXXELDEV(obj);
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);

    memory_region_init_io(&s->mmio,
                          obj,
                          &pixxeldev_ops,
                          s,
                          "pixxeldev-mmio",
                          PIXXELDEV_MMIO_SIZE);

    sysbus_init_mmio(sbd, &s->mmio);
}

static void pixxeldev_reset(DeviceState *dev)
{
    PixxelDevState *s = PIXXELDEV(dev);

    s->enable = 0;
    s->status = 0;
}

static void pixxeldev_class_init(ObjectClass *klass,
                              const void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_set_legacy_reset(dc, pixxeldev_reset);

    dc->desc = "Simple MMIO test device";
}

static const TypeInfo pixxeldev_info = {
    .name = TYPE_PIXXELDEV,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(PixxelDevState),
    .instance_init = pixxeldev_init,
    .class_init = pixxeldev_class_init,
};

static void pixxeldev_register_types(void)
{
    type_register_static(&pixxeldev_info);
}

type_init(pixxeldev_register_types);
