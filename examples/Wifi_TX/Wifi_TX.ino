#include <NexusTX.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Define your OpenWeatherMap API key
const char *apiKey = "";

// Define your city
const char *city = "Padova";

// Define password and ssid of wifi
const char *ssid = "";
const char *password = "";


unsigned long timerDelay = 180000; //ask api every 3 minute
unsigned long lastTime = 0;
unsigned long lastTime1 = 0;
bool first = true;
NexusTX transmitter(15);

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  transmitter.setChannel(0);      // ch1 = 0
  transmitter.setBatteryFlag(1);  // battery flag 0 = low charge
  transmitter.setTemperature(0);
  transmitter.setHumidity(0);
  transmitter.setId(12);
}

void loop() {
  if ((millis() - lastTime) > timerDelay || first) {
    // Check WiFi connection status
    
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;

      // Specify request destination
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + String(city) + "&appid=" + String(apiKey);

      WiFiClient client;  // Create a WiFiClient object
      http.begin(client, serverPath.c_str());
      // Check HTTP response
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
        String payload = http.getString();
        // Get temperature from payload
        int tempStartIndex = payload.indexOf("temp") + 6;
        int tempEndIndex = payload.indexOf(",", tempStartIndex);
        String tempString = payload.substring(tempStartIndex, tempEndIndex);
        float temperature = tempString.toFloat() - 273.15;  // Convert from Kelvin to Celsius

        // Get humidity from payload
        int humStartIndex = payload.indexOf("humidity") + 10;
        int humEndIndex = payload.indexOf(",", humStartIndex);
        String humString = payload.substring(humStartIndex, humEndIndex);
        float humidity = humString.toFloat();

        // Store the temperature and humidity
        transmitter.setTemperature(temperature);
        transmitter.setHumidity(humidity);
        Serial.println(temperature);
        Serial.println(humidity);
      } else {
        Serial.print("Error on sending GET request: ");
        Serial.println(httpResponseCode);
      }

      // Free resources
      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }

    lastTime = millis();
  }

  if ((millis() - lastTime1) > transmitter.tx_interval || first) {
    // Transmit data
    lastTime1 = millis();
    if (transmitter.transmit()) PrintSentData(transmitter.SendBuffer, transmitter.buffer_size);
    first = false;
    
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PrintSentData(bool *SendBuffer, int size) {
  for (int i = 0; i < size; i++) {
    Serial.print(SendBuffer[i] ? "1" : "0");
    Serial.print("");
  }
  Serial.println();
}
