#pragma once
#include "SensorData.hpp"

// Logger class to handle logging of sensor data
class Logger {
    public:
        void log(const SensorData& data);
};