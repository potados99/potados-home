#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <CoapServer.h>
#include <Device.h>
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"

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
Device myLight("MyLight", D4);

/**
 * Display on the board.
 */
SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_32);

String currentLine1 = "";
String currentLine2 = "";

unsigned long currentTextFrom = 0;
unsigned long currentTextUntil = 0;

unsigned long displayUpdateInterval = 10;
unsigned long lastDisplayUpdate = 0;

bool textModified = false;
bool textExpired = true;

/**
 * Display text when it is valid.
 * if the text is expired, it is turned to "".
 */
void updateDisplay();

/**
 * Register text to be displayed for m milliseconds.
 */
void showTextFor(String line1, String line2 = "", unsigned long mseconds = 0);

/**
 * Display current status of device.
 * if any texts are on the display(registered by showTextFor),
 * it won't work.
 */
void showStatus();

/**
 * Callback for "power"
 */
callback onPower = [](int coap_method, const char *payload, char *reply) {
    if (reply == NULL) return;

    String msg(payload);
    msg.toUpperCase();

    switch (coap_method) {

      case COAP_GET: {
        sprintf(reply, myLight.getPower() ? "ON." : "OFF.");
        showTextFor("CoAP", myLight.getPower() ? "Currently on." : "Currently off.", 2000);
        break;
      }

      case COAP_PUT: {
        if (payload == NULL) return;

        if (msg == "ON") {
          myLight.setPower(true);
          sprintf(reply, "OK");
          showTextFor("CoAP", "Turned on.", 2000);
        }
        else if (msg == "OFF") {
          myLight.setPower(false);
          sprintf(reply, "OK");
          showTextFor("CoAP", "Turned off.", 2000);
        }
        else {
          sprintf(reply, "FAIL");
          showTextFor("CoAP", "Wrong payload.", 2000);
        }

        break;
      }

    } /* end of switch */
};

void setup() {
    Serial.begin(115200);

    WiFi.hostname("appletree");
    WiFi.begin(MY_SSID, MY_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
    }

    showTextFor("Connected:", WiFi.localIP().toString(), 2000);

    Serial.print("\nConnected: ");
    Serial.println(WiFi.localIP());

    server.addResource("power", onPower);
    server.start();

    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_16);
}

void loop() {
    server.loop();

    delay(10);

    updateDisplay();

    delay(10);

    showStatus();

    delay(10);
}

void updateDisplay() {
    if (textExpired) {
        return;
    }

    if (millis() < currentTextUntil) {
        /**
         * text is valid.
         */
         if (textModified) {
             display.clear();

             display.setFont(ArialMT_Plain_16);
             display.drawString(0, 0, currentLine1);

             display.setFont(ArialMT_Plain_10);
             display.drawString(0, 16, currentLine2);

             display.display();
             textModified = false;
         }
    }
    else {
        /**
         * text is expired. invalidate it.
         */
         textExpired = true;
    }
}

void showTextFor(String line1, String line2, unsigned long mseconds) {
    currentLine1 = line1;
    currentLine2 = line2;

    textModified = true;
    textExpired = false;

    /**
     * if mseconds is 0(default),
     * do not update duration or lifespan of current text.
     */
    if (mseconds != 0) {
        currentTextFrom = millis();
        currentTextUntil = currentTextFrom + mseconds;
    }
}

void showStatus() {
    /**
     * do nothing if valid text exists.
     */
    if (!textExpired) {
        return;
    }

    display.clear();

    String status = "normal";

    if (WiFi.status() != WL_CONNECTED) {
        status = "offline";
    }

    // ...

    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "Status: " + status);

    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 16, "Uptime: " + String(millis() / 1000));

    display.display();
}
