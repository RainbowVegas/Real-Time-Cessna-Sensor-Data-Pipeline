#pragma once
#include "SensorData.hpp"
#include "AlertFlags.hpp"
#include "Logger.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <string>

class SensorReader {
public:
    SensorReader(Logger& logger,
                 const std::string& mode,
                 std::atomic<bool>& running);
    void generateData();
    void generateRandomData();
    void generateFGData();
    void analyzeData();

    SensorData getLatestData(){
        std::lock_guard<std::mutex> latestDataLock(dataMutex);
        return latestData;
    }
    AlertFlags getLatestAlerts(){
        std::lock_guard<std::mutex> latestDataLock(dataMutex);
        return latestAlerts;
    }

private:
    SensorData latestData;
    AlertFlags latestAlerts;

    bool parseFGData(const std::string& line, SensorData& data);
    std::queue<SensorData> dataQueue;
    std::mutex mtx;
    std::mutex dataMutex;
    
    std::condition_variable cv;
    
    Logger& logger;
    std::string mode;          // Mode of operation: "random" or "flightgear"
    std::atomic<bool>& running; // Control variable for thread execution
};