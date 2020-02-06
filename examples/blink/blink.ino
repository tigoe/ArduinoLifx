
/*
  WiFi LIFX Broadcast example

  This sketch sents a LIFX LAN UDP packet to set the color of a light
  listens for replies.

  created 28 Jan 2018
  by Tom Igoe

*/

#include <SPI.h>
#include <WiFi101.h>
//#include <ESP8266WiFi.h>    // use this instead of WiFi101 for ESP8266 modules
#include <WiFiUdp.h>
#include <ArduinoLifx.h>
#include "arduino_secrets.h"

char packetBuffer[255];

WiFiUDP Udp;
ArduinoLifx lifx(Udp);

char target[] = "128.122.6.12";
int  packetsSent = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Starting");
  
  WiFi.begin(ssid, pass);         // try to connect
  // while you're not connected to a WiFi AP,
  while ( WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);           // print the network name (SSID)
    
    delay(2000);                    // wait 2 seconds before next attempt
  }
  char targetMac[] = "d0:73:d5:aa:bb:cc";    // broadcast MAC
  IPAddress myIp = WiFi.localIP();
  // When you're connected, print out the device's network status:
  Serial.print("IP Address: ");
  Serial.println(myIp);
  lifx.begin(myIp, target, targetMac);
  // for directed packets, tagged = 0; for broadcast, tagged =1:
  lifx.setFlags(0, 0, 1);
  // a warm glow:
  lifx.setColor(30, 100, 65535, 3500, 10);
}

void loop() {
  lifx.setPower(1, 750);  // on
  delay(2000);
  lifx.setPower(0, 750); // off
  delay(2000);
  Serial.println(packetsSent);
  packetsSent++;



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
  }
}
