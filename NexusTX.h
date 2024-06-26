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
#define repeat 12



class NexusTX
{
  public:

    bool* SendBuffer;
    int buffer_size=36;
    int tx_interval = TX_INTERVAL;
    NexusTX(byte);
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

};

#endif

