/**
 * E32-TTL-100 Transceiver Interface
 *
 * @author Bob Chen (bob-0505@gotmail.com)
 * @date 1 November 2017
 * https://github.com/Bob0505/E32-TTL-100
 *
 * modified by: Luis González (langelog@me.com)
 */

#ifndef LORAMODULE_LORAMODULE_H
#define LORAMODULE_LORAMODULE_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "E32-TTL-100.h"
#include "BuiltInLed.h"

/*
 need series a 4.7k Ohm resistor between .
 UNO/NANO(5V mode)                E32-TTL-100
    *--------*                      *------*
    | D7     | <------------------> | M0   |
    | D8     | <------------------> | M1   |
    | A0     | <------------------> | AUX  |
    | D10(Rx)| <---> 4.7k Ohm <---> | Tx   |
    | D11(Tx)| <---> 4.7k Ohm <---> | Rx   |
    *--------*                      *------*
*/

#define M0_PIN          7
#define M1_PIN          8
#define AUX_PIN         A0
#define SOFT_RX         10
#define SOFT_TX         11
#define MEASURE_PIN1    A2

class LoraModule {
    bool AUX_HL;

public:
    LoraModule(int rx_pin = SOFT_RX, int tx_pin = SOFT_TX, struct CFGstruct* CFG = nullptr);
    ~LoraModule();

    // AUX
    bool ReadAUX();
    RET_STATUS WaitAUX_H();

    // Mode Select
    bool chkModeSame(MODE_TYPE mode);
    void SwitchMode(MODE_TYPE mode);

    // Basic CMD
    void cleanUARTBuf();
    void triple_cmd(SLEEP_MODE_CMD_TYPE Tcmd);
    RET_STATUS Module_info(uint8_t* pReadbuf, uint8_t buf_len);

    // Sleep mode CMD
    RET_STATUS Write_CFG_PDS(struct CFGstruct* pCFG);
    RET_STATUS Read_CFG(struct CFGstruct* pCFG);
    RET_STATUS Read_module_version(struct MVerstruct* Mver);
    void Reset_Module();
    RET_STATUS SleepModeCmd(uint8_t CMD, void* pBuff);

    // Sleep mode CMD
    RET_STATUS SettingModule(struct CFGstruct* pCFG);
    RET_STATUS ReceiveMsg(uint8_t* pdatabuf, uint8_t* data_len);

    RET_STATUS SendMsg(uint8_t& dest_addr_h, uint8_t& dest_addr_l,
            uint8_t channel, void* data, uint8_t data_len);

private:
    SoftwareSerial* softSerial;
    BuiltInLed*     led;

    struct CFGstruct default_conf = {
            CFG_HEAD_NON_VOLATILE, // if 0xC0: Save data when power-down, if 0xC2: then volatile
            SAMPLE_DEVICE_ADDR_H, // from 0x00 to 0xff
            SAMPLE_DEVICE_ADDR_L, // from 0x00 to 0xff
            {
                AIR_BPS_2400,
                UART_BPS_9600,
                UART_FORMAT_8N1
            },
            AIR_CHAN_MHZ_(433),
                {
                TSMT_PWR_10DB_21DB, //
                ENABLE_FEC,
                WAKE_UP_TIME_1000,
                PP_DRIVE_MODE,
                TRSM_FP_MODE,
            }
        };
};


#endif //LORAMODULE_LORAMODULE_H
