#include "SensorReader.hpp"
#include <random>
#include <thread>
#include <chrono>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <condition_variable> 

// SensorReader class constructor
SensorReader::SensorReader(Logger& logger, const std::string& mode, std::atomic<bool>& running) 
: logger(logger), mode(mode), running(running) {}

/*-------------------------------------------------------------------------------------
// Title: SensorReader::generateData
// Description:
// Generate sensor data based on the specified mode.
// This function checks the mode of operation and calls the appropriate data generation method.
-------------------------------------------------------------------------------------*/
void SensorReader::generateData() {
    std::cout << "[SensorReader] Starting data gen in mode: " << mode << std::endl;
    // If the mode is "random", it generates random sensor data.
    if(mode == "random"){
        generateRandomData();
    }
    // If the mode is "flightgear", it generates data from FlightGear.
    else if(mode == "flightgear"){
        generateFGData();
    }
    // If the mode is neither "random" nor "flightgear", it prints an error message.
    else {
        std::cerr << "[SensorReader] Unknown mode: " << mode << "Please use 'random' or 'flightgear'.\n";
        return;
    }
}// End of generateData

/*-------------------------------------------------------------------------------------
// Title: SensorReader::generateRandomData
// Description:
// Generate random sensor data and push it to a queue for analysis for testing purposes.
// This function simulates the generation of sensor data for a Cessna 172 Skyhawk.
// It uses random number generation to create values for temperature, altitude, speed,
// vertical speed, engine RPM, fuel flow, pitch, and roll.
// The generated data is pushed to a queue, which can be processed by another thread.
-------------------------------------------------------------------------------------*/
void SensorReader::generateRandomData() {
    // Create a random number generator and distributions for sensor data
    // This will generate random values for temperature, altitude, speed, etc.
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> tempDist(-20.0, 50.0);
    std::uniform_real_distribution<> altDist(0.0, 15000.0);
    std::uniform_real_distribution<> aglDist(5.0, 6.0);
    std::uniform_real_distribution<> speedDist(30.0, 180.0);
    std::uniform_real_distribution<> vsiDist(-1500.0, 1500.0);
    std::uniform_real_distribution<> rpmDist(400.0, 2800.0);
    std::uniform_real_distribution<> throttleDist(0.0, 100.0);
    std::uniform_real_distribution<> oilPressDist(15.0, 130.0);
    std::uniform_real_distribution<> oilTempDist(0.0, 300.0);
    std::uniform_real_distribution<> fuelCapDist(0.0, 43.0);
    std::uniform_real_distribution<> fuelFlowDist(3.0, 25.0);
    std::uniform_real_distribution<> pitchDist(-45.0, 45.0);
    std::uniform_real_distribution<> pitchRateDist(-30.0, 30.0);
    std::uniform_real_distribution<> rollDist(-60.0, 60.0);
    std::uniform_real_distribution<> rollRateDist(-30.0, 30.0);
    std::uniform_real_distribution<> yawDist(-180.0, 180.0);
    std::uniform_real_distribution<> yawRateDist(-30.0, 30.0);

    // Log data in an infinite loop
    while (running) {
        SensorData data = {
            tempDist(gen),
            altDist(gen), aglDist(gen),
            speedDist(gen), vsiDist(gen),
            rpmDist(gen), throttleDist(gen),
            oilPressDist(gen), oilTempDist(gen),
            fuelCapDist(gen), fuelFlowDist(gen),
            pitchDist(gen), pitchRateDist(gen),
            rollDist(gen), rollRateDist(gen),
            yawDist(gen), yawRateDist(gen)
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
// Title: SensorReader::generateFGData
// Description:
// Generate sensor data from FlightGear and push it to a queue for analysis.
// This function reads sensor data from a FlightGear thru a UDP port and pushes 
// it to a queue. It uses a simple parser to extract the sensor data.
-------------------------------------------------------------------------------------*/
void SensorReader::generateFGData() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return;
    }

    // Create a UDP socket
    SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return;
    }

    // Bind the socket to port 5500
    sockaddr_in recvAddr{};
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(5500); // Must match FlightGear output port
    recvAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the address and port
    // This allows the socket to listen for incoming UDP packets on port 5500
    // If the bind operation fails, print an error message and clean up
    if (bind(udpSocket, (SOCKADDR*)&recvAddr, sizeof(recvAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(udpSocket);
        WSACleanup();
        return;
    }

    std::cout << "Listening for UDP data on port 5500..." << std::endl;

    // Receive and process incoming UDP packets
    char buffer[1024];
    sockaddr_in senderAddr;
    int senderAddrSize = sizeof(senderAddr);
    
    while(running){
        // Receive data from the socket    
        int bytesReceived = recvfrom(udpSocket, buffer, sizeof(buffer) - 1, 0,
                                     (SOCKADDR*)&senderAddr, &senderAddrSize);
                                     
        //std::cout << "[DEBUG] Received " << bytesReceived << " bytes from FlightGear\n";

        // IF the receive operation wasn't successful
        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "[SensorReader] recvform failed!" << std::endl;
            break;
        }
        
        // Null-terminate the received data to make it a valid string
        buffer[bytesReceived] = '\0';
        std::string line(buffer);
        //std::cout << "[DEBUG] Received: " << line << std::endl;

        // Parse the received data line
        SensorData data{};
        if(parseFGData(line, data)) {
            std::lock_guard<std::mutex> lock(mtx);
            dataQueue.push(data);
            cv.notify_one();
            
            //std::cout << "[gFGData] Data Queue Size: " << dataQueue.size() << "\n";
        }
    }

    // Close the socket and clean up Winsock
    closesocket(udpSocket);
    WSACleanup();
    std::cout << "[SensorReader] Stopped receiving FlightGear data.\n";
}// End of generateFGData

/*-------------------------------------------------------------------------------------
// Title: SensorReader::parseFGData
// Description:
// Parse a line of FlightGear data and extract sensor values.
// This function takes a line of data from FlightGear, splits it by tabs, and extracts
// the sensor values into a SensorData structure. It returns true if parsing is successful,
// or false if the line is malformed.
-------------------------------------------------------------------------------------*/
bool SensorReader::parseFGData(const std::string& line, SensorData& data) {
    bool parsed = false;             // Initialize parsed flag to false
    std::istringstream stream(line); // Create a string stream from the line
    std::string token;               // Variable to hold each token

    // While there are tokens in the stream
    while (stream >> token) {
        // Check if the token starts with a known prefix and parse accordingly
        
        if (token.rfind("TF=", 0) == 0) { // Temperature in Fahrenheit
            data.temperature = std::stof(token.substr(3));
            parsed = true;
        } 
        else if (token.rfind("A=", 0) == 0) { // Altitude in feet
            data.altitude = std::stof(token.substr(2));
            parsed = true;
        } 
        else if (token.rfind("AGL=",0) == 0){
            data.agl = std::stof(token.substr(4));
            parsed = true;
        }
        else if (token.rfind("V=", 0) == 0) { // Speed in knots
            data.speed = std::stof(token.substr(2));
            parsed = true;
        } 
        else if (token.rfind("VSF=", 0) == 0) { // Vertical speed in feet per second
            // Convert from feet per second to feet per minute
            data.verticalSpeed = std::stof(token.substr(4)) * 60; 
            parsed = true;
        } 
        else if (token.rfind("ER=", 0) == 0) { // Engine RPM
            data.engineRPM = std::stof(token.substr(3));
            parsed = true;
        } 
        else if (token.rfind("T=", 0) == 0) { // Throttle position (0-100%)
            data.throttle = std::stof(token.substr(2));
            parsed = true;
        } 
        else if (token.rfind("OP=", 0) == 0) { // Oil pressure in psi
            data.oilPressure = std::stof(token.substr(3));
            parsed = true;
        } 
        else if (token.rfind("OT=", 0) == 0) { // Oil temperature in Fahrenheit
            data.oilTemperature = std::stof(token.substr(3));
            parsed = true;
        } 
        else if (token.rfind("FCG=", 0) == 0) { // Fuel capacity in gallons
            data.fuelCap = std::stof(token.substr(4));
            parsed = true;
        } 
        else if (token.rfind("FFG=", 0) == 0) { // Fuel flow in gallons per hour
            data.fuelFlow = std::stof(token.substr(4));
            parsed = true;
        } 
        else if (token.rfind("PA=", 0) == 0) { // Pitch angle in degrees
            data.pitch = std::stof(token.substr(3));
            parsed = true;
        } 
        else if (token.rfind("PR=", 0) == 0) { // Pitch rate in degrees per second
            data.pitchRate = std::stof(token.substr(3));
            parsed = true;
        } 
        else if (token.rfind("RA=", 0) == 0) { // Roll angle in degrees
            data.roll = std::stof(token.substr(3));
            parsed = true;
        } 
        else if (token.rfind("RR=", 0) == 0) { // Roll rate in degrees per second
            data.rollRate = std::stof(token.substr(3));
            parsed = true;
        } 
        else if (token.rfind("YA=", 0) == 0) { // Yaw angle in degrees
            data.yaw = std::stof(token.substr(3));
            parsed = true;
        } 
        else if (token.rfind("YR=", 0) == 0) { // Yaw rate in degrees per second
            data.yawRate = std::stof(token.substr(3));
            parsed = true;
        }
    }
    // If any of the expected fields were parsed, return true
    return parsed;
}// End of parseFGData

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
            //std::cout << "[analyzeData] Data Queue Size: " << dataQueue.size() << "\n";
            // Unlock mutex
            lock.unlock();  // Allow generateData to keep pushing while logging
            // Log the data
            AlertFlags alerts = logger.logSensorData(data);
            // Log to GUI
            {
                std::lock_guard<std::mutex> latestDataLock(dataMutex);
                latestData = data;
                latestAlerts = alerts;
            }
            // Re-lock the mutex
            lock.lock();
        }
    }

    std::cout << "[SensorReader] Data analysis stopped.\n";
}// End of analyzeDatap