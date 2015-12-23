/* mbed Microcontroller Library
 * Copyright (c) 2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed-drivers/mbed.h"
#include "mbed-drivers/I2S.h"
#include <stdio.h>
#include "minar/minar.h"
#include "core-util/Event.h"

#if DEVICE_I2S

#define XFER_SIZE  (2 * 8192)
#define TEST_BYTE0 0x00
#define TEST_BYTE1 0x11
#define TEST_BYTE2 0xFF
#define TEST_BYTE3 0xAA
#define TEST_BYTE4 0x55
#define TEST_BYTE5 0x50

#define TEST_BYTE_RX TEST_BYTE3
#define TEST_BYTE_TX_BASE TEST_BYTE5

#if !defined(YOTTA_CFG_HARDWARE_TEST_PINS_I2S_DPIN) ||	\
    !defined(YOTTA_CFG_HARDWARE_TEST_PINS_I2S_SCLK) ||	\
    !defined(YOTTA_CFG_HARDWARE_TEST_PINS_I2S_WSEL) ||	\
    !defined(YOTTA_CFG_HARDWARE_TEST_PINS_I2S_FDPX)

#error This example requires I2S test pins to be defined. Please define the hardware.test-pins.i2s.dpin/sclk/wsel/fdpx yotta confing values
#endif

#ifndef NDEBUG
/* betzw: enable debugging while using sleep modes */
#include "x-nucleo-common/DbgMCU.h"
static DbgMCU enable_dbg;
#endif // !NDEBUG

using namespace minar;

class I2STest {

public:
    I2STest(): i2s(YOTTA_CFG_HARDWARE_TEST_PINS_I2S_DPIN, YOTTA_CFG_HARDWARE_TEST_PINS_I2S_SCLK,
		   YOTTA_CFG_HARDWARE_TEST_PINS_I2S_WSEL, YOTTA_CFG_HARDWARE_TEST_PINS_I2S_FDPX) {
	for (uint32_t i = 0; i < sizeof(tx_buf); i++) {
	    tx_buf[i] = i + TEST_BYTE_TX_BASE;
	}
    }
    
    void start() {
        printf("Starting transfer test\r\n");

        printf("Res is %d\r\n", i2s.transfer()
	       .tx(tx_buf, XFER_SIZE)
	       .callback(I2S::event_callback_t(this, &I2STest::transfer_complete_cb), I2S_EVENT_ALL)
	       .circular(true)
	       .apply());
    }

private:
    void init_rx_buffer() {
        for (uint32_t i = 0; i < sizeof(rx_buf); i ++) {
            rx_buf[i] = 0;
        }
    }

    void compare_buffers(uint32_t len) {
	for (uint32_t i = 0; i < len; i ++) {
            if (tx_buf[i] != rx_buf[i]) {
                printf("MISMATCH at position %lu: expected %d, got %d\r\n", i, (int)tx_buf[i], (int)rx_buf[i]);
            }
        }
    }

    void transfer_complete_cb(Buffer tx_buffer, Buffer rx_buffer, int narg) {
        (void)tx_buffer;
        (void)rx_buffer;

        printf("Transfer: event is %d\r\n", narg);
    }

private:
	I2S i2s;
	uint8_t tx_buf[XFER_SIZE] __attribute((aligned (32)));
	uint8_t rx_buf[XFER_SIZE] __attribute((aligned (32)));
};

void app_start(int, char*[]) {
    static I2STest test;
    Scheduler::postCallback(mbed::util::FunctionPointer0<void>(&test, &I2STest::start).bind());
}

#else
void app_start(int, char*[]) {
    printf("The target does not support I2S (asynch) API.\r\n");
}
#endif
