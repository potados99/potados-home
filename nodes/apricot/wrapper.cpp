#include "node.h"

Node apricot("apricot", D4);

callback onPower = [](int coap_method, const char *payload, char *reply) {
    if (reply == NULL) return;

    switch (coap_method) {

        case COAP_GET: {
            sprintf(reply, apricot.device() -> getPower() ? "ON" : "OFF");
            break;
        }

        case COAP_PUT: {
            if (payload == NULL) return;

            Serial.println("payload:");
            Serial.println(payload);

            String msg(payload);
            msg.toUpperCase();

            if (msg == "ON") {
                apricot.device()
                -> setPower(true);

                apricot.display()
                -> forceUnlock()
                -> clear()
                -> drawBig("ON")
                -> commit()
                -> lockFor(1000);

                sprintf(reply, "OK");
            }
            else if (msg == "OFF") {
                apricot.device()
                -> setPower(false);

                apricot.display()
                -> forceUnlock()
                -> clear()
                -> drawBig("OFF")
                -> commit()
                -> lockFor(1000);

                sprintf(reply, "OK");
            }
            else {
                sprintf(reply, "FAIL");

                apricot.display()
                -> forceUnlock()
                -> clear()
                -> drawBig("ERROR")
                -> commit()
                -> lockFor(1000);
            }

            break;
        }

    } /* end of switch */
};
callback onBrightness = [](int coap_method, const char *payload, char *reply) {
    if (reply == NULL) return;

    switch (coap_method) {

        case COAP_GET: {
            sprintf(reply, "%d", apricot.device() -> getPWM());
            break;
        }

        case COAP_PUT: {
            if (payload == NULL) return;

            Serial.println("payload:");
            Serial.println(payload);

            char *end = NULL;
            long pwm = strtol(payload, &end, 10);

            /**
             * All number, not empty, in range.
             */
            bool success = (*end == '\0' && end != payload && pwm >= 0 && pwm <= 100);

            if (success) {
                apricot.device()
                -> setPWM((uint8_t)pwm);

                apricot.display()
                -> forceUnlock()
                -> clear()
                -> drawBig(String(pwm).c_str())
                -> commit()
                -> lockFor(1000);

                sprintf(reply, "OK");
            }
            else {
                sprintf(reply, "FAIL");

                apricot.display()
                -> forceUnlock()
                -> clear()
                -> drawBig("ERROR")
                -> commit()
                -> lockFor(1000);
            }

            break;
        }

    } /* end of switch */
};

void setup() {
    apricot.setup();

    apricot.addResource("power", onPower);
    apricot.addResource("brightness", onBrightness);
}

void loop() {
    apricot.loop();
}
