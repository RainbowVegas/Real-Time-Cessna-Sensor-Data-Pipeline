#pragma once

#include "AlertFlags.hpp"
#include "SensorData.hpp"
#include <string>
#include <fstream>

// Logger class to handle logging of sensor data
class Logger {
    public:
        Logger();                  //Constructor 
        ~Logger();                 //Deconstructor

         // Function to log sensor data
        AlertFlags logSensorData(const SensorData& data);
        
    private:
        std::ofstream output;      // Output file stream for logging
        
        // Helper: convert AlertFlags to CSV alert string
        std::string alertFlagsToString(const AlertFlags& flags);
};