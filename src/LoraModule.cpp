/**
 * E32-TTL-100 Transceiver Interface
 *
 * @author Bob Chen (bob-0505@gotmail.com)
 * @date 1 November 2017
 * https://github.com/Bob0505/E32-TTL-100
 *
 * modified by: Luis González (langelog@me.com)
 */

#include <LoraModule.h>
#include <SoftwareSerial.h>

LoraModule::LoraModule(int rx_pin, int tx_pin, struct CFGstruct* CFG) {
    led = BuiltInLed::GetInstance();

    pinMode(M0_PIN, OUTPUT);
    pinMode(M1_PIN, OUTPUT);
    pinMode(AUX_PIN, INPUT);
    softSerial = new SoftwareSerial(rx_pin, tx_pin);
    softSerial->begin(9600);

    RET_STATUS STATUS = RET_SUCCESS;

    struct CFGstruct  tempCFG;
    struct MVerstruct Mver;

    // move to sleep mode, and read current configurations
    STATUS = SleepModeCmd(R_CFG, (void*)&tempCFG);
    // Write configuration into EEPROM, restart and read config
    if(CFG == nullptr)
        STATUS = SettingModule(&default_conf);
    else
        STATUS = SettingModule(CFG);

    // Read module version:
    STATUS = SleepModeCmd(R_MODULE_VERSION, (void*)&Mver);

    // Modde 0 | normal operation
    SwitchMode(MODE_0_NORMAL);

    // self-check initialization
    WaitAUX_H();
    delay(10);

    led->blinkLED();
    delay(1000);
    if(STATUS == RET_SUCCESS) {
        Serial.println("Setup init OK!!");
    } else {
        Serial.println("Setup init FAILED!!");
        while(true) {
            led->blinkLED();
            delay(100);
        }
    }
    Serial.println("BEGIN");

}

LoraModule::~LoraModule() {
    delete softSerial;
}

bool LoraModule::ReadAUX() {
    int val = analogRead(AUX_PIN);

    if(val < 50) {
        AUX_HL = LOW;
    } else {
        AUX_HL = HIGH;
    }

    return AUX_HL;
}

RET_STATUS LoraModule::WaitAUX_H() {
    RET_STATUS STATUS = RET_SUCCESS;

    uint8_t cnt = 0;
    uint8_t data_buf[100], data_len;

    while((ReadAUX()==LOW) && (cnt++<TIME_OUT_CNT)) {
        Serial.print(".");
        delay(100);
    }

    if(cnt==0) {
    } else if(cnt>=TIME_OUT_CNT) {
        STATUS = RET_TIMEOUT;
        Serial.println(" TimeOut");
    } else {
        Serial.println("");
    }

    return STATUS;
}

bool LoraModule::chkModeSame(MODE_TYPE mode) {
    static MODE_TYPE pre_mode = MODE_INIT;

    if(pre_mode == mode) {
        return true;
    }
    Serial.print("SwitchMode: from "); Serial.print(pre_mode, HEX); Serial.print(" to "); Serial.println(mode, HEX);
    pre_mode = mode;
    return false;
}

void LoraModule::SwitchMode(MODE_TYPE mode) {
    if(!chkModeSame(mode)) {
        WaitAUX_H();

        switch(mode) {
            case MODE_0_NORMAL:
                // Mode 0 | normal operation
                digitalWrite(M0_PIN, LOW);
                digitalWrite(M1_PIN, LOW);
                break;
            case MODE_1_WAKE_UP:
                digitalWrite(M0_PIN, HIGH);
                digitalWrite(M1_PIN, LOW);
                break;
            case MODE_2_POWER_SAVIN:
                digitalWrite(M0_PIN, LOW);
                digitalWrite(M1_PIN, HIGH);
                break;
            case MODE_3_SLEEP:
                digitalWrite(M0_PIN, HIGH);
                digitalWrite(M1_PIN, HIGH);
                break;
            default:
                break;
        }

        WaitAUX_H();
        delay(10);
    }
}

void LoraModule::cleanUARTBuf() {
    bool IsNull = true;

    while (softSerial->available()) {
        IsNull = false;
        softSerial->read();
    }
}

void LoraModule::triple_cmd(SLEEP_MODE_CMD_TYPE Tcmd) {
    uint8_t CMD[3] = {static_cast<uint8_t>(Tcmd), static_cast<uint8_t>(Tcmd), static_cast<uint8_t>(Tcmd)};
    softSerial->write(CMD, 3);
    delay(50);
}

RET_STATUS LoraModule::Module_info(uint8_t *pReadbuf, uint8_t buf_len) {
    RET_STATUS STATUS = RET_SUCCESS;
    uint8_t Readcnt, idx;

    Readcnt = softSerial->available();
    if(Readcnt == buf_len) {
        for(idx=0; idx<buf_len; idx++) {
            *(pReadbuf+idx) = softSerial->read();
            Serial.print(" 0x");
            Serial.print(0xFF & *(pReadbuf+idx), HEX);
        }
        Serial.println("");
    } else {
        STATUS = RET_DATA_SIZE_NOT_MATCH;
        Serial.print(" RET_DATA_SIZE_NOT_MATCH - Readcnt: "); Serial.println(Readcnt);
        cleanUARTBuf();
    }

    return STATUS;
}

RET_STATUS LoraModule::Write_CFG_PDS(struct CFGstruct *pCFG) {
    softSerial->write((uint8_t*)pCFG, 6);

    WaitAUX_H();
    delay(1200);

    return RET_SUCCESS;
}

RET_STATUS LoraModule::Read_CFG(struct CFGstruct *pCFG) {
    RET_STATUS STATUS = RET_SUCCESS;

    //1. Read UART buffer.
    cleanUARTBuf();

    //2. send CMD
    triple_cmd(R_CFG);

    //3. Receive configure
    STATUS = Module_info((uint8_t*)pCFG, sizeof(CFGstruct));
    if(STATUS == RET_SUCCESS) {
        Serial.print("  HEAD:     "); Serial.println(pCFG->HEAD, HEX);
        Serial.print("  ADDH:     "); Serial.println(pCFG->ADDH, HEX);
        Serial.print("  ADDL:     "); Serial.println(pCFG->ADDL, HEX);

        Serial.print("  CHAN:     "); Serial.println(pCFG->CHAN, HEX);
    }

    return STATUS;
}

RET_STATUS LoraModule::Read_module_version(struct MVerstruct *Mver) {
    RET_STATUS STATUS = RET_SUCCESS;

    //1. Read UART buffer.
    cleanUARTBuf();

    //2. send CMD
    triple_cmd(R_MODULE_VERSION);

    //3. Receive configure
    STATUS = Module_info((uint8_t*) Mver, sizeof(MVerstruct));
    if(STATUS == RET_SUCCESS) {
        Serial.print("  HEAD:     0x");  Serial.println(Mver->HEAD, HEX);
        Serial.print("  Model:    0x");  Serial.println(Mver->Model, HEX);
        Serial.print("  Version:  0x");  Serial.println(Mver->Version, HEX);
        Serial.print("  features: 0x");  Serial.println(Mver->features, HEX);
    }

    return STATUS;
}

void LoraModule::Reset_Module() {
    triple_cmd(W_RESET_MODULE);

    WaitAUX_H();
    delay(1000);
}

RET_STATUS LoraModule::SleepModeCmd(uint8_t CMD, void *pBuff) {
    RET_STATUS STATUS = RET_SUCCESS;

    Serial.print("SleepModeCmd: 0x"); Serial.println(CMD, HEX);
    WaitAUX_H();

    SwitchMode(MODE_3_SLEEP);

    switch (CMD) {
        case W_CFG_PWR_DWN_SAVE:
            STATUS = Write_CFG_PDS((struct CFGstruct*)pBuff);
            break;
        case R_CFG:
            STATUS = Read_CFG((struct CFGstruct*)pBuff);
            break;
        case W_CFG_PWR_DWN_LOSE:
            break;
        case R_MODULE_VERSION:
            Read_module_version((struct MVerstruct*)pBuff);
            break;
        case W_RESET_MODULE:
            Reset_Module();
            break;
        default:
            return RET_INVALID_PARAM;
    }
    WaitAUX_H();
    return STATUS;
}

//TODO: Modify this to support runtime configuration
RET_STATUS LoraModule::SettingModule(struct CFGstruct *pCFG) {
    RET_STATUS STATUS = RET_SUCCESS;

    STATUS = SleepModeCmd(W_CFG_PWR_DWN_SAVE, (void*)pCFG);

    SleepModeCmd(W_RESET_MODULE, NULL);

    STATUS = SleepModeCmd(R_CFG, (void*)pCFG);

    return STATUS;
}

RET_STATUS LoraModule::ReceiveMsg(uint8_t *pdatabuf, uint8_t *data_len) {
    RET_STATUS STATUS = RET_SUCCESS;
    uint8_t idx;

    SwitchMode(MODE_0_NORMAL);
    *data_len = softSerial->available();

    if(*data_len >0) {
        for(idx=0; idx<*data_len; idx++) {
            *(pdatabuf+idx) = softSerial->read();
        }
    } else {
        STATUS = RET_NOT_IMPLEMENT;
    }

    return STATUS;
}

RET_STATUS LoraModule::SendMsg(uint8_t& dest_addr_h, uint8_t& dest_addr_l,
        uint8_t channel, void *data, uint8_t data_len) {
    RET_STATUS STATUS = RET_SUCCESS;

    SwitchMode(MODE_0_NORMAL);

    if(ReadAUX()!=HIGH)
        return RET_NOT_IMPLEMENT;
    delay(10);
    if(ReadAUX()!=HIGH)
        return RET_NOT_IMPLEMENT;

    auto* sendBuffer = (uint8_t*)malloc(data_len+3);
    sendBuffer[0] = dest_addr_h;
    sendBuffer[1] = dest_addr_l;
    sendBuffer[2] = channel;
    memcpy(data, sendBuffer+3, data_len);

    softSerial->write(sendBuffer, data_len+3);

    delete sendBuffer;

    return STATUS;
}







