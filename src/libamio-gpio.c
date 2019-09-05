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

#include "libamio-gpio.h"

GPIO_Handle GPIO_initpin(unsigned int pin, GPIO_direction direction) {}

void GPIO_releasepin(GPIO_Handle pin) {}

void GPIO_write(GPIO_Handle handle, int val) {}

GPIO_state GPIO_read(GPIO_Handle handle) {}

void GPIO_enableInt(GPIO_Handle handle) {}

void GPIO_setCallback(GPIO_Handle handle, GPIO_CallbackFxn fxn) {}
