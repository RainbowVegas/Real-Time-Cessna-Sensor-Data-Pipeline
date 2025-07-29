#include "Logger.hpp"
#include "AlertManager.hpp"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <cmath>

// Constructor
Logger::Logger() {
    std::cout << "[Logger] Initializing...\n";

    output.open("data/sensor_log.csv", std::ios::out); // Clear file on startup
    if (!output.is_open()) {
        std::cerr << "[Logger] Error when trying to open file" << std::endl;
    } else {
        std::cout << "[Logger] File is opened.\n";
        output << "Timestamp,Temperature,Altitude,Speed,Vertical Speed,Engine RPMs,Throttle,"
               << "Oil Pressure,Oil Temperature,Fuel Capacity,Fuel Flow,Pitch,Pitch Rate,Roll,"
               << "Roll Rate,Yaw,Yaw Rate,Alerts"
               << std::endl;
    }
}

// Deconstructor
Logger::~Logger(){
    if (output.is_open()){
        output.close();
        std::cout << "File closed.\n";
    }
}

// Utility function to get the current time in ISO 8601 format (UTC)
std::string get_iso8601_utc() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm utc_tm = *std::gmtime(&now_c);

    std::ostringstream oss;
    oss << std::put_time(&utc_tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

std::string Logger::alertFlagsToString(const AlertFlags& flags){
    std::ostringstream oss;

    // Convert all flags to alert strings
    if (flags.belowOperatingTemp)   oss << ",BELOW OP TEMP";
    if (flags.aboveOperatingTemp)   oss << ",ABOVE OP TEMP";
    if (flags.aboveAltitudeCeiling) oss << ",ABOVE ALT CEILING";
    if (flags.stallSpeed) oss << ",STALL SPEED";
    if (flags.overMaxSpeed) oss << ",OVER MAXSPEED";
    if (flags.overSafeVerticalSpeed) oss << ",OVER SAFE VERTSPEED";
    if (flags.lowEngineRPM) oss << ",LOW ENGINE RPM";
    if (flags.highEngineRPM) oss << ",HIGH ENGINE RPM";
    if (flags.highOilTemp) oss << ",HIGH OIL TEMP";
    if (flags.lowOilPressure) oss << ",LOW OIL PRESSURE";
    if (flags.highOilPressure) oss << ",HIGH OIL PRESSURE";
    if (flags.lowFuel) oss << ",LOW FUEL";
    if (flags.lowFuelFlow) oss << ",LOW FUEL FLOW";
    if (flags.highFuelFlow) oss << ",HIGH FUEL FLOW";
    if (flags.pitchExceeded) oss << ",PITCH";
    if (flags.pitchRateExceeded) oss << ",PITCH RATE";
    if (flags.rollExceeded) oss << ",ROLL";
    if (flags.rollRateExceeded) oss << ",ROLL RATE";
    if (flags.yawRateExceeded) oss << ",YAW RATE";
    
    //Return string
    return oss.str();
}

/*----------------------------------------------------------------------------------------
Title: Logger::log
Description:
Log function to log sensor data to a CSV file.
@param data SensorData object containing the sensor readings
This function appends the sensor data to a file named "sensor_log.csv" in CSV format.
It includes the current timestamp in ISO 8601 format, and checks for specific thresholds
for temperature, altitude, speed, vertical speed, engine RPM, oil temperature, 
oil pressure, fuel capacity, fuel flow, pitch, and roll.
If any of these thresholds are exceeded, an alert is added to the log entry.
----------------------------------------------------------------------------------------*/
AlertFlags Logger::logSensorData(const SensorData& data) {
    if (!output.is_open()) return AlertFlags{};
    //std::cout << "[Logger] Logging data: Temp=" << data.temperature << ", Alt=" << data.altitude << std::endl;

    //Create output file with sensor data in CSV format
    output << get_iso8601_utc()                      // Get current time in ISO 8601 format
           << std::fixed << std::setprecision(2)     // Set fixed-point notation to nearest 2nd dec 80.12
           << "," << data.temperature
           << "," << data.altitude               
           << "," << data.speed
           << "," << data.verticalSpeed
           << "," << data.engineRPM << "," << data.throttle
           << "," << data.oilPressure << "," << data.oilTemperature
           << "," << data.fuelCap << "," << data.fuelFlow
           << "," << data.pitch << "," << data.pitchRate
           << "," << data.roll << "," << data.rollRate
           << "," << data.yaw << "," << data.yawRate;             

    AlertFlags alerts = AlertManager::evaluate(data);

    // Print a newline at the end of the log entry
    // std::endl flushes the output buffer making sure the log is displayed immediate
    output << alertFlagsToString(alerts) << std::endl;
    output.flush();

    // Return alerts
    return alerts;
}// End of Logger::log