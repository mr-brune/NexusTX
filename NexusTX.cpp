#include "NexusTX.h"
#include <SPI.h>

NexusTX::NexusTX(byte tr_pin)
{
  SendBuffer = new bool[buffer_size];
  TX_PIN = tr_pin;
  pinMode(TX_PIN, OUTPUT);
  digitalWrite(TX_PIN, LOW);
  _radioType = RADIO_FS1000A;
  SendBuffer[9] = 0;
    SendBuffer[24] = 1;
    SendBuffer[25] = 1;
    SendBuffer[26] = 1;
    SendBuffer[27] = 1;

}

void NexusTX::setTxWriter(void (*writeFn)(bool level, void* ctx), void* ctx)
{
  _txWriter = writeFn;
  _txWriterCtx = ctx;
}

void NexusTX::setRadio(NexusRadioType type)
{
  _radioType = type;
}

void NexusTX::configFS1000A(uint8_t dataPin)
{
  _radioType = RADIO_FS1000A;
  TX_PIN = dataPin;
  pinMode(TX_PIN, OUTPUT);
  digitalWrite(TX_PIN, LOW);
}

void NexusTX::configSX1278(uint8_t dataPin, uint8_t nssPin, int8_t rstPin)
{
  _radioType = RADIO_SX1278;
  SX_DATA_PIN = dataPin;
  TX_PIN = dataPin; // reuse the same TX path (direct data input)
  pinMode(SX_DATA_PIN, OUTPUT);
  digitalWrite(SX_DATA_PIN, LOW);

  SX_NSS_PIN = nssPin;
  SX_RST_PIN = rstPin;
  _sxUseSPI = (SX_NSS_PIN != 0xFF);

  if (_sxUseSPI) {
    // Basic SPI setup and optional reset
    SPI.begin();
    pinMode(SX_NSS_PIN, OUTPUT);
    digitalWrite(SX_NSS_PIN, HIGH);
    if (SX_RST_PIN >= 0) {
      sxResetPulse();
    }
    sx1278InitDefault(); // best-effort minimal init for OOK direct mode
  }
}


void NexusTX::tx_bit(bool b)
{
  if (_txWriter) {
    _txWriter(true, _txWriterCtx);
  } else {
    digitalWrite(TX_PIN, HIGH);
  }
  delayMicroseconds(PULSE_HIGH);
  if (_txWriter) {
    _txWriter(false, _txWriterCtx);
  } else {
    digitalWrite(TX_PIN, LOW);
  }
  if (b == true)
    delayMicroseconds(PULSE_ONE);
  else
    delayMicroseconds(PULSE_ZERO);
}

void NexusTX::setBatteryFlag(bool level)
{
  SendBuffer[8] = level;
}

void NexusTX::setHumidity(int h)
{
uint8_t h8 = (uint8_t)h;
for (idx = 0; idx <= 7; idx++)
{
    SendBuffer[35 - idx] = (h8 >> idx) & 0x1;
}

}

void NexusTX::setChannel(byte dev_ch)
{
  SendBuffer[10] = dev_ch & 0x2;
  SendBuffer[11] = dev_ch & 0x1;
}  

void NexusTX::setId(byte dev_id)
{
    int array_idx = 0;
    for (idx = 7; idx >= 0; idx--)
    {
        SendBuffer[array_idx++] = dev_id & (0x1 << idx);
    }
}

void NexusTX::setTemperature(float t)
{
int16_t t12 = t * 10.0f;
for (idx = 0; idx <= 11; idx++)
{
    SendBuffer[23 - idx] = (t12 >> idx) & 0x1;
}
}

void NexusTX::SendNexus()
{
    for (int i=0; i <buffer_size; i++)
    {
        tx_bit(SendBuffer[i]);
    }
}

void NexusTX::SendPacket()
{
  for (idx = 1; idx < repeat; idx++)
  {
    SendNexus();
    if (idx + 1 == repeat) {break;} // do not send sync after last TX
    // sync bit
    if (_txWriter) {
      _txWriter(true, _txWriterCtx);
    } else {
      digitalWrite(TX_PIN, HIGH);
    }
    delayMicroseconds(PULSE_HIGH);
    if (_txWriter) {
      _txWriter(false, _txWriterCtx);
    } else {
      digitalWrite(TX_PIN, LOW);
    }
    delayMicroseconds(PULSE_SYNC);
  }
}


bool NexusTX::transmit()
{
  if (millis() >= time_marker_send && send_time)
  {
    time_marker_send = millis() + send_time;
    SendPacket();
    return true;
  }
  else return false;
}

// ===========================
// SX1278 helpers (best-effort)
// ===========================

void NexusTX::sxSelect() {
  if (_sxUseSPI) digitalWrite(SX_NSS_PIN, LOW);
}

void NexusTX::sxDeselect() {
  if (_sxUseSPI) digitalWrite(SX_NSS_PIN, HIGH);
}

void NexusTX::sxWriteReg(uint8_t addr, uint8_t val) {
  if (!_sxUseSPI) return;
  sxSelect();
  SPI.transfer(addr | 0x80);
  SPI.transfer(val);
  sxDeselect();
}

uint8_t NexusTX::sxReadReg(uint8_t addr) {
  if (!_sxUseSPI) return 0;
  sxSelect();
  SPI.transfer(addr & 0x7F);
  uint8_t v = SPI.transfer(0x00);
  sxDeselect();
  return v;
}

void NexusTX::sxResetPulse() {
  if (SX_RST_PIN < 0) return;
  pinMode(SX_RST_PIN, OUTPUT);
  digitalWrite(SX_RST_PIN, LOW);
  delay(1);
  digitalWrite(SX_RST_PIN, HIGH);
  delay(10);
}

void NexusTX::sx1278InitDefault() {
  // IMPORTANT: Exact register values depend on board variant.
  // This is a cautious minimal sequence to ensure FSK/OOK mode and a sane standby.
  // Users may need to refine frequency, power, and DIO mapping as per SX1278MA datasheet.

  // RegOpMode (0x01): set to Sleep, FSK/OOK (LongRangeMode=0)
  sxWriteReg(0x01, 0x00); // Sleep, LFMode off; LongRangeMode=0
  delay(1);

  // RegDataModul (0x02): OOK, continuous mode, no shaping (typical: 0b01100000 for OOK + continuous w/o bit sync)
  // Note: Values may require adjustment; consult datasheet for exact bitfields.
  sxWriteReg(0x02, 0x60);

  // Set a typical FRF ~433.92 MHz (RegFrfMsb/Mid/Lsb at 0x07/0x08/0x09 for SX127x)
  // FRF = (freq / Fstep), Fstep=61.035 Hz @ 32 MHz. For 433.92 MHz -> FRF ≈ 0x6C8000.
  sxWriteReg(0x07, 0x6C);
  sxWriteReg(0x08, 0x80);
  sxWriteReg(0x09, 0x00);

  // PA config and ramp may need board-specific tuning; keep defaults

  // Standby
  sxWriteReg(0x01, 0x01); // Standby, FSK/OOK mode
  delay(1);
}

