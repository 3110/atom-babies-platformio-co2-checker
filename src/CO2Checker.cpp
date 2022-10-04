#include "CO2Checker.h"

namespace M5Stack_AtomBabies {

const char* CO2Checker::JSON_KEY_CO2 = "co2";

CO2Checker::CO2Checker(void) : _scd4x(), _data{false, 0, 0.0f, 0.0f} {
}

CO2Checker::~CO2Checker(void) {
}

bool CO2Checker::begin(TwoWire& wire) {
    wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    this->_scd4x.begin(wire);
    delay(1000);
    bool result = stopPeriodicMeasurement();
    result |= showSerialNumber();
    result |= startPeriodicMeasurement();
#ifdef ENABLE_ESPNOW
    if (this->_espnow.begin()) {
        result |= this->_espnow.registerPeer(EspNowManager::BROADCAST_ADDRESS);
    } else {
        result = false;
    }
#endif
    return result;
}

bool CO2Checker::update(AtomBabies& babies) {
    if (babies.wasPressed()) {
#ifdef ENABLE_ESPNOW
        send();
#endif
        bool blinking = babies.isBlinking();
        if (blinking) {
            babies.stopBlink();
        }
        delay(1000);
        babies.scrollDigits(CRGB::Green, this->_data.co2, SCROLL_INTERVAL_MS);
        if (blinking) {
            babies.display();
            babies.startBlink();
        }
    }
    if (!isReady()) {
        return false;
    }
    if (readMeasurement(this->_data)) {
        showSensorData(this->_data);
#ifdef ENABLE_ESPNOW
        send();
#endif
        babies.clear(true);
        if (this->_data.co2 >= DEFAULT_CAUTION_CO2_CONCENTRATION) {
            displayCautionFace(babies);
        } else if (this->_data.co2 >= DEFAULT_WARNING_CO2_CONCENTRATION) {
            displayWarningFace(babies);
        } else {
            displayNormalFace(babies);
        }
        return true;
    } else {
        return false;
    }
}

bool CO2Checker::startPeriodicMeasurement(bool isLowPower) {
    uint16_t error = isLowPower
                         ? this->_scd4x.startLowPowerPeriodicMeasurement()
                         : this->_scd4x.startPeriodicMeasurement();
    if (error) {
        if (isLowPower) {
            showErrorMessage("startPeriodicMeasurement(isLowPower = true)",
                             error);
        } else {
            showErrorMessage("startPeriodicMeasurement(isLowPower = false)",
                             error);
        }
    }
    return error == NoError;
}

bool CO2Checker::stopPeriodicMeasurement(void) {
    uint16_t error = this->_scd4x.stopPeriodicMeasurement();
    if (error) {
        showErrorMessage("stopPeriodicMeasurement()", error);
    }
    return error == NoError;
}

bool CO2Checker::readMeasurement(SensorData& data) {
    uint16_t error =
        this->_scd4x.readMeasurement(data.co2, data.temperature, data.humidity);
    if (error) {
        showErrorMessage("readMeasurement()", error);
        data.ready = false;
    } else if (data.co2 == 0) {
        SERIAL_PRINTLN("Invalid sample detected, skipping.");
        data.ready = false;
    } else {
        data.ready = true;
    }
    return error == NoError;
}

bool CO2Checker::isReady(void) {
    bool ready = false;
    uint16_t error = this->_scd4x.getDataReadyFlag(ready);
    if (error) {
        showErrorMessage("getDataReadyFlag()", error);
        return false;
    } else {
        return ready;
    }
}

void CO2Checker::displayNormalFace(AtomBabies& babies) {
    babies.setEyesColor(AtomBabies::DEFAULT_EYE_COLOR)
        .setFace(FaceNormal)
        .display();
}

void CO2Checker::displayWarningFace(AtomBabies& babies) {
    babies.setEyesColor(CRGB::Blue).setFace(FaceDown).display();
}

void CO2Checker::displayCautionFace(AtomBabies& babies) {
    babies.setEyesColor(CRGB::Red).setFace(FaceDown).display();
}

bool CO2Checker::doFactoryReset(void) {
    uint16_t error = this->_scd4x.performFactoryReset();
    if (error) {
        showErrorMessage("performFactoryReset()", error);
    }
    return error == NoError;
}

bool CO2Checker::doForcedRecalibration(uint16_t target) {
    uint16_t frcCorrection;
    SERIAL_PRINTLN("# Factory Reset");
    if (!doFactoryReset()) {
        return false;
    }
    SERIAL_PRINTLN("# CO2 values before recalibration");
    if (!startPeriodicMeasurement()) {
        return false;
    }
    delay(1000);
    for (size_t c = 0; c < FORCED_RECALIBRATION_INTERVAL_SEC / 30;
         ++c) {  // 3min
        while (!isReady()) {
            delay(100);
        }
        readMeasurement(this->_data);
        showSensorData(this->_data);
    }
    if (!stopPeriodicMeasurement()) {
        return false;
    }
    delay(500);
    uint16_t error =
        this->_scd4x.performForcedRecalibration(target, frcCorrection);
    if (error) {
        showErrorMessage("performForcedRecalibration()", error);
    }
    SERIAL_PRINTF_LN("# Forced Recalibration is finished: Correction = %d",
                     frcCorrection);
    return error == NoError;
}

#ifdef ENABLE_ESPNOW
bool CO2Checker::send(void) {
    JSONVar o;
    o[JSON_KEY_CO2] = this->_data.co2;
    String s = JSON.stringify(o);
    return this->_espnow.send(EspNowManager::BROADCAST_ADDRESS,
                              reinterpret_cast<const uint8_t*>(s.c_str()),
                              s.length());
}
#endif

bool CO2Checker::showSerialNumber(void) {
    uint16_t s[3] = {0};
    uint16_t error = this->_scd4x.getSerialNumber(s[0], s[1], s[2]);
    if (error) {
        showErrorMessage("getSerialNumber()", error);
    } else {
        SERIAL_PRINTF_LN("Serial: 0x%04X%04X%04X", s[0], s[1], s[2]);
    }
    return error == NoError;
}

void CO2Checker::showSensorData(const SensorData& data) const {
    SERIAL_PRINTF_LN("CO2: %4d\tTemperature: %4.1f\tHumidity: %4.1f",
                     this->_data.co2, this->_data.temperature,
                     this->_data.humidity);
}

void CO2Checker::showErrorMessage(const char* msg, uint16_t error) {
#if defined(DEBUG)
    char errorMessage[ERROR_MESSAGE_SIZE] = {0};
    errorToString(error, errorMessage, ERROR_MESSAGE_SIZE);
    SERIAL_PRINTF_LN("%s: %s", msg, errorMessage);
#endif
}

}  // namespace M5Stack_AtomBabies
