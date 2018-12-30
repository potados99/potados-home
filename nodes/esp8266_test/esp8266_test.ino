#include <pdevice.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <CoapServer.h>
#include "secured.h"

void buttonClicked();

WiFiUDP udp;
CoapServer server(udp);

Device MyRelay("MyRelay", 2);
Button MyButton(0, 1, buttonClicked);

void buttonClicked() {
  Serial.println("Toggle!");
  MyRelay.togglePower();
}

char *myCallback(CoapPacket &packet, IPAddress ip, int port) {
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("connected.");
  
  server.addResource(myCallback, "pdevice/pwr");
  
  server.start();
}

void loop() {
  // put your main code here, to run repeatedly:
  server.loop();
  MyButton.loop();
  // Serial.println(digitalRead(0));
}
