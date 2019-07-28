#include "oled.h"

void OLED::begin() {
    this->u8g2.begin();

    /**
     * prepare
     */
    this->u8g2.setFont(defaultFont);
    this->u8g2.setFontRefHeightExtendedText();
    this->u8g2.setDrawColor(1);
    this->u8g2.setFontPosTop();
    this->u8g2.setFontDirection(0);
}

void OLED::loop() {
    if (isLocked) {
        if (millis() - currentLockDuration > currentLockPostedTime) {
            isLocked = false;
        }
    }
}

OLED *OLED::clear() {
    this->u8g2.clearBuffer();

    return this;
}

OLED *OLED::setFont(const uint8_t *font) {
    this->u8g2.setFont(font ? font : defaultFont);

    return this;
}

OLED *OLED::drawStr(int x, int y, const char *str) {
    this->u8g2.drawStr(x, y, str);

    return this;
}

OLED *OLED::drawStrf(int x, int y, const char *fmt, ...) {
    char buf[64] = {0, };

    va_list arg_ptr;
    va_start(arg_ptr, fmt);
    vsprintf(buf, fmt, arg_ptr);
    va_end(arg_ptr);

    this->u8g2.drawStr(x, y, buf);

    return this;
}

OLED *OLED::drawBig(const char *str) {
    return setFont(this -> bigFont)
    -> drawStr(0, 0, str)
    -> setFont(this -> defaultFont);
}

OLED *OLED::drawUTF8(int x, int y, const char *str) {
    this->u8g2.drawUTF8(x, y, str);

    return this;
}

OLED *OLED::lockFor(unsigned long duration) {
    currentLockPostedTime = millis();
    currentLockDuration = duration;

    isLocked = true;

    return this;
}

OLED *OLED::forceUnlock() {
    isLocked = false;
}

OLED *OLED::commit() {
    if (isLocked) {
        return this;
    }

    this->u8g2.sendBuffer();

    return this;
}

void OLED::showMovingTitle(const char *title) {
    setFont(bigFont)->clear();

    int steps = getUTF8Width(title) - 128;

    for (int i = 0; i < steps; ++i) {
        clear()
        -> drawStr(-i, 0, title)
        -> commit();
    }

    setFont(defaultFont);
}

u8g2_uint_t OLED::getUTF8Width(const char *text) {
    return this->u8g2.getUTF8Width(text);
}
