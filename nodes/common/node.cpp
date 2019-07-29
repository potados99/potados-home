#include "node.h"

void Node::setup() {
    EEPROM.begin(4);
    increassResetCount();

    Serial.begin(115200);
    mOled.begin();

    splash();
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
    Serial.println("MAC address:");
    Serial.println(WiFi.macAddress());

    String nameInUpperCase = String(mName);
    nameInUpperCase.toUpperCase();

    display()
    -> clear()
    -> drawStr(0, 0, "Name:")
    -> drawStr(0, 16, nameInUpperCase.c_str())
    -> commit();

    delay(1000);

    display()
    -> clear()
    -> drawStr(0, 0, "Build Date:")
    -> drawStr(0, 16, __DATE__)
    -> commit();

    delay(1000);

    display()
    -> clear()
    -> drawStr(0, 0, "Reset Count:")
    -> drawStrf(0, 16, "%ld", getResetCount())
    -> commit();

    delay(1000);
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
    mWiFiManager.autoConnect((mName + "_setup").c_str(), "potados");

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
    String uptimeString = "";

    unsigned long now = secs(); /* sec */

    unsigned long d = elapsedDays(now);
    unsigned long h = numberOfHours(now);
    unsigned long m = numberOfMinutes(now);
    unsigned long s = numberOfSeconds(now);

    if (d > 9) {
        uptimeString += d;
        uptimeString += "d";
    }
    else if (d > 0) {
        uptimeString += d;
        uptimeString += "d ";
        uptimeString += h;
        uptimeString += "h";
    }
    else if (h > 0) {
        uptimeString += h;
        uptimeString += "h ";
        uptimeString += m;
        uptimeString += "m";
    }
    else if (m > 0) {
        uptimeString += m;
        uptimeString += "m ";
        uptimeString += s;
        uptimeString += "s";
    }
    else {
        uptimeString += s;
        uptimeString += "s";
    }

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
    -> drawStrf(0, 16, "Uptime: %s", uptimeString.c_str())
    ->drawHLine(barLocation, 31, 32)
    -> commit();

    unsigned long ms = millis();
    /* if over 128 */
    if (barLocation & 0x10000000) {
        /* out of screen */
        if (ms - lastUpdate > 1000) {
            barLocation = -32;
            lastUpdate  = ms;
        }
    }
    else {
        /* in screen */
        barLocation += 6;
    }
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

unsigned long Node::secs() {
    unsigned long ms = millis();

    if (ms > ULONG_HIGH) {
        nearRollover = true;
    }

    if (ms < ULONG_LOW && nearRollover) {
        nearRollover = false;
        ++rollover;
    }

    return ((ULONG_MAX / 1000) * rollover) + (ms / 1000);
}

void Node::increassResetCount() {
    unsigned long currentResetCount = getResetCount();
    ++currentResetCount;

    for (unsigned int i = 0; i < sizeof(unsigned long); /* 4 */ ++i) {
        EEPROM.write(i, (currentResetCount >> (8 * i)) & 0xFF /* byte */);
    }

    EEPROM.commit();
}

unsigned long Node::getResetCount() {
    unsigned long resetCount = 0;
    for (unsigned int i = 0; i < sizeof(unsigned long); /* 4 */ ++i) {
        resetCount |= (EEPROM.read(i) << (8 * i));
    }

    return resetCount;
}
