#pragma once

#include "AlertFlags.hpp"
#include "SensorData.hpp"
#include <GLFW/glfw3.h>
#include "imgui.h"
#include <vector>
#include <mutex>

// utility structure for realtime plot
struct ScrollingBuffer {
    int MaxSize;
    int Offset;
    ImVector<ImVec2> Data;
    ScrollingBuffer(int max_size = 2000) {
        MaxSize = max_size;
        Offset  = 0;
        Data.reserve(MaxSize);
    }
    void AddPoint(float x, float y) {
        if (Data.size() >= MaxSize){
            Data.erase(Data.begin());  // Remove oldest point
        }
        Data.push_back(ImVec2(x, y));  // Add newest point
    }
    void Erase() {
        if (Data.size() > 0) {
            Data.shrink(0);
            Offset  = 0;
        }
    }
};

class GUI {
public:
    bool init();                     // Set up ImGui, window, etc.
    void render();                   // Called every frame
    void shutdown();                 // Cleanup

    void setLatestData(const SensorData& data, const AlertFlags& alerts){// Pass in for live view
        std::lock_guard<std::mutex> latestDataLock(dataMutex);
        latestData = data;
        latestAlerts = alerts;
    }  

    GLFWwindow* window;

private:
    // Time config
    double startTime = 0.0;
    static constexpr float HISTORY = 10.0f;

    // Thead-safe shared data
    std::mutex dataMutex;
    SensorData latestData;
    AlertFlags latestAlerts;

    // Buffers for graphs 
    ScrollingBuffer altitudeBuff;
    ScrollingBuffer speedBuff;
    ScrollingBuffer vertSpeedBuff;
    ScrollingBuffer engineRPMBuff;
    ScrollingBuffer oilPressureBuff;
    ScrollingBuffer fuelFlowBuff; 

    // Graphing funcs
    void drawAltitudeGraph(double currTime);
    void drawSpeedGraph(double currTime);
    void drawVertSpeedGraph(double currTime);
    void drawEngineRPMGraph(double currTime);
    void drawOilPresGraph(double currTime);
    void drawFuelFlowGraph(double currTime);

    // Gui helper funcs
    void drawTextData();
    void drawAnomilies();
    void DrawAlertLine(const char* label, bool triggered); 
};