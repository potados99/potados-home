#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include <CoapServer.h>
#include <Device.h>
#include "oled.h"

#include "auth.h"

WiFiManager wifiManager;
WiFiUDP udp;
CoapServer server(udp);

Device myLight("MyLight", D4);
OLED disp;

void splash();
void connectWiFi();
void showInfo();
void resetNeeded();
void (*reset)(void) = 0;

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
            break;
        }

        case COAP_PUT: {
            if (payload == NULL) return;

            if (msg == "ON") {
                myLight.setPower(true);

                disp.forceUnlock()
                -> clear()
                -> setFont(disp.bigFont)
                -> drawStr(0, 0, "ON")
                -> setFont(disp.defaultFont)
                -> commit()
                -> lockFor(1000);

                sprintf(reply, "OK");
            }
            else if (msg == "OFF") {
                myLight.setPower(false);

                disp.forceUnlock()
                -> clear()
                -> setFont(disp.bigFont)
                -> drawStr(0, 0, "OFF")
                -> setFont(disp.defaultFont)
                -> commit()
                -> lockFor(1000);

                sprintf(reply, "OK");
            }
            else {
                sprintf(reply, "FAIL");

                disp.forceUnlock()
                -> clear()
                -> setFont(disp.bigFont)
                -> drawStr(0, 0, "ERROR")
                -> setFont(disp.defaultFont)
                -> commit()
                -> lockFor(1000);
            }

            break;
        }

    } /* end of switch */
};

void setup() {
    Serial.begin(115200);
    disp.begin();

    splash();

    delay(500);

    connectWiFi();

    delay(2000);

    server.addResource("power", onPower);
    server.start();
}

void loop() {
    showInfo();

    server.loop();
    disp.loop();
}

void splash() {
    disp.showMovingTitle("APPLE TREE");
    delay(500);
    disp.showMovingTitle("v20190728");
}

void connectWiFi() {
    Serial.print("Configuring Wi-Fi");

    disp.clear()
    -> setFont(u8g2_font_open_iconic_all_2x_t)
    -> drawUTF8(0, 2, "\u00f7") /* antena */
    -> setFont(disp.defaultFont)
    -> drawStr(20, 2, "Connecting")
    -> commit();

    WiFi.hostname("appletree");
    wifiManager.autoConnect("appletree_setup");

    /**
     * Success
     */
    disp.clear()
    -> setFont(u8g2_font_open_iconic_all_2x_t)
    -> drawUTF8(0, 6, "\u0073") /* check mark */
    -> setFont(disp.defaultFont)
    -> drawStr(20, 0, WiFi.SSID().c_str())
    -> drawStr(20, 16, WiFi.localIP().toString().c_str())
    -> commit();

    Serial.print("\nConnected: ");
    Serial.println(WiFi.localIP());
}

void showInfo() {
    bool online = (WiFi.status() == WL_CONNECTED);

    disp.clear();

    if (online) {
        disp.setFont(u8g2_font_open_iconic_all_2x_t)
        -> drawUTF8(110, 6, "\u00b7"); /* heart */
    } else {
        Serial.println("HERE");
        disp.setFont(u8g2_font_open_iconic_all_2x_t)
        -> drawUTF8(110, 6, "\u0118"); /* warning mark */
    }

    disp.setFont(disp.defaultFont)
    -> drawStrf(0, 0, "IP: %s", online ? WiFi.localIP().toString().c_str() : "offline")
    -> drawStrf(0, 16, "Uptime: %d", millis() / 1000)
    -> commit();
}

void resetNeeded() {
    Serial.println("\nReset needed.\n");

    disp.forceUnlock() -> clear() -> setFont(disp.bigFont);

    bool show = true;
    int showCount = 0;

    while (true) {
        disp.clear();

        if (show) {
            disp.drawStr(0, 0, "RESET");
        }
        show = !show;

        disp.commit();

        ++showCount;

        if (showCount > 10) {
            reset();
        }

        delay(400);
    }
}
