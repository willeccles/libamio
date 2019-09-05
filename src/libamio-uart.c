#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

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

UART_Handle UART_Open(const char* dev, UART_BAUD baud) {
    // \todo look into O_NONBLOCK for this and maybe others?
    int fd = open(dev, O_RDWR | O_NOCTTY | O_SYNC); // no CTTY makes sure the tty doesn't start controlling this program
    if (fd < 0) {
        errExit();
    }

    if (0 != set_interface_attribs(fd, baud)) {
        errExit();
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

    cfsetospeed(&tty, (speed_t)BAUDS[speed]);
    cfsetispeed(&tty, (speed_t)BAUDS[speed]);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;                 /* 8-bit characters */
    tty.c_cflag &= ~PARENB;             /* no parity bit */
    tty.c_cflag &= ~CSTOPB;             /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;            /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 0; // see https://stackoverflow.com/a/6947758/2712525
    tty.c_cc[VTIME] = 5;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        return -1;
    }
    return 0;
}
