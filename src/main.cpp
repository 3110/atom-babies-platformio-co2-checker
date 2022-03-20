#include "AtomBabies.h"

using namespace M5Stack_AtomBabies;

AtomBabies babies;

void setup(void) {
    babies.begin();
    babies.setFace(FaceNormal);
    babies.setOrientation(OrientationUpsideDown);
}

void loop(void) {
    babies.update();
}