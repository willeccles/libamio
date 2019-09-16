/*!
 * \file libamio-gpio.h
 * \author Will Eccles
 * \date 2019-09-05
 *
 * \brief Defines GPIO functions.
 *
 * \defgroup LIBAMIOGPIO GPIO
 * Contains functionality for working with GPIO
 * \{
 */

#ifndef LIBAMIO_GPIO_H
#define LIBAMIO_GPIO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief States for GPIO pins.
 */
typedef enum {
    LOW = 0,        //!< Pin state is HIGH
    HIGH = 1,       //!< Pin state is LOW
    ACTIVE = HIGH,  //!< Pin state is HIGH
    INACTIVE = LOW, //!< Pin state is LOW
} GPIO_state;

/*!
 * \brief Direction options for GPIO pins.
 */
typedef enum {
    INPUT,      //!< Pin is an input
    OUTPUT,     //!< Pin is an output
} GPIO_direction;

//! Handle to a GPIO pin.
typedef void* GPIO_Handle;

//! GPIO callback function.
typedef void(*GPIO_CallbackFxn)(void);

//! Macro used to define GPIO pins. For example, to get the PIN
//! number for pin 1_17, you would use %GPIO_PIN(1,17).
#define GPIO_PIN(d,p) ((d)*32+(p))

/*!
 * \brief Initialize a GPIO pin to be input or output.
 *
 * \note At this time, pins can be shared with other processes.
 * 
 * \todo Allow for greedy or weak access to pins?
 * 
 * \param pin the desired pin; use GPIO_PIN macro
 * \param direction whether or not the pin is an input or an output
 *
 * \return A handle to the GPIO pin, or NULL.
 */
extern GPIO_Handle GPIO_initpin(unsigned int pin, GPIO_direction direction);

/*!
 * \brief Cleans up and closes a GPIO pin.
 *
 * \param pin a handle to the GPIO pin to release
 */
extern void GPIO_release(GPIO_Handle pin);

/*!
 * \brief Write a value to a GPIO pin.
 * \param handle the GPIO pin to use
 * \param val the value for the pin; see #GPIO_state
 * \return EXIT_SUCCESS or EXIT_FAILURE.
 */
extern int GPIO_write(GPIO_Handle handle, GPIO_state val);

/*!
 * \brief Read a value from a GPIO pin.
 * \param handle a handle to the GPIO pin to use
 * \return The state of the GPIO pin or -1 on error.
 */
extern GPIO_state GPIO_read(GPIO_Handle handle);

/*!
 * \brief Enables interrupt handling for a pin.
 *
 * This should be followed by a call to GPIO_setCallback() for the pin.
 * Interrupts are generated on both rising and falling edges.
 *
 * \param handle a handle to the GPIO pin to use
 * \return EXIT_SUCCESS or EXIT_FAILURE.
 */
extern int GPIO_enableInt(GPIO_Handle handle);

/*!
 * \brief Disables interrupt handling for a pin.
 *
 * \param handle a handle to the GPIO pin to use
 * \return EXIT_SUCCESS or EXIT_FAILURE.
 */
extern int GPIO_disableInt(GPIO_Handle handle);

/*!
 * \brief Set a callback for a pin with interrupts enabled.
 * \param handle the GPIO pin to use
 * \param fxn the GPIO callback function to associate with the pin
 * \return EXIT_SUCCESS or EXIT_FAILURE.
 */
extern int GPIO_setCallback(GPIO_Handle handle, GPIO_CallbackFxn fxn);

#ifdef __cplusplus
}
#endif

/*! \} */
#endif
