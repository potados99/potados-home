#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <CoapServer.h>
#include "secured.h"

WiFiUDP udp;
CoapServer server(udp);

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
    sprintf(reply, "You want to GET [%s] from me!", msg);
  }
  else {
    sprintf(reply, "You PUT me [%s]!", msg);
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
}
