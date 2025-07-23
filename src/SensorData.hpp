#pragma once

// Struct to hold sensor data for Cessna 172 Skyhawk simulation
struct SensorData {
    double temperature;    // Temperature in Farenheit
    double altitude;       // Altitude in feet
    double agl;            // AGL in feet
    double speed;          // Speed in knots
    double verticalSpeed;  // Vertical speed in feet per minute
    double engineRPM;      // Engine RPM
    double throttle;       // Throttle position (0-100%)
    double oilPressure;    // Oil pressure in psi
    double oilTemperature; // Oil temperature in Farenheit
    double fuelCap;        // Fuel capacity in gallons
    double fuelFlow;       // Fuel flow in gallons per hour
    double pitch;          // Pitch angle in degrees
    double pitchRate;      // Pitch angle rate in degrees per second
    double roll;           // Roll angle in degrees
    double rollRate;       // Roll angle rate in degrees per second
    double yaw;            // Yaw angle in degrees (optional, not used in the original code)
    double yawRate;        // Yaw angle rate in degrees per second (optional, not used in the original code)
};