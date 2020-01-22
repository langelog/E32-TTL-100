/**
 * LoraE32-On-Nano High Level Controller of E32-TTL-100
 * modules for easier application
 *
 * @author Luis González (langelog@me.com)
 * @date 21 Jan 2020
 * https://github.com/langelog/LoraE32-On-Nano
 *
 */

#define Device_A

#include <Arduino.h>
#include <LoraModule.h>
#include "BuiltInLed.h"

int counter = 1;

BuiltInLed* led;
LoraModule* lora;

#define CHANNEL_MHZ 410 // from 410 MHz to 441 MHz

#ifdef Device_A
struct CFGstruct default_conf = {
        CFG_HEAD_NON_VOLATILE, // if 0xC0: Save data when power-down, if 0xC2: then volatile
        0x08, // from 0x00 to 0xff
        0x21, // from 0x00 to 0xff
        {
            AIR_BPS_2400,
            UART_BPS_9600,
            UART_FORMAT_8N1
        },
        AIR_CHAN_MHZ_(CHANNEL_MHZ),
        {
            TSMT_PWR_10DB_21DB, //
            ENABLE_FEC,
            WAKE_UP_TIME_1000,
            PP_DRIVE_MODE,
            TRSM_FP_MODE,
        }
    };
#else
struct CFGstruct default_conf = {
        CFG_HEAD_NON_VOLATILE, // if 0xC0: Save data when power-down, if 0xC2: then volatile
        0x08, // from 0x00 to 0xff
        0x22, // from 0x00 to 0xff
        {
            AIR_BPS_2400,
            UART_BPS_9600,
            UART_FORMAT_8N1
        },
        AIR_CHAN_MHZ_(CHANNEL_MHZ),
        {
            TSMT_PWR_10DB_21DB, //
            ENABLE_FEC,
            WAKE_UP_TIME_1000,
            PP_DRIVE_MODE,
            TRSM_FP_MODE,
        }
    };
#endif

void setup() {
    Serial.begin(9200);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB
    }

    #ifdef Device_A
        Serial.println("[10-A] ");
    #else
        Serial.println("[10-B] ");
    #endif

    // Create Objects
    led = BuiltInLed::GetInstance(); // initialize the Singleton BuiltInLed
    lora = new LoraModule(SOFT_RX, SOFT_TX, &default_conf);
}

uint8_t dest_h = 0x08;
uint8_t dest_l = 0x21;
uint8_t* msg = (uint8_t *) "Hello";

void loop() {
    uint8_t data_buf[100], data_len;

#ifdef Device_A
    if(lora->ReceiveMsg(data_buf, &data_len) == RET_SUCCESS) {
        led->blinkLED();
        Serial.println("Received something");
    } else {
        Serial.println("Receive Failed");
    }
    delay(200);
#else
    if(lora->SendMsg(dest_h, dest_l,
            AIR_CHAN_MHZ_(CHANNEL_MHZ), msg, 5) == RET_SUCCESS) {
        led->blinkLED();
        Serial.println("Sending hola");
    } else {
        Serial.println("Sending Failed");
    }
    delay(1000);
#endif

}


