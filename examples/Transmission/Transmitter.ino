#include "NexusTX.h"


NexusTX transmitter(15); //15 = data pin

void setup()
{
  Serial.begin(115200);     
  
  transmitter.setChannel(0);  //ch1 = 0      
  transmitter.setBatteryFlag(0);    //battery flag 0 = ok
  transmitter.setTemperature(26.30); 
  transmitter.setHumidity(50);      
  transmitter.setId(12); 
}

void loop()
{  
    if (transmitter.transmit()) PrintSentData(transmitter.SendBuffer, transmitter.buffer_size);
    delay(transmitter.tx_interval); //send messagge every 56.75 seconds
  }

void PrintSentData(bool* SendBuffer, int size) {
    for(int i = 0; i < size; i++) {
        Serial.print(SendBuffer[i] ? "1" : "0");
        Serial.print("");
    }
    Serial.println();
}


