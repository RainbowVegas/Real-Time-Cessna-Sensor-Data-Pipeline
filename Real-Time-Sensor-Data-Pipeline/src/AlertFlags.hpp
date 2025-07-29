#pragma once

//Struct for alert flags
struct AlertFlags{
    // Temperature
    bool belowOperatingTemp = false;
    bool aboveOperatingTemp = false;

    // Altitude
    bool aboveAltitudeCeiling = false;

    // Speed (only if off ground)
    bool stallSpeed = false;
    bool overMaxSpeed = false;
    bool overSafeVerticalSpeed = false;

    // Engine
    bool lowEngineRPM = false;
    bool highEngineRPM = false;

    // Oil
    bool highOilTemp = false;
    bool lowOilPressure = false;
    bool highOilPressure = false;

    // Fuel
    bool lowFuel = false;
    bool lowFuelFlow = false;
    bool highFuelFlow = false;

    // Orientation
    bool pitchExceeded = false;
    bool pitchRateExceeded = false;
    bool rollExceeded = false;
    bool rollRateExceeded = false;
    bool yawRateExceeded = false;
};
