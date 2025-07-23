#pragma once

#include "SensorData.hpp"
#include "AlertFlags.hpp"

//Threshold for Cesna 172P SkyHawk
namespace Thresholds{
    static constexpr double OP_TEMP_LOW = -23.8;//Fahrenheit
    static constexpr double OP_TEMP_HIGH = 104.0;//Fahrenheit
    static constexpr double FLIGHT_CEIL = 13500.0;//Feet
    static constexpr double SPEED_LOW = 48.0;//Knots
    static constexpr double SPEED_HIGH = 158.0;//Knots
    static constexpr double HIGH_VERTICAL_SPEED = 800.0;//ft per minute
    static constexpr double RPM_LOW = 500.0;
    static constexpr double RPM_HIGH = 2700.0;
    static constexpr double OIL_TEMP_HIGH = 245.0;//Fahrenheit
    static constexpr double OIL_PRESSURE_LOW = 25.0;//Psi
    static constexpr double OIL_PRESSURE_HIGH = 115.0;//Psi
    static constexpr double FUEL_CAP_LOW = 6.0;//Gallons
    static constexpr double FUEL_FLOW_LOW = 5.0;
    static constexpr double FUEL_FLOW_HIGH = 20.0;
    static constexpr double PITCH_MAX = 30.0;
    static constexpr double PITCH_RATE_MAX = 15.0;
    static constexpr double ROLL_MAX = 45.0;
    static constexpr double ROLL_RATE_MAX = 25.0;
    static constexpr double YAW_RATE_MAX = 20.0;
};// Thresholds

class AlertManager{
public:
    // Evaluate sensor data against thresholds and return triggered alerts
    static AlertFlags evaluate(const SensorData& data);
};