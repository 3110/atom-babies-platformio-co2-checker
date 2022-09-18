#include "AtomBabies.h"
#include "CO2Checker.h"

using namespace M5Stack_AtomBabies;

AtomBabies babies;
CO2Checker checker;

void setup(void) {
    babies.begin();
    checker.begin();
    M5.update();
    if (M5.Btn.isPressed()) {
        babies.fill(CRGB::Green);
        checker.doForcedRecalibration();
        babies.clear();
    }
    checker.startPeriodicMeasurement();

    babies.display();
    babies.startBlink();
    SERIAL_PRINTF_LN("Auto Orientation: %s",
                     babies.isAutoOrientation() ? "true" : "false");
}

void loop(void) {
    babies.update();
    checker.update(babies);
    delay(100);
}