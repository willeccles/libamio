#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/types.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "libamio-i2c.h"

typedef struct {
  int fd;
  uint8_t devnum;
} i2cdevice;

// timeout should be in 10s of milliseconds, i.e. 2 = 20ms
static int set_timeout(i2cdevice* dev, int timeout);

static int i2c_rdwr(i2cdevice* dev, struct i2c_msg* message);

I2C_Handle I2C_open(uint8_t devnum) {
  i2cdevice* dev = (i2cdevice*)malloc(sizeof(i2cdevice));
  if (NULL == dev) {
    return NULL;
  }

  dev->devnum = devnum;

  char devpath[35];

  snprintf(devpath, 35, "/dev/i2c-%hhu", dev->devnum);

  if (0 != access(devpath, F_OK)) {
    free(dev);
    return NULL;
  }

  dev->fd = open(devpath, O_SYNC | O_RDWR | O_CLOEXEC);

  if (dev->fd < 0) {
    free(dev);
    return NULL;
  }

  return (I2C_Handle)dev;
}

int I2C_transfer(I2C_Handle handle, I2C_Transaction* transaction) {
  if (handle == NULL || transaction == NULL) {
    return EXIT_FAILURE;
  }

  if (transaction->writeCount == 0 && transaction->readCount == 0) {
    return EXIT_SUCCESS; // we technically did what they asked for
  }

  i2cdevice* dev = (i2cdevice*)handle;

  if (dev->fd < 0) {
    return EXIT_FAILURE;
  }

  if ((transaction->readCount && transaction->readBuf == NULL)
      || (transaction->writeCount && transaction->writeBuf == NULL)) {
    return EXIT_FAILURE;
  }

  // set timeout
  if (EXIT_SUCCESS != set_timeout(dev, transaction->timeout / 10)) {
    return EXIT_FAILURE;
  }

  struct i2c_msg msg;

  // start by sending data if necessary
  if (transaction->writeCount) {
    msg = (struct i2c_msg){
      .addr = transaction->slaveAddress,
          .flags = 0,
          .len = transaction->writeCount,
          .buf = transaction->writeBuf,
    };

    if (EXIT_FAILURE == i2c_rdwr(dev, &msg)) {
      return EXIT_FAILURE;
    }
  }

  if (transaction->readCount) {
    msg = (struct i2c_msg){
      .addr = transaction->slaveAddress,
          .flags = I2C_M_RD,
          .len = transaction->readCount,
          .buf = transaction->readBuf,
    };

    if (EXIT_FAILURE == i2c_rdwr(dev, &msg)) {
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}

void I2C_close(I2C_Handle handle) {
  if (handle != NULL) {
    i2cdevice* dev = (i2cdevice*)handle;
    if (dev->fd >= 0) {
      close(dev->fd);
    }
    free(dev);
  }
}

// timeout should be in 10s of milliseconds, i.e. 2 = 20ms
static int set_timeout(i2cdevice* dev, int timeout) {
  if (dev == NULL || timeout < 0) {
    return EXIT_FAILURE;
  }

  if (ioctl(dev->fd, I2C_TIMEOUT, timeout) < 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

static int i2c_rdwr(i2cdevice* dev, struct i2c_msg* message) {
  if (dev == NULL || message == NULL) {
    return EXIT_FAILURE;
  }

  struct i2c_rdwr_ioctl_data packets = (struct i2c_rdwr_ioctl_data){
    // this is a bit abusive; it's expecting an array of n messages
    .msgs = message,
        .nmsgs = 1,
  };

  if (ioctl(dev->fd, I2C_RDWR, &packets) < 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
