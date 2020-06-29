/*!
 * \file libamio-serial.h
 * \author Will Eccles
 * \date 2019-09-05
 * \brief Contains serial functions and data types.
 *
 * \defgroup LIBAMIOSERIAL Serial
 * Includes serial functionality
 * \{
 */

#ifndef LIBAMIO_SERIAL_H
#define LIBAMIO_SERIAL_H

#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Serial baud rate values.
 */
typedef enum SERIAL_BAUD {
    BAUD_0,         //!< 0Hz baud rate (Hang Up, mostly used for modems)
    BAUD_50,        //!< 50Hz baud rate
    BAUD_75,        //!< 75Hz baud rate
    BAUD_110,       //!< 110Hz baud rate
    BAUD_134,       //!< 134Hz baud rate
    BAUD_150,       //!< 150Hz baud rate
    BAUD_200,       //!< 200Hz baud rate
    BAUD_300,       //!< 300Hz baud rate
    BAUD_600,       //!< 600Hz baud rate
    BAUD_1200,      //!< 1200Hz baud rate
    BAUD_1800,      //!< 1800Hz baud rate
    BAUD_2400,      //!< 2400Hz baud rate
    BAUD_4800,      //!< 4800Hz baud rate
    BAUD_9600,      //!< 9600Hz baud rate
    BAUD_19200,     //!< 19200Hz baud rate
    BAUD_38400,     //!< 38400Hz baud rate
    BAUD_115200,    //!< 115200Hz baud rate
} SERIAL_BAUD;

/*!
 * \brief Defines the types of serial devices supported by this library.
 */
typedef enum SERIAL_TYPE {
    SERIAL_RAW,                 //!< Raw serial device
    SERIAL_UART = SERIAL_RAW,   //!< Serial UART device
    SERIAL_RS485,               //!< RS485 device
} SERIAL_TYPE;

/*!
 * \brief Structure used to configure a serial device.
 */
typedef struct Serial_Params {
    const char* dev;                //!< Device (e.g. "/dev/ttyS0")
    SERIAL_TYPE type;               //!< Type of serial device
    unsigned int baud;              //!< Baud rate (will be coerced to the nearest SERIAL_BAUD value)
    bool rs485_RTSHighOnSend;       //!< [RS485] Should the RTS pin be active when sending?
    bool rs485_RTSHighAfterSend;    //!< [RS485] Should the RTS pin be active after sending?
} Serial_Params;

//! Structure representing a serial device. You should not change its contents!
typedef struct Serial_Device {
    int _fd;
    SERIAL_TYPE _type;
} Serial_Device;

/*!
 * \brief Opens a serial device with the specified parameters.
 * \param device a pointer to a serial device structure to store the data in
 *               (its contents should not be modified by the user)
 * \param params a pointer to a Serial_Params structure with which to configure the device
 * \note When the baud is coerced to a SERIAL_BAUD value, the \a params baud value will be
 *       updated to the actual value in case you need to do calculations with it.
 * \return 0 on success, otherwise error.
 */
extern int Serial_Open(Serial_Device* device, Serial_Params* params);

/*!
 * \brief Sends a message over serial.
 * 
 * \param dev a pointer to the device to send the message with
 * \param msg the message to send
 * \param len number of bytes to send (aka \c strlen of \a msg in many cases)
 * \return If successful, Serial_Send() returns the number of bytes transmitted.
 *         If unsuccessful, this function will return -1 and set errno.
 */
extern ssize_t Serial_Send(Serial_Device* dev, const char* msg, size_t len);

/*!
 * \brief Reads data over serial.
 *
 * If \a len is zero, this function will immediately return 0.
 * Else, it will read up to \a len bytes into \a buf.
 * 
 * \note Devices are opened in non-blocking mode. Thus, read() will not block.
 *       If this function returns -1 and errno is either EWOULDBLOCK or EAGAIN,
 *       no data was ready to be read.
 *       If you wish for read() to block, you can remove the non-blocking flag
 *       from the device by using the appropriate ioctl() and the \a _fd member
 *       of the device structure.
 *
 * \param dev the serial device to use
 * \param buf the buffer to read into
 * \param len the maximum number of bytes to read
 * \return The number of bytes read or -1 if there was an error.
 */
extern ssize_t Serial_Read(Serial_Device* dev, char* buf, size_t len);

#ifdef __cplusplus
}
#endif

/* \} */
#endif
