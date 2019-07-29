#ifndef node_h
#define node_h

#include <Arduino.h>

#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <EEPROM.h>

#include <CoapServer.h>
#include <Device.h>
#include "oled.h"

#define ULONG_HIGH    (3000000000UL)
#define ULONG_LOW     (100000UL)

/* Useful Constants */
#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24L)

/* Useful Macros for getting elapsed time */
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)
#define numberOfHours(_time_)   (( _time_ % SECS_PER_DAY) / SECS_PER_HOUR)
#define elapsedDays(_time_)     ( _time_ / SECS_PER_DAY)

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

    bool                nearRollover = false;
    int                 rollover = 0;

    unsigned long       lastUpdate = 0;
    int                 barLocation = 0;

    void                splash();
    void                connectWiFi();
    void                showInfo();
    void                resetNeeded();

    unsigned long       secs();

    void                increassResetCount();
    unsigned long       getResetCount();

public:
    Node(String name, int pin): mName(name), mDevice(name, pin), mWiFiManager(), mUdp(), mServer(mUdp), mOled() {}

    void setup();
    void loop();

    void addResource(String url, callback c);

    OLED *display();
    Device *device();
};

#endif /* node_h */
