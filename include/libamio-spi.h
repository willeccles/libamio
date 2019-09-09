/*!
 * \file libhwio-spi.h
 * \author Will Eccles
 * \date 2019-08-28
 * \brief Defines SPI-related structures and data types.
 *
 * \defgroup LIBHWIOSPI SPI
 * SPI-related structures, data-types, and functions.
 * \{
 */

#ifndef LIBHWIO_SPI_H
#define LIBHWIO_SPI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Handle to a SPI device.
typedef void* SPI_Handle;

/*!
 * \brief SPI modes.
 */
typedef enum SPI_MODE {
    SPI_POL0_PHA0 = 0, //!< SPI mode 0 (polarity 0; phase 0)
    SPI_POL0_PHA1 = 1, //!< SPI mode 1 (polarity 0; phase 1)
    SPI_POL1_PHA0 = 2, //!< SPI mode 2 (polarity 1; phase 0)
    SPI_POL1_PHA1 = 3, //!< SPI mode 3 (polarity 1; phase 1)
} SPI_MODE;

/*!
 * \brief Wrapper SPI transaction struct.
 * \note Both #txBuf and #rxBuf need to be non-NULL and should have the same
 * length, which should be less than or equal to #count.
 */
typedef struct SPI_Transaction {
    int      count; //!< Number of bytes to transmit+receive
    uint8_t *rxBuf; //!< Buffer into which data should be written (NULL if you
                    //!< wish to only send but not receive)
    uint8_t *txBuf; //!< Buffer from which to transmit data
} SPI_Transaction;

/*!
 * \brief Wrapper SPI params struct.
 */
typedef struct SPI_Params {
    uint32_t    bitrate;    //!< Bitrate in Hz
    uint8_t     wordsize;   //!< Word size in bits (use 8)
    SPI_MODE    mode;       //!< Frame format (aka SPI mode)
} SPI_Params;

/*!
 * \brief Open a SPI device for use.
 * \param bus the SPI bus' device major number, i.e. N in /dev/spidevN.M
 * \param cs the SPI bus' device minor number, i.e. M in /dev/spidevN.M
 * \param params a pointer to the parameters for the SPI device
 * \return Handle to the SPI device opened or NULL on error
 */
extern SPI_Handle SPI_open(int bus, int cs, SPI_Params* params);

/*!
 * \brief Transfer data over a SPI bus.
 * \param handle a handle to the SPI device to transmit data with
 * \param transaction a pointer to the SPI transaction to make
 * \return EXIT_SUCCESS if successful, else EXIT_FAILURE. If the transaction's
 *                      \c count value is 0, this will return EXIT_SUCCESS,
 *                      because technically
 *                      there was no error.
 */
extern int SPI_transfer(SPI_Handle handle, SPI_Transaction* transaction);

/*!
 * \brief Closes a SPI device.
 * \param handle a handle to the SPI device to close
 */
extern void SPI_close(SPI_Handle handle);

#ifdef __cplusplus
} // extern "C"
#endif

/*! \} */
#endif // LIBHWIO_SPI_H
