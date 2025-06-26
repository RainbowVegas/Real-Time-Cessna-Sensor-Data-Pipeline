#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <cmath>


// Utility function to get the current time in ISO 8601 format (UTC)
std::string get_iso8601_utc() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm utc_tm = *std::gmtime(&now_c);

    std::ostringstream oss;
    oss << std::put_time(&utc_tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

/*----------------------------------------------------------------------------------------
Title: Logger::log
Description:
Log function to log sensor data to a CSV file.
@param data SensorData object containing the sensor readings
This function appends the sensor data to a file named "sensor_log.csv" in CSV format.
It includes the current timestamp in ISO 8601 format, and checks for specific thresholds
for temperature, altitude, speed, vertical speed, engine RPM, fuel flow, pitch, and roll.
If any of these thresholds are exceeded, an alert is added to the log entry.
----------------------------------------------------------------------------------------*/
void Logger::log(const SensorData& data) {
    // Open the file to log the data
    std::ofstream output("sensor_log.csv", std::ios::out | std::ios::app);
    // IF file didn't open successfully, handle error
    if (!output) {
        std::cerr << "Error opening file for logging." << std::endl;
        return;
    }

    //Create output file with sensor data in CSV format
    output << get_iso8601_utc()                  // Get current time in ISO 8601 format
               << std::fixed << std::setprecision(2) // Set fixed-point notation to nearest 2nd dec 80.12
               << "," << data.temperature
               << "," << data.altitude               
               << "," << data.speed
               << "," << data.verticalSpeed
               << "," << data.engineRPM
               << "," << data.fuelFlow
               << "," << data.pitch
               << "," << data.roll;             

    // IF temperature is less than 80.0 C or greater than 35.0 C, add alert
    if (data.temperature < -31.0 || data.temperature > 40.0) {
        output << ",TEMP";
    }
    // IF altitude is greater than 13500.0 ft, add alert
    if (data.altitude > 13500.0) {
        output << ",ALTITUDE";
    }
    // IF speed is less than 47.0 knots or greater than 163.0 knots, add alert
    if (data.speed < 47.0 || data.speed > 163.0) {
        output << ",SPEED";
    }
    // IF vertical speed is greater than 721.0 ft/min or less than -721.0 ft/min, add alert
    if (std::abs(data.verticalSpeed) > 721.0) {
        output << ",VERTICALSPEED";
    }
    // IF engine RPM is greater than 2700.0 or less than 500.0, add alert
    if (data.engineRPM > 2700.0 || data.engineRPM < 500.0) {
        output << ",RPM";
    }
    // IF fuel flow is greater than 20.0 or less than 5.0, add alert
    if (data.fuelFlow > 20.0 || data.fuelFlow < 5.0) {
        output << ",FUELFLOW";
    }
    // IF pitch is greater than 30.0 or less than -30.0, add alert
    if (std::abs(data.pitch) > 30.0) {
        output << ",PITCH";
    }
    // IF roll is greater than 45.0 or less than -45.0, add alert
    if (std::abs(data.roll) > 45.0) {
        output << ",ROLL";
    }

    // Print a newline at the end of the log entry
    // std::endl flushes the output buffer making sure the log is displayed immediate
    output << std::endl;
}// End of Logger::log