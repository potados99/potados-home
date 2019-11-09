#ifndef oled_h
#define oled_h

#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <stdio.h>
#include <stdarg.h>

class OLED {
private:
    /**
     * 128x64 SPI 6pin
     */
    U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2;

    bool                    isLocked = false;
    unsigned long           currentLockPostedTime = 0;
    unsigned long           currentLockDuration = 0;

public:
    const uint8_t           *defaultFont = u8g2_font_7x14_tf;
    const uint8_t           *bigFont = u8g2_font_logisoso32_tf;

    OLED(): u8g2(U8G2_R0, /* cs=*/ D8, /* dc=*/ D4, /* reset=*/ D3) {}

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
    OLED *drawHLine(int x, int y, int length);
    OLED *drawStr(int x, int y, const char *str);
    OLED *drawStrf(int x, int y, const char *fmt, ...);
    OLED *drawBig(const char *str);
    OLED *drawUTF8(int x, int y, const char *str);
    OLED *lockFor(unsigned long duration);
    OLED *forceUnlock();
    OLED *commit();

    void showMovingTitle(const char *title);

    u8g2_uint_t getUTF8Width(const char *text);
};

#endif /* oled_h */
