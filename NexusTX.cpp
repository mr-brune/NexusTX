#include "NexusTX.h"

NexusTX::NexusTX(byte tr_pin)
{
  SendBuffer = new bool[buffer_size];
  TX_PIN = tr_pin;
  pinMode(TX_PIN, OUTPUT);
  digitalWrite(TX_PIN, LOW);
  SendBuffer[9] = 0;
    SendBuffer[24] = 1;
    SendBuffer[25] = 1;
    SendBuffer[26] = 1;
    SendBuffer[27] = 1;

}


void NexusTX::tx_bit(bool b)
{
  digitalWrite(TX_PIN, HIGH);
  delayMicroseconds(PULSE_HIGH);
  digitalWrite(TX_PIN, LOW);
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

void NexusTX::SendPacket(bool first)
{
  int i=0;

  if(first){i=4;}

    for (i; i <buffer_size; i++)
    {
        tx_bit(SendBuffer[i]);
    }
}


bool NexusTX::transmit()
{
  SendPacket(true);
  for (idx = 1; idx < repeat; idx++)
  {
    SendPacket(false);

    if (idx + 1 == repeat) {break;} // do not send sync after last TX
    // sync bit
    digitalWrite(TX_PIN, HIGH);
    delayMicroseconds(PULSE_HIGH);
    digitalWrite(TX_PIN, LOW);
    delayMicroseconds(PULSE_SYNC);
  }
  return true;
}