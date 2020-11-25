#define _DEFAULT_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <linux/serial.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#include "libamio-serial.h"

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
 * \brief Sets up an interface's attributes for raw serial comms.
 * \param fd the interface's file descriptor
 * \param speed the requested BAUD rate
 * \return 0 if no error, else -1. Sets errno on error.
 */
static int set_interface_attribs_raw(int fd, SERIAL_BAUD speed);

/*!
 * \brief Set up a device for RS485 communications.
 * \param fd the interface's file descriptor
 * \param params params for the device
 * \return 0 on success, -1 otherwise.
 */
static int set_interface_attribs_rs485(int fd, Serial_Params* params);

// convert from an int to the nearest SERIAL_BAUD value and update the params to match
static SERIAL_BAUD get_baud_value(Serial_Params* params);

int Serial_Open(Serial_Device* device, Serial_Params* params) {
    device->_fd = open(params->dev, O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK | O_CLOEXEC); // no CTTY makes sure the tty doesn't start controlling this program
    if (device->_fd < 0) {
        return -1;
    }

    device->_type = params->type;

    if (params->type == SERIAL_UART) {
        if (0 != set_interface_attribs_raw(device->_fd, get_baud_value(params))) {
            return -1;
        }
    } else if (params->type == SERIAL_RS485) {
        if (0 != set_interface_attribs_rs485(device->_fd, params)) {
            return -1;
        }
    } else {
        close(device->_fd);
        return -1;
    }

    return 0;
}

ssize_t Serial_Send(Serial_Device* dev, const char* msg, size_t len) {
    return write(dev->_fd, msg, len);
}

ssize_t Serial_Read(Serial_Device* dev, char* buf, size_t len) {
    if (len == 0) return 0;
    return read(dev->_fd, buf, len);
}

static int set_interface_attribs_raw(int fd, SERIAL_BAUD speed) {
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        return -1;
    }

    cfsetospeed(&tty, BAUDS[speed]);
    cfsetispeed(&tty, BAUDS[speed]);

    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL
            | IXON);

    tty.c_cflag &= ~(PARENB | CSTOPB | CSIZE | CRTSCTS);
    tty.c_cflag |= (CLOCAL | CREAD | CS8);

    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

    tty.c_oflag &= ~OPOST;

    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 5;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        return -1;
    }
    return 0;
}

static int set_interface_attribs_rs485(int fd, Serial_Params* params) {
    struct serial_rs485 rs485conf = {0};

    // enable RS485 mode
    rs485conf.flags |= SER_RS485_ENABLED;

    // default to RTS inactive when sending
    rs485conf.flags &= ~(SER_RS485_RTS_ON_SEND);
    // change if user specified
    if (params->rs485_RTSHighOnSend) {
        rs485conf.flags |= SER_RS485_RTS_ON_SEND;
    }

    // default to RTS inactive after sending
    rs485conf.flags &= ~(SER_RS485_RTS_AFTER_SEND);
    // change if user specified
    if (params->rs485_RTSHighAfterSend) {
        rs485conf.flags |= SER_RS485_RTS_AFTER_SEND;
    }

    // don't receive while sending
    rs485conf.flags &= ~(SER_RS485_RX_DURING_TX);

    if (0 != ioctl(fd, TIOCSRS485, &rs485conf)) {
        return -1;
    }

    return set_interface_attribs_raw(fd, get_baud_value(params));
}

static SERIAL_BAUD get_baud_value(Serial_Params* params) {
    if (params->baud <= BAUD_115200) {
        return params->baud;
    }

#define BAUD_THRESHOLD(p,n) {\
    if (params->baud <= ((n)-(p))/2 && params->baud >= (p)) {params->baud=(p);return BAUD_##p;}\
    if (params->baud > ((n)-(p))/2 && params->baud <= (n)) {params->baud=(n);return BAUD_##n;}}
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
    params->baud = 115200;
    return BAUD_115200;
}
