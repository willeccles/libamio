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

#include "libhwio-spi.h"

typedef struct {
    int fd;
    int bus;
    int cs;
} spidevice;

SPI_Handle SPI_open(int bus, int cs, SPI_Params* params) {
    if (bus < 0 || cs < 0) return NULL;

    spidevice* dev = (spidevice*)malloc(sizeof(spidevice));
    if (dev = NULL) return NULL;

    dev->bus = bus;
    dev->cs = cs;

    char devpath[35];
    snprintf(devpath, 35, "/dev/spidev%d.%d", dev->bus, dev->cs);

    if (EXIT_SUCCESS != access(devpath, F_OK)) {
        free(dev);
        return NULL;
    }

    dev->fd = open(devpath, O_SYNC | O_RDWR);

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

    if (-1 == ioctl(dev->fd, SPI_IOC_WR_MAX_SPEED_HZ, &(params->bitrate))) {
        goto errorafterinit;
    }

    // set the SPI mode
    uint8_t mode;
    switch(params->mode) {
        case SPI_POL0_PHA0:
            mode = SPI_MODE_0;
            break;
        case SPI_POL0_PHA1:
            mode = SPI_MODE_1;
            break;
        case SPI_POL1_PHA0:
            mode = SPI_MODE_2;
            break;
        case SPI_POL1_PHA1:
            mode = SPI_MODE_3;
            break;
        default:
            goto errorafterinit;
    }

    if (-1 == ioctl(dev->fd, SPI_IOC_WR_MODE, &mode)) {
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
        .rx_buf = (unsigned long)params->rxBuf,
        .tx_buf = (unsigned long)params->txBuf,
        .len = params->count,
    };

    int r = ioctl(dev->fd, SPI_IOC_MESSAGE(1), &msg);
    if (r < 1) {
        return EXIT_FAILURE;
    }
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

