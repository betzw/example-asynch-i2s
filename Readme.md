# Asynchronous SPI API

The current mbed SPI API is byte oriented, providing the blocking method ``write()`` that waits for a word to be received.

This user guide describes the new array-oriented, non-blocking transfer methods. 

Currently only the following configuration is supported:

- Freescale FRDM-K64F board
- GCC ARM Embedded Toolchain

### Asynch Methods

The SPI API was expanded by three ``transfer()`` methods. These start the non-blocking transfer. Each method uses a different buffer width (8, 16 and 32 bits), defining how data are read/written from/to buffers. 

**Note:** the methods do not reflect the format of transferred data. To change data bits, please use the ``format()`` method.

The ``tx_buffer`` can be a pointer to a TX buffer or NULL. If NULL is passed, the default SPI value is transmitted, to be able to receive data. If ``rx_buffer`` is NULL, received data are ignored.

```
virtual int transfer(uint8_t *tx_buffer, int tx_length, uint8_t *rx_buffer, int rx_length, void (*callback)(int), int event = SPI_EVENT_COMPLETE);

virtual int transfer(uint16_t *tx_buffer, int tx_length, uint16_t *rx_buffer, int rx_length, void (*callback)(int), int event = SPI_EVENT_COMPLETE);

virtual int transfer(uint32_t *tx_buffer, int tx_length, uint32_t *rx_buffer, int rx_length, void (*callback)(int), int event = SPI_EVENT_COMPLETE);

```

### Events

The following SPI events are available for event handling:

- SPI_EVENT_ERROR - Generic SPI error.
- SPI_EVENT_COMPLETE - All data were transferred or received (depending on the transfer parameters).
- SPI_EVENT_RX_OVERFLOW - RX overflow.
- SPI_EVENT_ALL - Logical OR of all available events.

## The Demo Application

This repository is an example for asynchronous SPI. It is a test unit with test cases for asynchronous SPI methods, using a loopback (MOSI connected to MISO).

### Pre-requisites

To build and run this example the following requirements are necessary:

* A computer with the following software installed:
	* [CMake](http://www.cmake.org/download/).
	* [yotta](https://github.com/ARMmbed/yotta). Please note that **yotta has its own set of dependencies**, listed in the [installation instructions](http://armmbed.github.io/yotta/#installing-on-windows).
	* [Python](https://www.python.org/downloads/).
	* [ARM GCC toolchain](https://launchpad.net/gcc-arm-embedded).
	* A serial terminal emulator (e.g. screen, pySerial, cu).
* An FRDM-K64F development board.
* A micro-USB cable.
* A wire to connect MOSI to MISO.
* If your OS is Windows, please follow the installation instructions [for the serial port driver](https://developer.mbed.org/handbook/Windows-serial-configuration).

### Pins

MISO and MOSI pin definitions are target specific - the code and the physical board must both be using the same pins:

1. To know which pins the code is looking for, see the ``spi_asynch.cpp`` file for this sample. The pins are defined as ``TEST_MOSI_PIN`` and ``TEST_MISO_PIN``.

2. To know where on your board those pins are, please look at its [pinout picture](http://developer.mbed.org/platforms/FRDM-K64F/#overview).

### Getting started

1. Connect the FRDM-K64F to the computer with the micro-USB cable, being careful to use the micro-USB port labeled "OpenSDA".

3. Assuming you have cloned this repository or receive it with a release tarball, open a terminal window and navigate to the repository's folder.

	```	
	cd /path/to/mbed-example-asynch-spi
	```

4. Select the yotta target and build to it:

	```
    yotta target frdm-k64f-gcc
    yotta build
    ```

3. Copy ``build/frdm-k64f-gcc/source/mbed-example-asynch-spi.bin`` to your mbed board and wait until the LED next to the USB port stops blinking.

4. Start the serial terminal emulator and connect to the virtual serial port presented by FRDM-K64F. For settings, use 9600 baud, 8N1, no flow control.

5. Press the reset button on the board.

6. The output in the terminal should look like this:

    ```
    TEST(SPI_Master_Asynchronous, queue_test) - 0 ms
    TEST(SPI_Master_Asynchronous, short_tx_long_rx) - 1 ms
    TEST(SPI_Master_Asynchronous, long_tx_short_rx) - 0 ms
    TEST(SPI_Master_Asynchronous, long_tx_long_rx) - 0 ms
    TEST(SPI_Master_Asynchronous, short_tx_short_rx) - 0 ms
    TEST(SPI_Master_Asynchronous, 0_tx_nn_short_rx) - 0 ms
    TEST(SPI_Master_Asynchronous, 0_tx_short_rx) - 0 ms
    TEST(SPI_Master_Asynchronous, short_tx_0_rx_nn) - 0 ms
    TEST(SPI_Master_Asynchronous, short_tx_0_rx) - 0 ms

    OK (9 tests, 9 ran, 36 checks, 0 ignored, 0 filtered out, 51 ms)

    {{success}}
    {{end}}
    ```

7. The LED should be slowly blinking (toggling about 1 Hz).

