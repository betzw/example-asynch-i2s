# Asynchronous SPI Example

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

MISO, MOSI and SSEL pin definitions are target-specific. To function correctly, the code and the physical board must both be using the same pins.  Follow these steps to determine which pins the code is configured to use.

1. First, find the pin references in the source files.  In this example, view ``spi_asynch.cpp``. The pin references are ``YOTTA_CFG_HARDWARE_TEST_PINS_SPI_MISO``, ``YOTTA_CFG_HARDWARE_TEST_PINS_SPI_MOSI`` and ``YOTTA_CFG_HARDWARE_TEST_PINS_SPI_SSEL``.

2. Next, find the target-specific pin definitions corresponding to the pin references in the target's ``target.json`` file. The file is located at ``yotta_targets/frdm-k64f-gcc/target.json``.

    Each keyword in the pin reference represents a level in the target configuration structure.  To find what YOTTA_CFG_HARDWARE_TEST_PINS_SPI resolves to, look under config/hardware/test-pins/spi.

    The spi pins for K64F are:

    ```
    "spi": {
      "mosi": "PTD2",
      "miso": "PTD3",
      "sclk": "PTD1",
      "ssel": "PTD0"
    }
    ```

3. Finally, locate those pins on the board's [pinout picture](http://developer.mbed.org/platforms/FRDM-K64F/#overview).

### Getting started

1. Connect the FRDM-K64F to the computer with the micro-USB cable, being careful to use the micro-USB port labeled "OpenSDA".

3. Assuming you have cloned this repository or receive it with a release tarball, open a terminal window and navigate to the repository's folder.

    ```
    cd /path/to/example-asynch-spi
    ```

4. Select the yotta target and build to it:

    ```
    yotta target frdm-k64f-gcc
    yotta build
    ```

3. Copy ``build/frdm-k64f-gcc/source/example-asynch-spi.bin`` to your mbed board and wait until the LED next to the USB port stops blinking.

4. Start the serial terminal emulator and connect to the virtual serial port presented by FRDM-K64F. For settings, use 115200 baud, 8N1, no flow control.

5. Press the reset button on the board.

6. The output in the terminal should look like this:

    ```
    Starting short transfer test
    Res is 0
    Short transfer DONE, event is 4
    Starting long transfer test
    Res is 0
    Long transfer DONE, event is 4
    **** Test done ****
    ```

