#pragma once

// Struct to hold sensor data for Cessna 172 Skyhawk simulation
struct SensorData {
    double temperature;   // Temperature in Celsius
    double altitude;      // Altitude in feet
    double speed;         // Speed in knots
    double verticalSpeed; // Vertical speed in feet per minute
    double engineRPM;     // Engine RPM
    double fuelFlow;      // Fuel flow in gallons per hour
    double pitch;         // Pitch angle in degrees
    double roll;          // Roll angle in degrees
};
