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

#include "my_song.inc"

#define DMA_BUF_SAMPLE_NUM  (512)
#define NR_CHANNELS         (2)
#define NR_BYTES_PER_SAMPLE (NR_CHANNELS*sizeof(int16_t))

#define DMA_BUFFER_SIZE     (DMA_BUF_SAMPLE_NUM*NR_BYTES_PER_SAMPLE)
#define DMA_BUF_HALF_SIZE   (DMA_BUFFER_SIZE / 2)

static int8_t dma_buffer[DMA_BUFFER_SIZE];
static unsigned int song_pos = 0;

class I2STest {

public:
    I2STest():
		toggle1(PC_0),
		toggle2(PC_1),
    	dev_i2c(YOTTA_CFG_HARDWARE_TEST_PINS_I2C_SDA, YOTTA_CFG_HARDWARE_TEST_PINS_I2C_SCL),
    	sta350(&dev_i2c, PA_10,
    		YOTTA_CFG_HARDWARE_TEST_PINS_I2S_DPIN, YOTTA_CFG_HARDWARE_TEST_PINS_I2S_SCLK,
			YOTTA_CFG_HARDWARE_TEST_PINS_I2S_WSEL, YOTTA_CFG_HARDWARE_TEST_PINS_I2S_FDPX,
			YOTTA_CFG_HARDWARE_TEST_PINS_I2S_MCLK) {
    	// reset debug toggles
    	toggle1 = toggle2 = 0;

    	if(sta350.Init(73, 32000)) {
    		printf("%s(%d): sta350bw init failed!\r\n", __func__, __LINE__);
    		exit(-1);
    	}

    	memcpy(&dma_buffer[0], &my_song[song_pos], DMA_BUFFER_SIZE);
    	song_pos = DMA_BUFFER_SIZE/sizeof(int16_t);

    	printf("\r\nTransfer test inited!\r\n");
    }
    
    void changeVolume(void) {
    	printf("Changing volume now!\r\n");

    	sta350.SetVolume(0x0, 113);
    }

    void start() {
        printf("Starting transfer test\r\n");

        printf("Res is %d\r\n", sta350.dev_i2s.transfer()
	       .tx((void*)dma_buffer, sizeof(dma_buffer))
	       .callback(I2S::event_callback_t(this, &I2STest::transfer_complete_cb), I2S_EVENT_ALL)
	       .circular(true)
	       .apply());

        Scheduler::postCallback(mbed::util::FunctionPointer0<void>(this, &I2STest::changeVolume).bind()).delay(milliseconds(3500));
    }

private:
    void transfer_complete_cb(Buffer tx_buffer, Buffer rx_buffer, int narg) {
        (void)tx_buffer;
        (void)rx_buffer;

        unsigned int dma_pos;

        toggle1 = 1;

        if(((song_pos * sizeof(int16_t)) + DMA_BUF_HALF_SIZE) > sizeof(my_song)) {
        	song_pos = 0;
        }

        if(narg & I2S_EVENT_TX_COMPLETE) dma_pos = DMA_BUF_HALF_SIZE;

#if 0
    	printf("---\r\n");
    	printf("dma_pos = %u\r\n", dma_pos);
    	printf("song_pos = %u\r\n", song_pos);
#endif

    	memcpy(&dma_buffer[dma_pos], &my_song[song_pos], DMA_BUF_HALF_SIZE);
    	song_pos += (DMA_BUF_HALF_SIZE/sizeof(int16_t));

    	toggle1 = 0;
    }

private:
    DigitalOut toggle1; // betzw: for debug only
    DigitalOut toggle2; // betzw: for debug only
	DevI2C dev_i2c;
    STA350BW sta350;
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
