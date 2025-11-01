#include <NexusTX.h>

// Example: SX1278 in OOK direct mode using built-in library configuration.

// Pins (adjust to your wiring)
static const uint8_t PIN_SX_NSS  = 10;   // SX1278 CS (SS). Set to 0xFF if you don't want the library to touch SPI.
static const uint8_t PIN_SX_DATA = 5;    // MCU GPIO wired to SX1278 direct OOK data input (often DIO2)
static const int8_t  PIN_SX_RST  = -1;   // Optional SX1278 RESET pin (-1 = not used)

// Create the transmitter (legacy constructor still works, but we'll reconfigure below)
NexusTX transmitter(2);

void setup() {
  Serial.begin(115200);

  // Select SX1278 radio and configure pins.
  // If you pass a valid NSS pin, the library will perform a minimal best-effort init for OOK direct mode.
  // If you pass 0xFF as NSS, it will just use DATA as a plain output and assume the module is preconfigured.
  transmitter.configSX1278(PIN_SX_DATA, PIN_SX_NSS, PIN_SX_RST);

  transmitter.setChannel(0);
  transmitter.setBatteryFlag(1);
  transmitter.setTemperature(23.4);
  transmitter.setHumidity(45);
  transmitter.setId(12);
}

void loop() {
  if (transmitter.transmit()) {
    // Optional: print the bits we just sent
    for (int i = 0; i < transmitter.buffer_size; i++) {
      Serial.print(transmitter.SendBuffer[i] ? '1' : '0');
    }
    Serial.println();
  }
}
