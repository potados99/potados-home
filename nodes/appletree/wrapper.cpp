#include "node.h"

Node appleTree("appletree", D4);

callback onPower = [](int coap_method, const char *payload, char *reply) {
    if (reply == NULL) return;

    String msg(payload);
    msg.toUpperCase();

    switch (coap_method) {

        case COAP_GET: {
            sprintf(reply, appleTree.device() -> getPower() ? "ON" : "OFF");
            break;
        }

        case COAP_PUT: {
            if (payload == NULL) return;

            if (msg == "ON") {
                appleTree.device()
                -> setPower(true);

                appleTree.display()
                -> forceUnlock()
                -> clear()
                -> drawBig("ON")
                -> commit()
                -> lockFor(1000);

                sprintf(reply, "OK");
            }
            else if (msg == "OFF") {
                appleTree.device()
                -> setPower(false);

                appleTree.display()
                -> forceUnlock()
                -> clear()
                -> drawBig("OFF")
                -> commit()
                -> lockFor(1000);

                sprintf(reply, "OK");
            }
            else {
                sprintf(reply, "FAIL");

                appleTree.display()
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
    appleTree.addResource("power", onPower);
    appleTree.setup();
}

void loop() {
    appleTree.loop();
}
