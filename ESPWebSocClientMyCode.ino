/*
 * WebSocketClientSSL.ino
 *
 * Created on: 10.12.2015
 *
 * note SSL is only possible with the ESP8266
 *
 */

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
//#include <OneWire.h>
//#include <SPI.h>
//#include <DallasTemperature.h>
#include <DHT.h>

#define DHTPIN 5          //Digital pin connected to the DHT sensor
// Uncomment whatever type of DHT sensor you're using.
#define DHTTYPE DHT11     //DHT 11
//#define DHTTYPE DHT22   //DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   //DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor.

WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

//#define Serial Serial1

void DHTSensor()
{
  float h = dht.readHumidity();        //reads humidity
  float t = dht.readTemperature();     //Read temperature as Celsius (the default)
  //float f = dht.readTemperature(true); //Read temperature as Fahrenheit (isFahrenheit = true)
  
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  //heat index is an index that combines air temperature and relative humidity, in shaded areas, to posit a human-perceived equivalent temperature
  // Compute heat index in Fahrenheit (the default)
  // float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  // float hic = dht.computeHeatIndex(t, h, false);  

  //Serial.print() can also be used. Serial.print() helps dev boards with low RAM.
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  /*Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
  */
}

void soilMoisture()
{
  sensor = analogRead(A0);
  output = (100 - map(sensor, 0, 1023, 0, 100));
  delay(1000);
  //didnt understand below commented lines
  /*
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  Serial.println(temp);
  */
  Serial.print("moisture = ");
  Serial.print(output);
  Serial.println("%");
}

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
 const uint8_t* src = (const uint8_t*) mem;
 Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
 for(uint32_t i = 0; i < len; i++) {
  if(i % cols == 0) {
   Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
  }
  Serial.printf("%02X ", *src);
  src++;
 }
 Serial.printf("\n");
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {


    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[WSc] Disconnected!\n");
            break;
        case WStype_CONNECTED:
            {
                Serial.printf("[WSc] Connected to url: %s\n", payload);

       // send message to server when Connected
    webSocket.sendTXT("{ \"type\": \"register\", \"id\": \"zswitch-1\", \"timestamp\": 0, \"sign\": \"\"}");
            }
            break;
        case WStype_TEXT:
            Serial.printf("[WSc] get text: %s\n", payload);

   // send message to server
   // webSocket.sendTXT("message here");
            break;
        case WStype_BIN:
            Serial.printf("[WSc] get binary length: %u\n", length);
            hexdump(payload, length);

            // send data to server
            // webSocket.sendBIN(payload, length);
            break;
  case WStype_ERROR:   
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT:
  case WStype_FRAGMENT_FIN:
   break;
    }

}

void setup() {
    // Serial.begin(921600);
    Serial.begin(115200);

    pinMode(A0,INPUT);

    dht.begin();

    //Serial.setDebugOutput(true);
    Serial.setDebugOutput(true);

    Serial.println();
    Serial.println();
    Serial.println();

      for(uint8_t t = 4; t > 0; t--) {
          Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
          Serial.flush();
          delay(1000);
      }

    WiFiMulti.addAP("GNXS-181350", "youwillneverknow");

    //WiFi.disconnect();
    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    webSocket.beginSSL("zgate-1.herokuapp.com", 443, "/");
    webSocket.onEvent(webSocketEvent);

}

void loop() {
    webSocket.loop();
    DHTSensor();
    soilMoisture();
}
