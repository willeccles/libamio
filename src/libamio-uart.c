#include <errno.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#include "libamio-uart.h"

//! Contains all the baud values. Can be indexed by UART_BAUD values.
static const int BAUDS[] = {
    B0,
    B50,
    B75,
    B110,
    B134,
    B150,
    B200,
    B300,
    B600,
    B1200,
    B1800,
    B2400,
    B4800,
    B9600,
    B19200,
    B38400,
    B115200,
};

/*!
 * \brief Sets up an interface's attributes.
 * \author User 'sawdust' on StackOverflow: https://stackoverflow.com/a/38318768/2712525
 * \param fd the interface's file descriptor
 * \param speed the requested BAUD rate
 * \return 0 if no error, else -1. Sets errno on error.
 */
static int set_interface_attribs(int fd, UART_BAUD speed);

// convert from an int to the nearest UARD_BAUD value
static UART_BAUD get_baud_value(unsigned int baud);

UART_Handle UART_Open(const char* dev, unsigned int baud) {
    int fd = open(dev, O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK); // no CTTY makes sure the tty doesn't start controlling this program
    if (fd < 0) {
        return -1;
    }

    if (0 != set_interface_attribs(fd, get_baud_value(baud))) {
        return -1;
    }

    return fd;
}

ssize_t UART_Send(UART_Handle dev, const char* msg, size_t len) {
    return write(dev, msg, len);
}

ssize_t UART_Read(UART_Handle dev, char* buf, size_t len) {
    if (len == 0) return 0;
    return read(dev, buf, len);
}

static int set_interface_attribs(int fd, UART_BAUD speed) {
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        return -1;
    }

    cfmakeraw(&tty);
    
    cfsetospeed(&tty, BAUDS[speed]);
    cfsetispeed(&tty, BAUDS[speed]);

    tty.c_cflag &= ~(PARENB | CSTOPB | CSIZE | CRTSCTS);
    tty.c_cflag |= (CLOCAL | CREAD | CS8);

    tty.c_oflag &= ~OPOST;
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 5;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        return -1;
    }
    return 0;
}

static UART_BAUD get_baud_value(unsigned int baud) {
    if (baud <= UART_115200) {
        return baud;
    }

#define BAUD_THRESHOLD(p,n) {\
    if (baud <= ((n)-(p))/2 && baud >= (p)) return UART_##p;\
    if (baud > ((n)-(p))/2 && baud <= (n)) return UART_##n;}
    BAUD_THRESHOLD(0, 50);
    BAUD_THRESHOLD(50, 75);
    BAUD_THRESHOLD(75, 110);
    BAUD_THRESHOLD(110, 134);
    BAUD_THRESHOLD(134, 150);
    BAUD_THRESHOLD(150, 200);
    BAUD_THRESHOLD(200, 300);
    BAUD_THRESHOLD(300, 600);
    BAUD_THRESHOLD(600, 1200);
    BAUD_THRESHOLD(1200, 1800);
    BAUD_THRESHOLD(1800, 2400);
    BAUD_THRESHOLD(2400, 4800);
    BAUD_THRESHOLD(4800, 9600);
    BAUD_THRESHOLD(9600, 19200);
    BAUD_THRESHOLD(19200, 38400);
    BAUD_THRESHOLD(38400, 115200);
    // only possibility is that the value is >115200
    return UART_115200;
}
