#include <fcntl.h>
#include <linux/ioctl.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "libamio-spi.h"

typedef struct {
  int fd;
  int bus;
  int cs;
} spidevice;

SPI_Handle SPI_open(int bus, int cs, SPI_Params* params) {
  if (bus < 0 || cs < 0) return NULL;

  char devpath[35];
  snprintf(devpath, 35, "/dev/spidev%d.%d", bus, cs);

  spidevice* dev = SPI_open_path(devpath, params);
  if (dev) {
    dev->bus = bus;
    dev->cs = cs;
  }

  return (SPI_Handle)dev;
}

SPI_Handle SPI_open_path(const char* path, SPI_Params* params) {
  spidevice* dev = (spidevice*)malloc(sizeof(spidevice));
  if (dev == NULL) return NULL;

  dev->bus = -1;
  dev->cs = -1;

  if (EXIT_SUCCESS != access(path, F_OK)) {
    free(dev);
    return NULL;
  }

  dev->fd = open(path, O_SYNC | O_RDWR | O_CLOEXEC);

  if (dev->fd < 0) {
    free(dev);
    return NULL;
  }

  // set the bits per word
  if (params->wordsize == 0) {
    goto errorafterinit;
  }

  if (-1 == ioctl(dev->fd, SPI_IOC_WR_BITS_PER_WORD, &(params->wordsize))) {
    goto errorafterinit;
  }

  // set the bitrate
  if (params->bitrate == 0) {
    goto errorafterinit;
  }

  // set the SPI speed
  if (-1 == ioctl(dev->fd, SPI_IOC_WR_MAX_SPEED_HZ, &(params->bitrate))) {
    goto errorafterinit;
  }

  // set the SPI mode
  if (-1 == ioctl(dev->fd, SPI_IOC_WR_MODE32, &(params->mode))) {
    goto errorafterinit;
  }

  // if we have been successful the whole time,
  // we can return a handle to the SPI device
  return (SPI_Handle)dev;

errorafterinit:
  close(dev->fd);
  free(dev);
  return NULL;
}

int SPI_transfer(SPI_Handle handle, SPI_Transaction* transaction) {
  if (handle == NULL || transaction == NULL || transaction->count < 0) {
    return EXIT_FAILURE;
  }

  if (transaction->count == 0) {
    // just call this successful, since technically we didn't fail at anything
    return EXIT_SUCCESS;
  }

  spidevice* dev = (spidevice*)handle;

  // we don't have to do anything special to accomodate for rxBuf being NULL,
  // since making .rx_buf == NULL already does this for us
  struct spi_ioc_transfer msg = (struct spi_ioc_transfer){
    .rx_buf = (unsigned long)transaction->rxBuf,
        .tx_buf = (unsigned long)transaction->txBuf,
        .len = transaction->count,
  };

  int r = ioctl(dev->fd, SPI_IOC_MESSAGE(1), &msg);
  if (r < 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int SPI_setMode(SPI_Handle handle, SPI_MODE mode) {
  if (handle == NULL) {
    return EXIT_FAILURE;
  }

  int r = ioctl(((spidevice*)handle)->fd, SPI_IOC_WR_MODE, &mode);
  if (r < 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void SPI_close(SPI_Handle handle) {
  if (handle != NULL) {
    spidevice* d = (spidevice*)handle;
    if (d->fd >= 0) {
      close(d->fd);
    }
    free(d);
  }
}

ssize_t SPI_getMaxBufSize() {
  int fd = open("/sys/module/spidev/parameters/bufsiz", O_RDONLY);
  if (fd < 0) {
    return -1;
  }

  char buf[32] = {0};
  if (read(fd, buf, 31) < 0) {
    close(fd);
    return -1;
  }

  close(fd);

  ssize_t val = -1;
  if (sscanf(buf, "%zd", &val) < 1) {
    val = -1;
  }

  return val;
}
