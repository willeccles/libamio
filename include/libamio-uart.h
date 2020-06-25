/*!
 * \file libamio-uart.h
 * \author Will Eccles
 * \date 2019-09-05
 * \brief Contains UART functions and data types.
 *
 * \defgroup LIBAMIOUART UART
 * Includes UART functionality
 * \{
 */

#ifndef LIBAMIO_UART_H
#define LIBAMIO_UART_H

#include <stdint.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Handle to a UART device.
typedef int UART_Handle;

/*!
 * \brief UART baud rate values.
 */
typedef enum UART_BAUD {
    UART_0,         //!< 0Hz baud rate (Hang Up, mostly used for modems)
    UART_50,        //!< 50Hz baud rate
    UART_75,        //!< 75Hz baud rate
    UART_110,       //!< 110Hz baud rate
    UART_134,       //!< 134Hz baud rate
    UART_150,       //!< 150Hz baud rate
    UART_200,       //!< 200Hz baud rate
    UART_300,       //!< 300Hz baud rate
    UART_600,       //!< 600Hz baud rate
    UART_1200,      //!< 1200Hz baud rate
    UART_1800,      //!< 1800Hz baud rate
    UART_2400,      //!< 2400Hz baud rate
    UART_4800,      //!< 4800Hz baud rate
    UART_9600,      //!< 9600Hz baud rate
    UART_19200,     //!< 19200Hz baud rate
    UART_38400,     //!< 38400Hz baud rate
    UART_115200,    //!< 115200Hz baud rate
} UART_BAUD;

/*!
 * \brief Opens a UART devices and returns a handle to it.
 * \param dev the device to open, such as "/dev/ttyS0"
 * \param baud the baud rate to use (if outside of #UART_BAUD range, will
 *             be coerced to the nearest neighbor)
 * \return A handle to the UART device. Will exit the program if
 *         an error occurrs.
 */
extern UART_Handle UART_Open(const char* dev, unsigned int baud);

/*!
 * \brief Sends a message over UART.
 * 
 * \param dev the device to send the message with
 * \param msg the message to send
 * \param len number of bytes to send (aka \c strlen of \a msg in many cases)
 * \return If successful, UART_Send() returns the number of bytes transmitted.
 *         If unsuccessful, this function will return -1.
 */
extern ssize_t UART_Send(UART_Handle dev, const char* msg, size_t len);

/*!
 * \brief Reads data over UART.
 *
 * If \a len is zero, this function will immediately return 0.
 * Else, it will read up to \a len bytes into \a buf.
 *
 * \param dev the UART device to use
 * \param buf the buffer to read into
 * \param len the maximum number of bytes to read
 * \return The number of bytes read or -1 if there was an error.
 */
extern ssize_t UART_Read(UART_Handle dev, char* buf, size_t len);

#ifdef __cplusplus
}
#endif

/* \} */
#endif
