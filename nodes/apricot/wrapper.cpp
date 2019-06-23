#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <CoapServer.h>
#include <pdevice.h>
#include "wifi.h"

WiFiUDP udp;
CoapServer server(udp);
Device myLED("MyLED", 2);

char *light_callback(CoapPacket &packet, IPAddress ip, int port);

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(MY_SSID, MY_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // pinMode(3, FUNCTION_3);

  server.addResource(light_callback, "LED");

  server.start();
}

void loop() {
  server.loop();
}

char *light_callback(CoapPacket &packet, IPAddress ip, int port) {

  const char * msg = (const char *)packet.payload ? (const char *)packet.payload : "NULL";
  bool methodIsGet = (packet.code == COAP_GET);

  char *reply = (char *)malloc(32);
  memset(reply, 0, 32);

  if (methodIsGet) {
    sprintf(reply, "Power is %s!", myLED.getPower() ? "On" : "Off");
  }
  else {
    if (strcmp(msg, "on") == 0) {
      myLED.setPower(true);
      sprintf(reply, "Turned On :)");
    }
    else if (strcmp(msg, "off") == 0) {
      myLED.setPower(false);
      sprintf(reply, "Turned Off :(");
    }
    else {
      sprintf(reply, "I don't get it \"%s\"..sorry", msg);
    }
  }

  return reply;
}
