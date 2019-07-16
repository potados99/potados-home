#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <CoapServer.h>
#include <Device.h>

#include "auth.h" /* Wi-Fi ssid and password. */

/**
 * Send data through this.
 * CoapServer needs it.
 */
WiFiUDP udp;

/**
 * The CoAP server.
 */
CoapServer server(udp);

/**
 * Device to control.
 */
Device myLED("MyLED", D4);

/**
 * Callback for "power" resource.
 */
callback onPower = [](int coap_method, const char *payload, char *reply) {
    if (reply == NULL) return;

    String msg(payload);
    msg.toUpperCase();

    switch (coap_method) {

      case COAP_GET: {
        sprintf(reply, myLED.getPower() ? "ON" : "OFF");
        break;
      }

      case COAP_PUT: {
        if (payload == NULL) return;

        if (msg == "ON") {
          myLED.setPower(true);
          sprintf(reply, "OK");
        }
        else if (msg == "OFF") {
          myLED.setPower(false);
          sprintf(reply, "OK");
        }
        else {
          sprintf(reply, "FAIL");
        }

        break;
      }

    } /* end of switch */
};

/**
 * Callback for "brightness" resource.
 */
callback onBrightness = [](int coap_method, const char *payload, char *reply) {
    if (reply == NULL) return;

    String msg(payload);
    msg.toUpperCase();

    switch (coap_method) {

    case COAP_GET: {
      sprintf(reply, "%d", myLED.getPWM());
      break;
    }

    case COAP_PUT: {
      if (payload == NULL) return;

      int brightness = msg.toInt();
      if (brightness == 0) {
        sprintf(reply, "FAIL");
        return;
      }

      myLED.setPWM(brightness);
      sprintf(reply, "OK");

      break;
    }

    } /* end of switch */
};


void setup() {
  Serial.begin(115200);

  WiFi.hostname("apricot");
  WiFi.begin(MY_SSID, MY_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.print("\nConnected: ");
  Serial.println(WiFi.localIP());

  server.addResource("power", onPower);
  server.addResource("brightness", onBrightness);

  server.start();
}

void loop() {
  server.loop();
}
