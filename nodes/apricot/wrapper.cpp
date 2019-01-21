#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <CoapServer.h>
#include <pdevice.h>
#include "wifi.h"

WiFiUDP udp;
CoapServer server(udp);

Device myDevice("MyDevice", 2);
Button myButton(0, HIGH, buttonClicked); /* pin 0, pull-up, callback */

void setup() {
  if (! WiFi.getAutoConnect()) {
      WiFi.begin(SSID, PASSWORD); /* in wifi.h */
  }

  server.addResource(light_callback, "light");

  server.start();
}

void loop() {
  server.loop();
  myButton.loop();
}


void buttonClicked() {
  myDevice.togglePower();
}

char *light_callback(CoapPacket &packet, IPAddress ip, int port) {
  Serial.println("BGN");

  const char * msg = (const char *)packet.payload ? (const char *)packet.payload : "NULL";
  bool methodIsGet = (packet.code == COAP_GET);

  Serial.print("Method:");
  Serial.println(methodIsGet ? "GET" : "PUT");

  Serial.print("Payload:");
  Serial.println(msg ? msg : "NULL");

  char *reply = (char *)malloc(32);
  memset(reply, 0, 32);

  if (methodIsGet) {
    sprintf(reply, "Power is %s!", MyRelay.getPower() ? "On" : "Off");
  }
  else {
    if (strcmp(msg, "on") == 0) {
      MyRelay.setPower(true);
      sprintf(reply, "Turned On :)");
    }
    else if (strcmp(msg, "off") == 0) {
      MyRelay.setPower(false);
      sprintf(reply, "Turned Off :(");
    }
    else {
      sprintf(reply, "I don't get it \"%s\"..sorry", msg);
    }
  }

  Serial.print("Reply:");
  Serial.println(reply);
  Serial.println("END");

  return reply;
}
