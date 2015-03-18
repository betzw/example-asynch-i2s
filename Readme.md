# Asynchronous SPI

## The demo application

This repository is an example for asynchronous SPI, a test unit with defined test cases for asynchronous SPI methods, using a loopback (MOSI connected to MISO).

Please look at the target's pin definition in the spi_asynch.cpp file (TEST_MOSI_PIN and TEST_MISO_PIN) , and connect them together.

Currently only the following configuration is supported:
- Freescale FRDM-K64F board
- GCC ARM Embedded Toolchain

### Installation
This is a yotta executable. You need the latest [yotta](https://github.com/ARMmbed/yotta) release version (latest tested: 0.0.36) and all of its dependencies. See also the [yotta documentation](http://armmbed.github.io/yotta/) for further details about the installation and the build process.

Assuming you have cloned this repository, move to its folder. Select target, install and build:
```bash
cd /path/to/mbed-example-asynch-spi
yotta target frdm-k64f-gcc
yotta install
yotta build
```

The resulting binary file will be located in `build/frdm-k64f-gcc/source/`. After flashing, you can observe the UART output on the COM port.


The output on the console should be:
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

## API overview

The current mbed SPI API is byte oriented, providing blocking method write() which waits for a word to be received.

This user guide describes the non-blocking methods which are array oriented. The transfer methods initiates the transfer and return to the caller. The completition is reported via callbacks which are registered to specific events.

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
