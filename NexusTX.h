#include <Arduino.h>
#ifndef NexusTX_h
#define NexusTX_h

#define PULSE_HIGH 500  /* 500 us */
#define PULSE_ZERO 1000  /* 1000 us */
#define PULSE_ONE 2000  /* 2000 us */
#define PULSE_SYNC 4000 /* 4000 us */

// Time between two TX = 56.75 seconds
#define TX_INTERVAL 56750

// Bitstream repetition (this device sends the information 10 times)
#define repeat 10



class NexusTX
{
  public:

    // Radio types supported
    enum NexusRadioType : uint8_t { RADIO_FS1000A = 0, RADIO_SX1278 = 1 };

    bool* SendBuffer;
    int buffer_size=36;
    int tx_interval = TX_INTERVAL;
    NexusTX(byte);
    // Optional: override how the TX line is driven (e.g., to support radios like SX1278 in direct OOK mode)
    // writeFn(level, ctx) should set the TX state: true = HIGH (carrier on), false = LOW (carrier off)
    void setTxWriter(void (*writeFn)(bool level, void* ctx), void* ctx);
    // Select radio at runtime (default: RADIO_FS1000A)
    void setRadio(NexusRadioType type);
    // Configure FS1000A (classic single data pin)
    void configFS1000A(uint8_t dataPin);
    // Configure SX1278 in OOK direct mode using a data pin; optionally pass NSS and RST to let the library perform minimal init
    void configSX1278(uint8_t dataPin, uint8_t nssPin = 0xFF, int8_t rstPin = -1);
    void setChannel(byte);
    void setId(byte);
    void setBatteryFlag(bool);
    void setTemperature(float);
    void setHumidity(int);
    bool transmit();
    void SendPacket();
    

  private:
  void tx_bit(bool);
  byte TX_PIN;
  int idx = 0;
  void SendNexus();
  unsigned long time_marker_send = 0;
  unsigned long send_time = TX_INTERVAL;
  // Optional TX writer override (defaults to digitalWrite on TX_PIN)
  void (*_txWriter)(bool level, void* ctx) = nullptr;
  void* _txWriterCtx = nullptr;
  // Radio selection and (optional) SX1278 pins
  NexusRadioType _radioType = RADIO_FS1000A;
  uint8_t SX_NSS_PIN = 0xFF;   // 0xFF = not used
  int8_t  SX_RST_PIN = -1;     // -1   = not used
  uint8_t SX_DATA_PIN = 0xFF;  // data (direct OOK input)
  bool _sxUseSPI = false;
  // Minimal SX1278 helpers (safe no-op if SPI pins not provided)
  void sx1278InitDefault();
  void sxSelect();
  void sxDeselect();
  void sxWriteReg(uint8_t addr, uint8_t val);
  uint8_t sxReadReg(uint8_t addr);
  void sxResetPulse();

};

#endif

