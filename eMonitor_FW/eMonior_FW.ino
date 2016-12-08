/*
 WiFiEsp example: WiFi UDP Send and Receive String

 This sketch wait an UDP packet on localPort using a WiFi shield.
 When a packet is received an 'ACK' packet is sent to the client on port remotePort.

 For more details see: http://yaab-arduino.blogspot.com/p/wifiesp-example-client.html
*/

#include "DHT.h"

#define DHTPIN 2     // what digital pin we're connected to
//Mic A0
//SDA A4 SCL A5

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

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

char bufferChar[150];

WiFiEspUDP Udp;

void setup() {
   randomSeed(analogRead(0));
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial1.begin(9600);

  dht.begin();

 
   Serial.println(readSensors(bufferChar, sizeof bufferChar));
Serial.println();
   
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
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
    Serial.println("Contents:");
    Serial.println(packetBuffer);
   
    if (String("readSensors").equals(packetBuffer)) {
       Serial.println("--------------------------------- Reading sensors --------------------------------- ");
        // send a reply, to the IP address and port that sent us the packet we received
    
    Serial.println(readSensors(bufferChar, sizeof bufferChar));
    //Serial.println(Udp.remotePort());
    //Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.beginPacket(Udp.remoteIP(), 10000);
    Udp.write(readSensors(bufferChar, sizeof bufferChar));
    //Udp.write("other data");
    Udp.endPacket();
      
    }

  
    
  }
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



char *readSensors(char result[], size_t result_sz) {
  char buf01[20], buf02[20];

  strcpy(result, readHumidity(buf01, sizeof buf01));
  strcat(result, SEPARATOR);
  strcat(result, readSound(buf02, sizeof buf02));
  //strcat(result, readHumidity(buf01, sizeof buf01));
  //readHumidity(bufferChar, sizeof bufferChar);
  //readSound(bufferChar, sizeof bufferChar);
  return result; 
}

//----------------------------
char *readSound(char result[], size_t result_sz) {
 
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
double scale= 10;  

   unsigned long startMillis= millis();  // Start of sample window
   unsigned int peakToPeak = 0;   // peak-to-peak level

   unsigned int signalMax = 0;
   unsigned int signalMin = 1024;

   // collect data 
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(0);
      if (sample < 1024)  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;  // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;  // save just the min levels
         }
      }
   }
   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
   double level = (peakToPeak * scale);  // convert to volts;

char buf01[100];
  dtostrf(level,1,2,buf01);

  strcpy(result, buf01);
  
return result; 
}


//----------------------------
char *readPressure(char result[], size_t result_sz) {
 // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float w = 154.25;
  // Read temperature as Celsius (the default)
  float e = 197.24;
char buf01[100], buf02[100];
  dtostrf(w,1,2,buf01);
  dtostrf(e,1,2,buf02);
 
  strcpy(result, buf01);
  strcat(result, SEPARATOR);
  strcat(result, buf02);
  
return result; 
}


//----------------------------
char *readlight(char result[], size_t result_sz) {
 // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float w = 154.25;
  // Read temperature as Celsius (the default)
  float e = 197.24;
char buf01[100], buf02[100];
  dtostrf(w,1,2,buf01);
  dtostrf(e,1,2,buf02);
 
  strcpy(result, buf01);
  strcat(result, SEPARATOR);
  strcat(result, buf02);
  
return result; 
}


//----------------------------
char *readHumidity(char result[], size_t result_sz) {

 // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) ) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

 char buf01[6], buf02[6];
  dtostrf(t,1,2,buf01);
  dtostrf(h,1,2,buf02);
 
  strcpy(result, buf01);
  strcat(result, SEPARATOR);
  strcat(result, buf02);

return result; 

}


//----------------------------
char *readGases(char result[], size_t result_sz) {
 // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float w = 154.25;
  // Read temperature as Celsius (the default)
  float e = 197.24;
char buf01[100], buf02[100];
  dtostrf(w,1,2,buf01);
  dtostrf(e,1,2,buf02);
 
  strcpy(result, buf01);
  strcat(result, SEPARATOR);
  strcat(result, buf02);
  
return result; 
}


