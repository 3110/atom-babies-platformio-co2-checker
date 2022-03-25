#pragma once

#include <SensirionI2CScd4x.h>

#include "AtomBabies.h"
#include "Debug.h"

namespace M5Stack_AtomBabies {

typedef struct {
    bool ready;
    uint16_t co2;
    float temperature;
    float humidity;
} SensorData;

class CO2Checker {
public:
    static const int I2C_SCL_PIN = 22;
    static const int I2C_SDA_PIN = 19;
    static const uint16_t DEFAULT_FORCED_TARGET_CO2_CONCENTRATION = 450;
    static const uint16_t DEFAULT_WARNING_CO2_CONCENTRATION = 700;
    static const uint16_t DEFAULT_CAUTION_CO2_CONCENTRATION = 1000;
    static const uint16_t FORCED_RECALIBRATION_INTERVAL_SEC = 180;  // 3min
    static const uint16_t SCROLL_INTERVAL_MS = 300;

    CO2Checker(void);
    virtual ~CO2Checker(void);

    virtual bool begin(TwoWire& wire = Wire1);
    virtual bool update(AtomBabies& babies);

    virtual bool startPeriodicMeasurement(bool isLowPower = true);
    virtual bool stopPeriodicMeasurement(void);
    virtual bool readMeasurement(SensorData& data);

    virtual bool isReady(void);

    virtual void displayNormalFace(AtomBabies& babies);
    virtual void displayWarningFace(AtomBabies& babies);
    virtual void displayCautionFace(AtomBabies& babies);

    virtual bool doFactoryReset(void);
    virtual bool doForcedRecalibration(
        uint16_t target = DEFAULT_FORCED_TARGET_CO2_CONCENTRATION);

protected:
    virtual bool showSerialNumber(void);
    virtual void showSensorData(const SensorData& data) const;
    virtual void showErrorMessage(const char* msg, uint16_t error);

private:
    static const size_t ERROR_MESSAGE_SIZE = 256;

    SensirionI2CScd4x _scd4x;
    SensorData _data;
};

}  // namespace M5Stack_AtomBabies
