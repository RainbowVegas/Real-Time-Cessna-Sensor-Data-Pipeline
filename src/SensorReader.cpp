#include "SensorReader.hpp"
#include <random>
#include <thread>
#include <chrono>
#include <iostream>

// SensorReader class constructor
// Initializes the SensorReader with a Logger instance.
SensorReader::SensorReader(Logger& logger, const std::string& mode, std::atomic<bool>& running) 
: logger(logger), mode(mode), running(running) {}

/*-------------------------------------------------------------------------------------
// Title: SensorReader::generateData
// Description:
// Generate random sensor data and push it to a queue for analysis for testing purposes.
// This function simulates the generation of sensor data for a Cessna 172 Skyhawk.
// It uses random number generation to create values for temperature, altitude, speed,
// vertical speed, engine RPM, fuel flow, pitch, and roll.
// The generated data is pushed to a queue, which can be processed by another thread.
-------------------------------------------------------------------------------------*/
void SensorReader::generateData() {
    (void)mode; //Implement tomorrow

    // Create a random number generator and distributions for sensor data
    // This will generate random values for temperature, altitude, speed, etc.
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> tempDist(-20.0, 50.0);
    std::uniform_real_distribution<> altDist(0.0, 15000.0);
    std::uniform_real_distribution<> speedDist(30.0, 180.0);
    std::uniform_real_distribution<> vsiDist(-1500.0, 1500.0);
    std::uniform_real_distribution<> rpmDist(400.0, 2800.0);
    std::uniform_real_distribution<> fuelDist(3.0, 25.0);
    std::uniform_real_distribution<> pitchDist(-45.0, 45.0);
    std::uniform_real_distribution<> rollDist(-60.0, 60.0);

    // Print the mode of operation
    std::cout << "[Main] Starting in mode: " << mode << std::endl;

    // Log data in an infinite loop
    while (running) {
        SensorData data = {
            tempDist(gen),
            altDist(gen),
            speedDist(gen),
            vsiDist(gen),
            rpmDist(gen),
            fuelDist(gen),
            pitchDist(gen),
            rollDist(gen)
        };
        // Push the generated data to the queue
        {
            // Lock the mutex to ensure thread-safe access to the queue
            std::lock_guard<std::mutex> lock(mtx);
            // Push data to queue
            dataQueue.push(data);
        }
        // Notify the analyzer thread that new data is available
        // This will wake up the analyzer thread if it is waiting
        cv.notify_one();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}// End of generateData

/*-------------------------------------------------------------------------------------
// Title: SensorReader::analyzeData
// Description:
// Analyze the sensor data by logging it to a file.
// This function continuously checks the queue for new sensor data,
// retrieves it, and logs it using the Logger class.
--------------------------------------------------------------------------------------*/
void SensorReader::analyzeData() {
    // Wait for data to be available in the queue or for the running flag to be false
    while (running || !dataQueue.empty()) {
        // Wait for data to be available in the queue
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !dataQueue.empty() || !running; });
        
        // If running is false and the queue is empty, exit the loop
        while (!dataQueue.empty()) {
            //Get data from queue
            SensorData data = dataQueue.front();
            //Pop data from queue
            dataQueue.pop();
            // Unlock mutex
            lock.unlock();  // Allow generateData to keep pushing while logging
            // Log the data
            logger.log(data);
            // Re-lock the mutex
            lock.lock();
        }
    }

    std::cout << "[SensorReader] Data analysis stopped.\n";
}