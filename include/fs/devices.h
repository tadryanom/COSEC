#ifndef __DRIVERS_TABLE_H__
#define __DRIVERS_TABLE_H__

#include <stdbool.h>
#include <stdint.h>

#define N_CHR           30
#define N_BLK           12

enum devicetype_e {
    DEV_CHR = 0,
    DEV_BLK = 1,
};

enum char_device_family {
    CHR_VIRT        = 0,
    CHR_MEMDEV      = 1,
    CHR_PTY_MASTER  = 2,
    CHR_PTY_SLAVE   = 3,
    CHR_TTY         = 4,
    CHR_TTY_OTH     = 5,
    CHR_LP          = 6,
    CHR_VCS         = 7,
    CHR_SCSI_TAPE   = 9,
    CHR_MISC        = 10,
    CHR_KBD         = 11,
    CHR_FRAMEBUF    = 29,
};

enum chrmem_device {
    CHRMEM_MEM      = 1,
    CHRMEM_NULL     = 3,
    CHRMEM_PORT     = 4,
    CHRMEM_ZERO     = 5,
    CHRMEM_FULL     = 7,
    CHRMEM_RAND     = 8,
    CHRMEM_URAND    = 9,
    CHRMEM_KMSG     = 11,
};

enum char_virtual_devices {
    CHR0_UNSPECIFIED = 0,
    CHR0_SYSFS       = 1,
};

enum block_device_family {
    BLK_VIRT        = 0,
    BLK_RAM         = 1,
    BLK_FLOPPY      = 2,
    BLK_IDE         = 3,
    BLK_LOOPBACK    = 7,
    BLK_SCSI_DISK   = 8,
    BLK_RAID        = 9,
    BLK_SCSI_CDROM  = 11,
};

typedef enum devicetype_e   devicetype_e;
typedef index_t     majdev_t, mindev_t;

typedef struct devclass  devclass;
typedef struct device    device;

struct devclass {
    devicetype_e  dev_type;
    majdev_t      dev_maj;
    const char   *dev_class_name;

    device  *(*get_device)(mindev_t num);
    void     (*init_devclass)(void);
};


struct device_operations {
    /**
     * \brief  gets a cached block from device in read-only mode
     */
    const char *(*dev_get_roblock)(device *dev, off_t block);

    /**
     * \brief  get a cached block from device in read-write mode
     *         the block will be dirty after `dev_release_block()`.
     */
    char *      (*dev_get_rwblock)(device *dev, off_t block);

    /**
     * \brief  tells a device that the gotten block will not be used anymore.
     */
    int         (*dev_forget_block)(device *dev, off_t block);

    size_t      (*dev_size_of_block)(device *dev);

    off_t       (*dev_size_in_blocks)(device *dev);

    /* mostly character devices operations */
    int     (*dev_read_buf)(device *dev, char *buf, size_t buflen, size_t *written, off_t pos);
    int     (*dev_write_buf)(device *dev, const char *buf, size_t buflen, size_t *written, off_t pos);
    bool    (*dev_has_data)(device *dev);
};

struct device {
    devicetype_e    dev_type;   // character or block
    majdev_t        dev_clss;   // generic driver
    mindev_t        dev_no;     // device index in the family
    const char *    dev_fsname; // how it should appear in /dev

    struct device_operations  dev_ops;  // yep, devopses should care about devices
};

device * device_by_devno(devicetype_e  ty, dev_t devno);

void dev_setup(void);

#endif //__DRIVERS_TABLE_H__
