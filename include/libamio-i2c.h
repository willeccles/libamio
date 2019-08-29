/*!
 * \file libhwio-i2c.h
 * \author Will Eccles
 * \date 2019-08-28
 * \brief Defines various I2C-related types, structures, and functions.
 *
 * \defgroup HWIOI2C I2C
 * Contains I2C functionality.
 * \todo Add support for slave mode.
 * \{
 */
#ifndef HWIO_I2C_H
#define HWIO_I2C_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Contains information related to an I2C transaction.
 */
typedef struct I2C_Transaction {
    int         timeout;        //!< The timeout for this transaction (milliseconds)
                                //!< \note Try to keep this to multiples of 10!
    uint8_t     slaveAddress;   //!< The address for the slave used in this transaction
                                //!< \note This doesn't need to be shifted - simply
                                //!<       put in the 7-bit address (i.e. 0x40) as the
                                //!<       documentation lists it.
    uint16_t    writeCount;     //!< The number of bytes to write
    uint16_t    readCount;      //!< The number of bytes to read
    uint8_t*    writeBuf;       //!< The buffer to write data from
    uint8_t*    readBuf;        //!< The buffer to read data into
} I2C_Transaction;

//! A handle to an I2C device.
typedef void* I2C_Handle;

/*!
 * \brief Open an I2C device.
 * \param devnum the device number of the I2C device to open
 * \return A handle to the I2C device opened or NULL on error.
 */
extern I2C_Handle I2C_open(uint8_t devnum);

/*!
 * \brief Transfer data over I2C.
 * \param handle a handle to the I2C device to use for the transfer
 * \param transaction the transaction to transfer over I2C
 * \return EXIT_SUCCESS if successful, else EXIT_FAILURE.
 */
extern int I2C_transfer(I2C_Handle handle, I2C_Transaction* transaction);

/*!
 * \brief Closes an cleans up a given I2C device.
 * \param handle a handle to the device to close
 */
extern void I2C_close(I2C_Handle handle);

#ifdef __cplusplus
} // extern "C"
#endif

/*! \} */
#endif
