# NexusTX
Arduino library to transmit with Nexus / Rubiscon protocol used in temperature sensor at 433 Mhz

In example are the way to use the library. 
Wifi_TX pull temperature and humidity from OpenWether and send it to the temperature display


# Resource
https://www.onetransistor.eu/2022/01/send-data-weather-station-lpd.html

https://github.com/invandy/Oregon_NR

## SX1278MA support (experimental)

You can now choose the radio from the library via a variable/API. Default remains the classic single data pin (FS1000A-type modules).

- FS1000A (default):
	- `NexusTX tx(pin);` or `tx.configFS1000A(pin);`

- SX1278 (OOK direct mode):
	- Wire a GPIO from your MCU to the SX1278 module's direct OOK data input (often DIO2).
	- Optionally provide NSS and RST so the library can do a minimal best-effort init.
	- Example:
		- `tx.configSX1278(dataPin, nssPin, rstPin /* or -1 */);`

See `examples/SX1278MA_Direct/SX1278MA_Direct.ino` to get started. The library includes a conservative initialization for OOK direct mode if you pass an `nssPin`. Depending on your specific SX1278MA module, you may need to fine-tune registers (frequency ~433.92 MHz, power path, DIO mapping). Refer to the Semtech SX1278 datasheet.
