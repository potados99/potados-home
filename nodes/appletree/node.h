#ifndef node_h
#define node_h

#include <Arduino.h>

#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include <CoapServer.h>
#include <Device.h>
#include "oled.h"

/**
 * Wrapper class for potados-home node.
 */
class Node {
private:
    String              mName;
    Device              mDevice;

    WiFiManager         mWiFiManager;
    WiFiUDP             mUdp;
    CoapServer          mServer;
    OLED                mOled;

    void                splash();
    void                connectWiFi();
    void                showInfo();
    void                resetNeeded();

public:
    Node(String name, int pin): mName(name), mDevice(name, pin), mWiFiManager(), mUdp(), mServer(mUdp), mOled() {}

    void setup();
    void loop();

    void addResource(String url, callback c);

    OLED *display();
    Device *device();
};

#endif /* node_h */
