#include "AlertManager.hpp"
#include "cmath"
#include <iostream>

// Method check data against threshold limit of the Cesna
// and raise flags on bad checks
AlertFlags AlertManager::evaluate(const SensorData& data){
    AlertFlags flags{};
    basicEval(data, flags);
    trendEval(data, flags);
    return flags;
}

using namespace Thresholds;
void AlertManager::basicEval(const SensorData& data, AlertFlags& flags){
    // IF temperature is less than -23.8 C or greater than 104.0 C, flag
    if (data.temperature < OP_TEMP_LOW) flags.belowOperatingTemp = true;
    if (data.temperature > OP_TEMP_HIGH) flags.aboveOperatingTemp = true;

    // IF altitude is greater than 13500.0 ft, flag
    if (data.altitude > FLIGHT_CEIL) flags.aboveAltitudeCeiling = true;

    // IF off the ground
    if (data.agl > 5.0){
        // IF speed is less than 47.0 knots or greater than 163.0 knots, add alert
        if (data.speed <= SPEED_LOW) flags.stallSpeed = true;
        if (data.speed > SPEED_HIGH) flags.overMaxSpeed = true;

        // IF vertical speed is greater than 721.0 ft/min or less than -721.0 ft/min, add alert
        if (std::abs(data.verticalSpeed) > HIGH_VERTICAL_SPEED) flags.overSafeVerticalSpeed = true;

        // IF engine RPM is greater than 2700.0 or less than 500.0, add alert
        if (data.engineRPM < RPM_LOW) flags.lowEngineRPM = true;
        if (data.engineRPM > RPM_HIGH) flags.highEngineRPM = true;

        // IF oil temperature is greater then 245.0 F, add alert
        if (data.oilTemperature > OIL_TEMP_HIGH) flags.highOilTemp = true;

        // IF oil pressure is less the 25.0 psi or greater than 115.0 psi, add alert
        if (data.oilPressure < OIL_PRESSURE_LOW) flags.lowOilPressure = true;
        if (data.oilPressure > OIL_PRESSURE_HIGH) flags.highOilPressure = true;

        // IF oil temp is high and pressure is low add alert
        if (data.oilPressure < OIL_PRESSURE_LOW && data.oilTemperature > OIL_TEMP_HIGH) 
        flags.engineFailure = true;

        // IF fuel capacity is less than 6.0, add alert
        if (data.fuelCap < FUEL_CAP_LOW) flags.lowFuel = true;

        // IF fuel flow is greater than 20.0 or less than 5.0, add alert
        if (data.fuelFlow < FUEL_FLOW_LOW) flags.lowFuelFlow = true;
        if (data.fuelFlow > FUEL_FLOW_HIGH) flags.highFuelFlow = true;

        // IF abs of pitch is greater than 30.0, add alert
        if (abs(data.pitch) > PITCH_MAX) flags.pitchExceeded = true;
        // IF abs of pitch rate is greater than 15.0, add alert
        if (abs(data.pitchRate) > PITCH_RATE_MAX) flags.pitchRateExceeded = true;

        // IF abs of roll is greater than 45.0, add alert
        if (abs(data.roll) > ROLL_MAX) flags.rollExceeded = true;
        // IF abs of roll rate is greater than 25.0, add alert
        if (abs(data.rollRate) > ROLL_RATE_MAX) flags.rollRateExceeded = true;

        // IF abs of yaw rate is greater than 20.0, add alert
        if (abs(data.yawRate) > YAW_RATE_MAX) flags.yawRateExceeded = true;
    }
}

void AlertManager::trendEval(const SensorData& newestData, AlertFlags& flags){
    // IF history is full  
    if (history.isFull()) {
        auto oldestOpt = history.peakTail();
        if (!oldestOpt) {
            return; // no valid oldest data, bail out
        }
        const SensorData& oldestData = *oldestOpt;

        // Get differences
        double speedDelta    = newestData.speed    - oldestData.speed;
        double altitudeDelta = newestData.altitude - oldestData.altitude;
        double pitchDelta    = newestData.pitch    - oldestData.pitch;
        double rollDelta     = newestData.roll     - oldestData.roll;
        double yawDelta      = newestData.yawRate  - oldestData.yawRate;

        if (speedDelta > SAFE_SPEED_INC)  flags.rapidSpeedIncrease = true;
        if (speedDelta < SAFE_SPEED_DEC)  flags.rapidSpeedDecrease = true;

        if (newestData.engineRPM > RPM_HIGH && newestData.engineRPM >= oldestData.engineRPM)
            flags.longTermHighEngineRPM = true;

        if (altitudeDelta <= -DANGEROUS_ALT_DROP) flags.extremeAltitudeDrop = true;

        if (std::abs(pitchDelta) > EXTREME_PITCH_CHANGE ||
            std::abs(rollDelta)  > EXTREME_ROLL_CHANGE  ||
            std::abs(yawDelta)   > EXTREME_YAW_CHANGE) {
            flags.extremeManeuvers = true;
        }
    }   
    // Add data to history
    history.write(newestData);
}