#include "SensorReader.hpp"
#include "Logger.hpp"
#include "GUI.hpp"
#include <GLFW/glfw3.h>
#include <thread>
#include <windows.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <atomic>
#include <csignal>

// Global atomic variable to control the running state of the threads
std::atomic<bool> running(true);

// Signal handler to gracefully stop the threads
void signalHandler(int signum) {
    std::cout << "Interrupt signal (" << signum << ") received. Stopping threads..." << std::endl;
    running = false; // Set running to false to stop the threads
}

int main(int argc, char* argv[]){
    //Prevent sys sleep
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
    
    //Init gui
    GUI gui;
    if (!gui.init()) return -1;
    
    // Check if the correct number of arguments is provided
    if (argc < 2) {
        // Print usage instructions if no arguments are provided
        std::cerr << "Usage: " << argv[0] << " [random|flightgear]" << std::endl;
        return 1;
    }
    
    // Check the argument to determine the mode of operation
    std::string mode = argv[1];
    if (mode != "random" && mode != "flightgear") {
        // Print error message if not flightgear or random
        std::cerr << "Invalid mode. Use 'flightgear' or 'random'" << std::endl;
        return 1;
    }

    // Register signal handler for graceful shutdown
    std::signal(SIGINT, signalHandler);

    // Initialize the logger and sensor reader
    Logger logger;
    SensorReader reader(logger, mode, running); //Gets data from flightgear or random gen

    // Create threads for generating and analyzing sensor data
    std::thread sensorThread(&SensorReader::generateData, &reader);
    // Create a thread for analyzing sensor data
    std::thread analyzerThread(&SensorReader::analyzeData, &reader);

    // Render gui
    while(!glfwWindowShouldClose(gui.window)){
        gui.setLatestData(reader.getLatestData(), reader.getLatestAlerts());
        gui.render();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    sensorThread.join();
    analyzerThread.join();
    
    //Shutdown gui
    gui.shutdown();

    return 0;
}