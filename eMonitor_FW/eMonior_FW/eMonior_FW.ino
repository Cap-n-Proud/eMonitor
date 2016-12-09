/*
 WiFiEsp example: WiFi UDP Send and Receive String

 This sketch wait an UDP packet on localPort using a WiFi shield.
 When a packet is received an 'ACK' packet is sent to the client on port remotePort.

 For more details see: http://yaab-arduino.blogspot.com/p/wifiesp-example-client.html
*/

#include "DHT.h"

#define DHTPIN 5     // what digital pin we're connected to
#define PHOTOPIN 0     
#define TGS2602PIN 3     // what digital pin we're connected to

#define lightReadingSamples 10
#define lightReadingDelay 10

#define gasesReadingSamples 3
#define gasesReadingDelay 10

//Mic A2
//SDA A4 SCL A5
//Photoresistor A0
//Barometric 280 I2C
//TGS2602 A2

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

#include <WiFiEsp.h>
#include <WiFiEspUdp.h>

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#endif

  
char SEPARATOR[1] = ", ";
char ssid[] = "homeWiFi";            // your network SSID (name)
char pass[] = "TCOkbdJdYD7B8ZLEsTf8";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

unsigned int localPort = 10002;  // local port to listen on
unsigned long timeNow;

char packetBuffer[50];          // buffer to hold incoming packet
char bufferChar[50];

WiFiEspUDP Udp;

void setup() {
   randomSeed(analogRead(0));
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial1.begin(9600);

  dht.begin();

 
  // Serial.println(readSensors(bufferChar, sizeof bufferChar));
//Serial.println();
   
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }
  
  Serial.println("Connected to wifi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  Udp.begin(localPort);
  
  Serial.print("Listening on port ");
  Serial.println(localPort);


}

void loop() {

  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    /*Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());
*/
    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
  //  Serial.println("Contents:");
  //  Serial.println(packetBuffer);
   
    if (String("readSensors").equals(packetBuffer)) {
    /*Serial.println("--------------------------------- Reading sensors --------------------------------- ");
        // send a reply, to the IP address and port that sent us the packet we received
    Serial.println("Temp, Humidity, Sound, Light, Gases");
    Serial.println(readSensors(bufferChar, sizeof bufferChar));*/
    //Serial.println(Udp.remotePort());
    //Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.beginPacket(Udp.remoteIP(), 10000);
    readSensors();
    //Udp.write(readSensors(bufferChar, sizeof bufferChar));
    //Udp.write("other data");
    Udp.endPacket();
      
    }

  
    
  }
}



void readSensors() {
  char buf01[20];

  Udp.write(readHumidity(buf01, sizeof buf01));
  Udp.write(readTemperature(buf01, sizeof buf01));
  Udp.write(readSound(buf01, sizeof buf01));
  Udp.write(readLight(buf01, sizeof buf01));
  Udp.write(readGases(buf01, sizeof buf01));
  
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


