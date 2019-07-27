#ifndef oled_h

#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <stdio.h>
#include <stdarg.h>

class OLED {
private:
    /**
     * Heltec WiFi kit 8
     */
    U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2;

    bool isLocked = false;
    long currentLockPostedTime = 0;
    long currentLockDuration = 0;

public:
    const uint8_t *defaultFont = u8g2_font_7x14_tf;
    const uint8_t *bigFont = u8g2_font_logisoso32_tf;

    OLED(): u8g2(U8G2_R0, /* reset=*/ 16, /* clock=*/ 5, /* data=*/ 4) {}

    /**
     * Prepare display.
     * Call this in setup().
     */
    void begin();

    /**
     * Update display on every loop.
     * Call this in loop().
     */
    void loop();

    OLED *clear();
    OLED *setFont(const uint8_t *font);
    OLED *drawStr(int x, int y, const char *str);
    OLED *drawStrf(int x, int y, const char *fmt, ...);
    OLED *drawUTF8(int x, int y, const char *str);
    OLED *lockFor(long duration);
    OLED *forceUnlock();
    OLED *commit();

    void showMovingTitle(const char *title);

    u8g2_uint_t getUTF8Width(const char *text);
};

#endif /* oled_h */
