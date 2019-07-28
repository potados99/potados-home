#include "node.h"

void Node::setup() {
    Serial.begin(115200);
    mOled.begin();

    splash();
    delay(500);
    connectWiFi();
    delay(2000);

    mServer.start();
}

void Node::loop() {
    showInfo();

    mServer.loop();
    mOled.loop();
}

void Node::addResource(String url, callback c) {
    mServer.addResource(url, c);
}

OLED *Node::display() {
    return &mOled;
}

Device *Node::device() {
    return &mDevice;
}


void Node::splash() {
    display()
    -> clear()
    -> drawBig("POTADOS")
    -> commit();

    delay(500);

    display()
    -> clear()
    -> drawBig("v0.1.0")
    -> commit();
}

void Node::connectWiFi() {
    Serial.print("Configuring Wi-Fi");

    display()
    -> clear()
    -> setFont(u8g2_font_open_iconic_all_2x_t)
    -> drawUTF8(0, 2, "\u00f7") /* antena */
    -> setFont(NULL)
    -> drawStr(20, 2, "Connecting")
    -> commit();

    WiFi.hostname(mName);
    mWiFiManager.autoConnect((mName + "_setup").c_str());

    /**
     * Success
     */
    display()
    -> clear()
    -> setFont(u8g2_font_open_iconic_all_2x_t)
    -> drawUTF8(0, 6, "\u0073") /* check mark */
    -> setFont(NULL)
    -> drawStr(20, 0, WiFi.SSID().c_str())
    -> drawStr(20, 16, WiFi.localIP().toString().c_str())
    -> commit();

    Serial.print("\nConnected: ");
    Serial.println(WiFi.localIP());
}

void Node::showInfo() {
    bool online = (WiFi.status() == WL_CONNECTED);

    display() -> clear();

    if (online) {
        display()
        -> setFont(u8g2_font_open_iconic_all_2x_t)
        -> drawUTF8(110, 6, "\u00b7"); /* heart */
    } else {
        display()
        -> setFont(u8g2_font_open_iconic_all_2x_t)
        -> drawUTF8(110, 6, "\u0118"); /* warning mark */
    }

    display()
    -> setFont(NULL)
    -> drawStrf(0, 0, "IP: %s", online ? WiFi.localIP().toString().c_str() : "offline")
    -> drawStrf(0, 16, "Uptime: %d", millis() / 1000)
    -> commit();
}

void Node::resetNeeded() {
    Serial.println("\nReset needed.\n");

    display() -> forceUnlock();

    bool show = true;
    int showCount = 0;

    while (true) {
        display() -> clear();

        if (show) {
            display() -> drawBig("RESET");
        }
        show = !show;

        display() -> commit();

        ++showCount;

        if (showCount > 10) {
            ((void (*)(void))0)(); /* raise wdt reset. */
        }

        delay(400);
    }
}
