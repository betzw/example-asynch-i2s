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

#include "sta350bw/sta350bw.h"

#if DEVICE_I2S

#if !defined(YOTTA_CFG_HARDWARE_TEST_PINS_I2S_DPIN) ||	\
    !defined(YOTTA_CFG_HARDWARE_TEST_PINS_I2S_SCLK) ||	\
    !defined(YOTTA_CFG_HARDWARE_TEST_PINS_I2S_WSEL) ||	\
    !defined(YOTTA_CFG_HARDWARE_TEST_PINS_I2S_FDPX) ||  \
    !defined(YOTTA_CFG_HARDWARE_TEST_PINS_I2S_MCLK) ||  \
	!defined(YOTTA_CFG_HARDWARE_TEST_PINS_I2C_SDA)  ||	\
    !defined(YOTTA_CFG_HARDWARE_TEST_PINS_I2C_SCL)
#error This example requires I2S & I2C test pins to be defined. Please define the hardware.test-pins.i2s.dpin/sclk/wsel/fdpx yotta confing values
#endif

#ifndef NDEBUG
/* betzw: enable debugging while using sleep modes */
#include "x-nucleo-common/DbgMCU.h"
static DbgMCU enable_dbg;
#endif // !NDEBUG

using namespace minar;

#define DMA_BUF_SAMPLE_NUM  (1024*4)
#define NR_CHANNELS         (2)
#define NR_BYTES_PER_SAMPLE (NR_CHANNELS*sizeof(int16_t))

#define DMA_BUFFER_SIZE     (DMA_BUF_SAMPLE_NUM*NR_BYTES_PER_SAMPLE)
#define DMA_BUF_HALF_SIZE   (DMA_BUFFER_SIZE / 2)

static int8_t dma_buffer[DMA_BUFFER_SIZE];

class I2STest {

public:
    I2STest():
		toggle1(PC_0),
		toggle2(PC_1),
		dev_i2c(YOTTA_CFG_HARDWARE_TEST_PINS_I2C_SDA, PullUp, 
			YOTTA_CFG_HARDWARE_TEST_PINS_I2C_SCL, PullUp),
    	sta350(&dev_i2c, PA_10,
    		YOTTA_CFG_HARDWARE_TEST_PINS_I2S_DPIN, YOTTA_CFG_HARDWARE_TEST_PINS_I2S_SCLK,
			YOTTA_CFG_HARDWARE_TEST_PINS_I2S_WSEL, YOTTA_CFG_HARDWARE_TEST_PINS_I2S_FDPX,
			YOTTA_CFG_HARDWARE_TEST_PINS_I2S_MCLK),
		recv_i2s(PC_12, PC_10, PA_4) {
    	// reset debug toggles
    	toggle1 = toggle2 = 0;

    	/* configure sound terminal */
    	if(sta350.Init(92, 48000)) {
    		printf("%s(%d): sta350bw init failed!\r\n", __func__, __LINE__);
    		exit(-1);
    	}

    	/* configure I@S in reception */
    	recv_i2s.audio_frequency(48000);
    	recv_i2s.set_mode(MASTER_RX);
    	recv_i2s.format(16, 32);

    	printf("\r\nTransfer test inited!\r\n");
    }
    
    void start_reception() {
        printf("Starting reception\r\n");

        int res = recv_i2s.transfer()
	       .rx((void*)dma_buffer, sizeof(dma_buffer))
	       .callback(I2S::event_callback_t(this, &I2STest::reception_complete_cb), I2S_EVENT_ALL)
	       .circular(true)
	       .apply();

        if(res != 0) {
        	error("%s, %d: res=%d\r\n", __func__, __LINE__, res);
        }
    }

    void start_transmission() {
        printf("Starting transmission\r\n");

        int res = sta350.dev_i2s.transfer()
	       .tx((void*)dma_buffer, sizeof(dma_buffer))
	       .callback(I2S::event_callback_t(this, &I2STest::transfer_complete_cb), I2S_EVENT_ALL)
	       .circular(true)
	       .apply();

        if(res != 0) {
        	error("%s, %d: res=%d\r\n", __func__, __LINE__, res);
        }
    }

    void start() {
    	start_reception();
    	start_transmission();
    }

private:
    void reception_complete_cb(Buffer tx_buffer, Buffer rx_buffer, int narg) {
        (void)tx_buffer;
        (void)rx_buffer;

        /* error "handling" */
        if(!(narg & (I2S_EVENT_RX_HALF_COMPLETE | I2S_EVENT_RX_COMPLETE))) {
        	error("%s, %d: narg=0x%x\r\n", __func__, __LINE__, (unsigned int)narg);
        }

        toggle2 = !toggle2;
    }

    void transfer_complete_cb(Buffer tx_buffer, Buffer rx_buffer, int narg) {
        (void)tx_buffer;
        (void)rx_buffer;

        /* error "handling" */
        if(!(narg & (I2S_EVENT_TX_HALF_COMPLETE | I2S_EVENT_TX_COMPLETE))) {
        	error("%s, %d: narg=0x%x\r\n", __func__, __LINE__, (unsigned int)narg);
        }

        toggle1 = !toggle1;
    }

private:
    DigitalOut toggle1; // betzw: for debug only
    DigitalOut toggle2; // betzw: for debug only
	DevI2C dev_i2c;
    STA350BW sta350;
    I2S recv_i2s;
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
