# Asynchronous SPI

## The demo application

This repository is an example for asynchronous SPI, a test unit with defined test cases for asynchronous SPI methods, using a loopback (MOSI connected to MISO). It currently supports K64F board.

Please look at the target's pin definition in the spi_asynch.cpp file (TEST_MOSI_PIN and TEST_MISO_PIN) , and connect them together.

The output on the console should be:
```
TEST(SPI_Master_Asynchronous, short_tx_long_rx) - 0 ms
TEST(SPI_Master_Asynchronous, long_tx_short_rx) - 0 ms
TEST(SPI_Master_Asynchronous, long_tx_long_rx) - 0 ms
TEST(SPI_Master_Asynchronous, short_tx_short_rx) - 0 ms
TEST(SPI_Master_Asynchronous, 0_tx_nn_short_rx) - 0 ms
TEST(SPI_Master_Asynchronous, 0_tx_short_rx) - 0 ms
TEST(SPI_Master_Asynchronous, short_tx_0_rx_nn) - 0 ms
TEST(SPI_Master_Asynchronous, short_tx_0_rx) - 1 ms

OK (8 tests, 8 ran, 30 checks, 0 ignored, 0 filtered out, 45 ms)

{{success}}
{{end}}
```

## API overview

The current mbed SPI API is byte oriented, providing blocking method write() which waits for a word to be received.

This user guide describes the non-blocking methods which are array oriented. The write methods initiates the transfer and return to the caller. The completition is reported via callbacks which are registered to specific events.

### Asynch methods

The SPI API was expanded by three methods, write().
The write method starts the non-blocking transfer. There are three write methods, each one uses different buffer width. The buffer width defines how are data read/written from/to buffers. They do not reflect how are data transferred. To change data bits, please use format method.

The tx_buffer can be a pointer to a TX buffer or NULL. If NULL is passed, the default SPI value is transmitted, to be able to receive data. If rx_buffer is NULL, received data are ignored.
```
virtual int write(uint8_t *tx_buffer, int tx_length, uint8_t *rx_buffer, int rx_length, void (*callback)(int), int event = SPI_EVENT_COMPLETE);

virtual int write(uint16_t *tx_buffer, int tx_length, uint16_t *rx_buffer, int rx_length, void (*callback)(int), int event = SPI_EVENT_COMPLETE);

virtual int write(uint32_t *tx_buffer, int tx_length, uint32_t *rx_buffer, int rx_length, void (*callback)(int), int event = SPI_EVENT_COMPLETE);

```

### Events

The following SPI events are available:

- SPI_EVENT_ERROR - Generic SPI error
- SPI_EVENT_COMPLETE - All data were transfered or received, depends on the transfer parameters
- SPI_EVENT_RX_OVERFLOW - The RX overflow
- SPI_EVENT_ALL - Logical OR of all available events
