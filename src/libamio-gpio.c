#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdatomic.h>
#include <stdbool.h>

#include "libamio-gpio.h"

typedef struct {
    GPIO_CallbackFxn fn;
    pthread_t *thread;
} gpiocallback;

typedef struct {
    unsigned int pin;       //!< Pin number
    int fd_val;             //!< Value file descriptor
    struct pollfd fd_poll;  //!< \c pollfd used for polling
    atomic_bool int_en;     //!< Interrupts enabled flag
    gpiocallback cb;        //!< Callback for interrupts
} gpiodev;

// strings used for setting the direction
char* direction_strs[] = { "in", "out" };
// strings used to set values
char* value_strs[] = { "0", "1" };
// strings used to set edges
char* edge_strs[] = { "none", "rising", "falling", "both" };

// function run by the polling threads
static void* _pollthreadfunc(void* dev);

// set the edge for a pin
static int set_edge(int pin, char* edgestr);

GPIO_Handle GPIO_initpin(unsigned int pin, GPIO_direction direction) {
    if (direction != INPUT && direction != OUTPUT) {
        return NULL;
    }

    char tmppath[128];
    int fd;

    snprintf(tmppath, 128, "/sys/class/gpio/gpio%d/value", pin);
    if (0 != access(tmppath, F_OK)) {
        fd = open("/sys/class/gpio/export", O_SYNC | O_WRONLY);
        if (fd < 0) {
            return NULL;
        }

        char tmp[16];
        snprintf(tmp, 16, "%d", pin);

        if (write(fd, tmp, 16) < 0) {
            return NULL;
        }

        if (close(fd) < 0) {
            return NULL;
        }
    }

    snprintf(tmppath, 128, "/sys/class/gpio/gpio%d", pin);

    if (access(tmppath, F_OK) != 0) {
        return NULL;
    }

    gpiodev* newdev = (gpiodev*)malloc(sizeof(gpiodev));
    if (NULL == newdev) {
        return NULL;
    }

    snprintf(tmppath, 128, "/sys/class/gpio/gpio%d/value", pin);

    newdev->fd_val = open(tmppath, O_SYNC | O_RDWR);
    if (newdev->fd_val < 0) {
        goto errorafterinit;
    }

    newdev->pin = pin;
    newdev->cb.thread = NULL;
    newdev->cb.fn = NULL;
    newdev->int_en = false;

    newdev->fd_poll.fd = newdev->fd_val;
    newdev->fd_poll.events = POLLPRI;
    newdev->fd_poll.revents = 0;

    // set the direction of the pin

    snprintf(tmppath, 128, "/sys/class/gpio/gpio%d/direction", newdev->pin);
    fd = open(tmppath, O_SYNC | O_WRONLY);
    if (fd < 0) {
        goto errorafterinit;
    }
    
    if (write(fd, direction_strs[direction], strlen(direction_strs[direction])) < 0) {
        close(fd);
        goto errorafterinit;
    }

    if (close(fd) < 0) {
        goto errorafterinit;
    }

    return (GPIO_Handle)newdev;

errorafterinit:
    free(newdev);
    return NULL;
}

void GPIO_release(GPIO_Handle pin) {
    if (pin != NULL) {
        gpiodev* dev = (gpiodev*)pin;
        if (dev->int_en) {
            dev->int_en = false;
            if (dev->cb.thread != NULL) {
                pthread_cancel(*(dev->cb.thread));
                pthread_join(*(dev->cb.thread), NULL);
                free(dev->cb.thread);
            }
        }
        free(dev);
    }
}

int GPIO_write(GPIO_Handle handle, GPIO_state val) {
    if (handle == NULL || (val != HIGH && val != LOW)) {
        return EXIT_FAILURE;
    }

    gpiodev* dev = (gpiodev*)handle;

    if (write(dev->fd_val, value_strs[val], 1) < 0) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

GPIO_state GPIO_read(GPIO_Handle handle) {
    if (handle == NULL) {
        return -1;
    }
    
    char state;

    gpiodev* dev = (gpiodev*)handle;

    lseek(dev->fd_val, 0, SEEK_SET);

    if (read(dev->fd_val, &state, 1) != 1) {
        return -1;
    }

    switch(state) {
        case '0':
            return LOW;
        case '1':
            return HIGH;
        default:
            return -1;
    }
}

int GPIO_setEdge(GPIO_Handle handle, GPIO_edge edge) {
    gpiodev* dev = (gpiodev*)handle;

    char tmpstr[256] = {0};
    snprintf(tmpstr, 256, "/sys/class/gpio/gpio%d/edge", dev->pin);
    int fd = open(tmpstr, O_SYNC | O_WRONLY);
    if (fd < 0) {
        return EXIT_FAILURE;
    }

    char* str = edge_strs[edge];
    ssize_t r = write(fd, str, strlen(str));
    if (r < 0) {
        close(fd);
        return EXIT_FAILURE;
    }

    close(fd);

    return EXIT_SUCCESS;
}

int GPIO_enableInt(GPIO_Handle handle) {
    if (handle == NULL) {
        return EXIT_FAILURE;
    }

    gpiodev* dev = (gpiodev*)handle;

    if (dev->int_en) {
        return EXIT_SUCCESS;
    }

    pthread_t *pollthread = (pthread_t*)malloc(sizeof(pthread_t));

    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

    dev->cb.thread = pollthread;

    if (0 != pthread_create(pollthread, NULL, _pollthreadfunc, handle)) {
        free(dev->cb.thread);

        return EXIT_FAILURE;
    }

    dev->int_en = true;

    return EXIT_SUCCESS;
}

int GPIO_disableInt(GPIO_Handle handle) {
    if (handle == NULL) {
        return EXIT_FAILURE;
    }

    gpiodev* dev = (gpiodev*)handle;
    dev->int_en = false;

    pthread_cancel(*(dev->cb.thread));

    free(dev->cb.thread);

    dev->cb.thread = NULL;

    return EXIT_SUCCESS;
}

int GPIO_setCallback(GPIO_Handle handle, GPIO_CallbackFxn fxn) {
    if (handle == NULL || fxn == NULL) {
        return EXIT_FAILURE;
    }

    gpiodev* dev = (gpiodev*)handle;

    if (dev->int_en == false) {
        dev->cb.fn = fxn;
    } else {
        dev->int_en = false;
        dev->cb.fn = fxn;
        dev->int_en = true;
    }

    return EXIT_SUCCESS;
}

static void* _pollthreadfunc(void* dev) {
    if (dev == NULL) {
        return NULL;
    }

    gpiodev* _dev = (gpiodev*)dev;

    int ret;
    char c;

    while (1) {
        if (_dev->int_en) {
            // read to clear interrupt
            ret = lseek(_dev->fd_val, 0, SEEK_SET);
            ret = read(_dev->fd_val, &c, 1);

            // now poll
            ret = poll(&_dev->fd_poll, 1, -1);
            if (ret == 1 && _dev->fd_poll.revents & POLLPRI) {
                // clear interrupt
                read(_dev->fd_val, &c, 1);
                
                // perform callback
                _dev->cb.fn();
            }
        }
    }
}
