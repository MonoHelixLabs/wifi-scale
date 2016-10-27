/***************************************************
  This sketch shows how to send data from the Feather Huzzah to Adafruit IO
  when the Feather Huzzah uses:
  - 3 x Force Sensitive Resistors (FSRs)
  - 3 x DS18B20 digital temperature sensors
  - 1 x CD4051 analog multiplexer
  - 1 x DHT22 temperature and humidity sensor.
  
  This sketch is based on the Adafruit MQTT Library ESP8266 Example:
  
  Adafruit MQTT Library ESP8266 Example

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino

  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#include "DHT.h"

#include <OneWire.h>
#include <DallasTemperature.h>

/************************* DHT ***********************************************/

#define DHTPIN 14
#define DHTTYPE DHT22   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

/************************* DS18B20 ***********************************************/

// Data wire is plugged into this pin on the Arduino
#define ONE_WIRE_BUS 5

#define TEMPERATURE_PRECISION 9 // 9 to 12

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress tmp1addr, tmp2addr, tmp3addr;

/************************* MUX ***********************************************/

// initialise vars used for selection of pins
int r0 = 0;      //value of select pin at the 4051 (s0)
int r1 = 0;      //value of select pin at the 4051 (s1)
int r2 = 0;      //value of select pin at the 4051 (s2)
int count = 0;   //which y pin we are selecting

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "" // add network name here
#define WLAN_PASS       "" // add network password here

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  8883  //1883                   // use 8883 for SSL
#define AIO_USERNAME    "" // add adafruit IO username here
#define AIO_KEY         "" // add adafruit AIO key here

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
//WiFiClient client;
// or... use WiFiFlientSecure for SSL
WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish tmpdht = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/tmpdht");
Adafruit_MQTT_Publish humdht = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humdht");
Adafruit_MQTT_Publish fsr1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/fsr1");
Adafruit_MQTT_Publish fsr2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/fsr2");
Adafruit_MQTT_Publish fsr3 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/fsr3");
Adafruit_MQTT_Publish tmp1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/tmp1");
Adafruit_MQTT_Publish tmp2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/tmp2");
Adafruit_MQTT_Publish tmp3 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/tmp3");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  Serial.begin(115200);

  // DHT sensor:
  dht.begin();

  // MUX pin selection:
  pinMode(12, OUTPUT);    // s0
  pinMode(13, OUTPUT);    // s1
  pinMode(15, OUTPUT);    // s2
  
  delay(10);

  // Start up the library
  sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement

  if (!sensors.getAddress(tmp1addr, 0)) Serial.println("Unable to find address for Device 0"); 
  if (!sensors.getAddress(tmp2addr, 1)) Serial.println("Unable to find address for Device 1"); 
  if (!sensors.getAddress(tmp3addr, 2)) Serial.println("Unable to find address for Device 1"); 

  // set the resolution per device
  sensors.setResolution(tmp1addr, TEMPERATURE_PRECISION);
  sensors.setResolution(tmp2addr, TEMPERATURE_PRECISION);
  sensors.setResolution(tmp3addr, TEMPERATURE_PRECISION);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();


  /************************* DHT ***********************************************/

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Now we can publish stuff!
  Serial.print(F("\nSending tmp1 val "));
  Serial.println(t);
  if (! tmpdht.publish(t)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  Serial.print(F("Sending hum1 val "));
  Serial.println(h);
  if (! humdht.publish(h)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }


  /************************* DS18B20 ***********************************************/
  
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  sensors.requestTemperatures(); // Send the command to get temperatures
  
  Serial.print("Temperature for Device 1 is: ");
  float temperatureC1 = sensors.getTempCByIndex(0);
  Serial.println(temperatureC1); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  if (! tmp1.publish(temperatureC1)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  Serial.print("Temperature for Device 2 is: ");
  float temperatureC2 = sensors.getTempCByIndex(1);
  Serial.println(temperatureC2); 
  if (! tmp2.publish(temperatureC2)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  
  Serial.print("Temperature for Device 3 is: ");
  float temperatureC3 = sensors.getTempCByIndex(2);
  Serial.println(temperatureC3); 
  if (! tmp3.publish(temperatureC3)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  /************************* MUX ***********************************************/

  for (count=0; count<=2; count++) {

    if (count == 0 || count == 1 || count == 2) {
  
      // select the bit  
      r0 = bitRead(count,0);    // use this with arduino 0013 (and newer versions)     
      r1 = bitRead(count,1);    // use this with arduino 0013 (and newer versions)     
      r2 = bitRead(count,2);    // use this with arduino 0013 (and newer versions)     
   
      digitalWrite(12, r0);
      digitalWrite(13, r1);
      digitalWrite(15, r2);
  
      //Either read or write the multiplexed pin here
      // read and transform
      int reading = analogRead(A0);
      Serial.print("Reading: ");
      Serial.print(reading);
        
      if (reading < 10) {
        Serial.println(" - No pressure");
      } else if (reading < 200) {
        Serial.println(" - Light touch");
      } else if (reading < 500) {
        Serial.println(" - Light squeeze");
      } else if (reading < 800) {
        Serial.println(" - Medium squeeze");
      } else {
        Serial.println(" - Big squeeze");
      }
     
     if (count == 2) {
       if (! fsr1.publish(reading)) {
          Serial.println(F("Failed"));
        } else {
          Serial.println(F("OK!"));
        }
     }
     else if (count == 1) {
       if (! fsr2.publish(reading)) {
          Serial.println(F("Failed"));
        } else {
          Serial.println(F("OK!"));
        }
     }
     else if (count == 0) {
       if (! fsr3.publish(reading)) {
          Serial.println(F("Failed"));
        } else {
          Serial.println(F("OK!"));
        }
     }
    }
      
      delay(1000);
  
      
  }

  delay(10000);

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
