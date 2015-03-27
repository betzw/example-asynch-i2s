# Asynchronous SPI

## The demo application

This repository is an example for asynchronous SPI, a test unit with defined test cases for asynchronous SPI methods, using a loopback (MOSI connected to MISO).

Please look at the target's pin definition in the spi_asynch.cpp file (TEST_MOSI_PIN and TEST_MISO_PIN) , and connect them together. The pin definitions are target specific, look at the [mbed platform page](https://developer.mbed.org/platforms), where each platform has pinout picture.

Currently only the following configuration is supported:
- Freescale FRDM-K64F board
- GCC ARM Embedded Toolchain

### Pre-requisites

To build and run this example the requirements below are necessary:
* A computer with the following software installed
  * CMake
  * yotta
  * python
  * ARM-GCC toolchain
  * a serial terminal emulator (e.g. screen, pyserial, cu)
* A frdm-k64f development board
* A micro-USB cable
* A wire to connect MOSI to MISO

### Getting started
This is a yotta executable. You need the latest [yotta](https://github.com/ARMmbed/yotta) release version (latest tested: 0.2.2) and all of its dependencies. See also the [yotta documentation](http://armmbed.github.io/yotta/) for further details about the installation and the build process.

1. Connect the frdm-k64f to the computer with the micro-USB cable, being careful to use the micro-usb port labeled "OpenSDA"

2. Assuming you have cloned this repository, move to its folder. Select target, update, install and build:
    ```
    cd /path/to/mbed-example-asynch-spi
    yotta target frdm-k64f-gcc
    yotta build
    ```

3. Copy 'build/frdm-k64f-gcc/source/mbed-example-asynch-spi.bin` to your mbed board and wait until the LED next to the USB port stops blinking.

4. Start the serial terminal emulator and connect to the virtual serial port presented by frdm-k64f. For settings, use 9600 baud, 8N1, no flow control.

5. Press the reset button on the board.

6. The output in the terminal should look like:

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

7. LED should be slowly blinking (toggling about 1 Hz).

## API overview

The current mbed SPI API is byte oriented, providing blocking method write() which waits for a word to be received.

This user guide describes the non-blocking methods which are array oriented. The transfer methods initiates the transfer and return to the caller. The completion is reported via callbacks which are registered to specific events.

### Asynch methods

The SPI API was expanded by three methods, transfer().
The transfer method starts the non-blocking transfer. There are three transfer methods, each one uses different buffer width. The buffer width defines how are data read/written from/to buffers. They do not reflect the format of transferred data. To change data bits, please use format() method.

The tx_buffer can be a pointer to a TX buffer or NULL. If NULL is passed, the default SPI value is transmitted, to be able to receive data. If rx_buffer is NULL, received data are ignored.
```
virtual int transfer(uint8_t *tx_buffer, int tx_length, uint8_t *rx_buffer, int rx_length, void (*callback)(int), int event = SPI_EVENT_COMPLETE);

virtual int transfer(uint16_t *tx_buffer, int tx_length, uint16_t *rx_buffer, int rx_length, void (*callback)(int), int event = SPI_EVENT_COMPLETE);

virtual int transfer(uint32_t *tx_buffer, int tx_length, uint32_t *rx_buffer, int rx_length, void (*callback)(int), int event = SPI_EVENT_COMPLETE);

```

### Events

The following SPI events are available:

- SPI_EVENT_ERROR - Generic SPI error
- SPI_EVENT_COMPLETE - All data were transfered or received, depends on the transfer parameters
- SPI_EVENT_RX_OVERFLOW - The RX overflow
- SPI_EVENT_ALL - Logical OR of all available events
