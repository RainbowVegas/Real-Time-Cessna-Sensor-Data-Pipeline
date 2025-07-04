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
for temperature, altitude, speed, vertical speed, engine RPM, oil temperature, 
oil pressure, fuel capacity, fuel flow, pitch, and roll.
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
    std::cout << "[Logger] Logging data: Temp=" << data.temperature << ", Alt=" << data.altitude << std::endl;
    //Create output file with sensor data in CSV format
    output << get_iso8601_utc()                  // Get current time in ISO 8601 format
               << std::fixed << std::setprecision(2) // Set fixed-point notation to nearest 2nd dec 80.12
               << "," << data.temperature
               << "," << data.altitude               
               << "," << data.speed
               << "," << data.verticalSpeed
               << "," << data.engineRPM
               << "," << data.throttle
               << "," << data.oilPressure
               << "," << data.oilTemperature
               << "," << data.fuelCap
               << "," << data.fuelFlow
               << "," << data.pitch
               << "," << data.pitchRate
               << "," << data.roll
               << "," << data.rollRate
               << "," << data.yaw
               << "," << data.yawRate;             

    // IF temperature is less than -23.8 C or greater than 104.0 C, add alert
    if (data.temperature < -23.8 || data.temperature > 104.0) {
        output << ",TEMP";
    }
    // IF altitude is greater than 13500.0 ft, add alert
    if (data.altitude > 13500.0) {
        output << ",ALTITUDE";
    }
    // IF speed is less than 47.0 knots or greater than 163.0 knots, add alert
    if (data.speed < 47.0 || data.speed > 158.0) {
        output << ",SPEED";
    }
    // IF vertical speed is greater than 721.0 ft/min or less than -721.0 ft/min, add alert
    if (std::abs(data.verticalSpeed) > 721.0 || data.verticalSpeed < -721.0) {
        output << ",VERTICALSPEED";
    }
    // IF engine RPM is greater than 2700.0 or less than 500.0, add alert
    if (data.engineRPM > 2700.0 || data.engineRPM < 500.0) {
        output << ",RPM";
    }
    // IF oil temperature is greater then 245.0 F, add alert
    if (data.oilTemperature > 245.0){
        output << ",OILTEMP";
    }
    // IF oil pressure is less the 25.0 psi or greater than 115.0 psi, add alert
    if (data.oilPressure < 25.0 || data.oilPressure > 115.0){
        output << ",OILPRESSURE";
    }
    // IF fuel capacity is less than 6.0, add alert
    if (data.fuelCap < 6.0){
        output << ",LOWFUEL";
    }
    // IF fuel flow is greater than 20.0 or less than 5.0, add alert
    if (data.fuelFlow > 20.0 || data.fuelFlow < 5.0) {
        output << ",FUELFLOW";
    }
    // IF pitch is greater than 30.0 or less than -30.0, add alert
    if (std::abs(data.pitch) > 30.0 || std::abs(data.pitch) < -30.0) {
        output << ",PITCH";
    }
    // IF pitch rate is greater than 15.0 or less than -15.0, add alert
    if (std::abs(data.pitchRate) > 15.0 || std::abs(data.pitchRate) < -15.0) {
        output << ",PITCHRATE";
    }
    // IF roll is greater than 45.0 or less than -45.0, add alert
    if (std::abs(data.roll) > 45.0 || std::abs(data.roll) < -45.0) {
        output << ",ROLL";
    }
    // IF roll rate is greater than 25.0 or less than -25.0, add alert
    if (std::abs(data.rollRate) > 25.0 || std::abs(data.rollRate) < -25.0) {
        output << ",ROLLRATE";
    }
    // IF yaw rate is greater than 20.0 or less than -20.0, add alert
    if (std::abs(data.yawRate) > 20.0 || std::abs(data.yawRate) < -20.0) {
        output << ",YAWRATE";
    }

    // Print a newline at the end of the log entry
    // std::endl flushes the output buffer making sure the log is displayed immediate
    output << std::endl;
}// End of Logger::log