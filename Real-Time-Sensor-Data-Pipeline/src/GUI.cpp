// ImGui/ImPlot includes for GUI rendering
#include "implot.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Custom data types and states
#include "SensorData.hpp"
#include "AlertFlags.hpp"

#include <glad/glad.h> // GLAD loader for OpenGL
#include "GUI.hpp"
#include <iostream>    //for std::cerr

// Init window dimensions
const GLuint WIDTH = 1280, HEIGHT = 720;
// Tracks if window is open 
bool opened;

bool GUI::init() {
    // Init GLFW library
    if (!glfwInit()){
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    // Set OpenGL version to 3.3 core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Create GLFW window
    window = glfwCreateWindow(WIDTH, HEIGHT, "Sensor Data", NULL, NULL);
    if (window == NULL){
        std::cerr << "Could not create window.\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Load OpenGl functions using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }

    opened = true;
    
    // Init ImGui/ImPlot
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    //Init start time for real time plotting
    startTime = ImGui::GetTime();

    //Bind ImGui/ImPlot to OpenGL
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Set default UI theme
    ImGui::StyleColorsDark();

    return true;
}

void GUI::render() {
    glfwPollEvents(); // Process OS window events

    // Start new ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Get the actual frame buffer size
    int display_w = WIDTH, display_h = HEIGHT;
    glfwGetFramebufferSize(window, &display_w, &display_h);

    // Full-screen seamless ImGui window
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)display_w, (float)display_h));

    // UI logic for main dash window
    if (ImGui::Begin("Sensor Dashboard", &opened,  
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus)){

        double currTime = ImGui::GetTime() - startTime;

        // Safely update scrolling buffers with latest data
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            altitudeBuff.AddPoint((float)currTime, (float)latestData.altitude);
            speedBuff.AddPoint((float)currTime, (float)latestData.speed);
            vertSpeedBuff.AddPoint((float)currTime, (float)latestData.verticalSpeed);
            engineRPMBuff.AddPoint((float)currTime, (float)latestData.engineRPM);
            oilPressureBuff.AddPoint((float)currTime, (float)latestData.oilPressure);
            fuelFlowBuff.AddPoint((float)currTime, (float)latestData.fuelFlow);
        }

        // Left column: flight performance graphs
        ImGui::BeginChild("LeftGraphs", ImVec2(375, 675), true);
        drawAltitudeGraph(currTime);
        drawSpeedGraph(currTime);
        drawVertSpeedGraph(currTime);
        ImGui::EndChild();

        ImGui::SameLine();
        
        // Middle column: engine/fuel graphs
        ImGui::BeginChild("RightGraphs", ImVec2(375, 675), true);
        drawEngineRPMGraph(currTime);
        drawOilPresGraph(currTime);
        drawFuelFlowGraph(currTime);
        ImGui::EndChild();
        
        ImGui::SameLine(0.0f, 20.0f); // spacing

        // Right panel: live numeric data
        ImGui::BeginGroup();
        drawTextData();
        ImGui::EndGroup();

        ImGui::SameLine(0.0f, 20.0f); // spacing

        // Right most panel: alert statuses 
        ImGui::BeginGroup();
        drawAnomilies();
        ImGui::EndGroup();
    }

    ImGui::End();
    ImGui::PopStyleVar(3);
    
    // Render final GUI output to framebuffer
    ImGui::Render();
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap buffs
    glfwSwapBuffers(window);
}

void GUI::shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

// Graphing Funcs
void GUI::drawAltitudeGraph(double currTime){
    if (!altitudeBuff.Data.empty() && ImPlot::BeginPlot("Altitude Plot", ImVec2(-1, 200))) {
            ImPlot::SetupAxisLimits(ImAxis_X1, currTime - HISTORY, currTime, ImGuiCond_Always);

            // Compute max altitude dynamically
            float maxAltitude = 0.0f;
            {
                std::lock_guard<std::mutex> lock(dataMutex);
                for (auto& p : altitudeBuff.Data)
                    if (p.y > maxAltitude)
                        maxAltitude = p.y;
            }
            maxAltitude = std::max(maxAltitude, static_cast<float>(latestData.altitude));

            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, maxAltitude, ImGuiCond_Always);

            ImPlot::PlotLineG("Altitude",
                [](int idx, void* data) -> ImPlotPoint {
                    const ImVector<ImVec2>* vec = static_cast<const ImVector<ImVec2>*>(data);
                    const ImVec2& pt = (*vec)[idx];
                    return ImPlotPoint(pt.x, pt.y);
                },
                &altitudeBuff.Data, altitudeBuff.Data.size());

            ImPlot::EndPlot();
        }
        ImGui::Text("Altitude: %.2f ft", latestData.altitude);
        return;
}

void GUI::drawSpeedGraph(double currTime){
    if (!speedBuff.Data.empty() && ImPlot::BeginPlot("Speed Plot", ImVec2(-1, 200))) {
            ImPlot::SetupAxisLimits(ImAxis_X1, currTime - HISTORY, currTime, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 230, ImGuiCond_Always);

            ImPlot::PlotLineG("Speed",
                [](int idx, void* data) -> ImPlotPoint {
                    const ImVector<ImVec2>* vec = static_cast<const ImVector<ImVec2>*>(data);
                    const ImVec2& pt = (*vec)[idx];
                    return ImPlotPoint(pt.x, pt.y);
                },
                &speedBuff.Data, speedBuff.Data.size());

            ImPlot::EndPlot();
        }
        ImGui::Text("Speed: %.2f kt", latestData.speed);
        return;
}

void GUI::drawVertSpeedGraph(double currTime){
    if (!vertSpeedBuff.Data.empty() && ImPlot::BeginPlot("Vert Speed Plot", ImVec2(-1, 200))) {
            ImPlot::SetupAxisLimits(ImAxis_X1, currTime - HISTORY, currTime, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, -5000, 5000, ImGuiCond_Always);

            ImPlot::PlotLineG("Vert Speed",
                [](int idx, void* data) -> ImPlotPoint {
                    const ImVector<ImVec2>* vec = static_cast<const ImVector<ImVec2>*>(data);
                    const ImVec2& pt = (*vec)[idx];
                    return ImPlotPoint(pt.x, pt.y);
                },
                &vertSpeedBuff.Data, vertSpeedBuff.Data.size());
            
            ImPlot::EndPlot();
        }
        ImGui::Text("Vertical Speed: %.2f ft", latestData.verticalSpeed);
        return;
}

void GUI::drawEngineRPMGraph(double currTime){
    if (!engineRPMBuff.Data.empty() && ImPlot::BeginPlot("Engine RPM Plot", ImVec2(-1, 200))) {
            ImPlot::SetupAxisLimits(ImAxis_X1, currTime - HISTORY, currTime, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 3600, ImGuiCond_Always);

            ImPlot::PlotLineG("Engine RPM",
                [](int idx, void* data) -> ImPlotPoint {
                    const ImVector<ImVec2>* vec = static_cast<const ImVector<ImVec2>*>(data);
                    const ImVec2& pt = (*vec)[idx];
                    return ImPlotPoint(pt.x, pt.y);
                },
                &engineRPMBuff.Data, engineRPMBuff.Data.size());
            
            ImPlot::EndPlot();
        }
        ImGui::Text("Engine RPM: %.2f rpm", latestData.engineRPM);
        return;
}

void GUI::drawOilPresGraph(double currTime){
    if (!oilPressureBuff.Data.empty() && ImPlot::BeginPlot("Oil Pressure Plot", ImVec2(-1, 200))) {
            ImPlot::SetupAxisLimits(ImAxis_X1, currTime - HISTORY, currTime, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 150, ImGuiCond_Always);

            ImPlot::PlotLineG("Oil Pressure",
                [](int idx, void* data) -> ImPlotPoint {
                    const ImVector<ImVec2>* vec = static_cast<const ImVector<ImVec2>*>(data);
                    const ImVec2& pt = (*vec)[idx];
                    return ImPlotPoint(pt.x, pt.y);
                },
                &oilPressureBuff.Data, oilPressureBuff.Data.size());
            
            ImPlot::EndPlot();
        }
        ImGui::Text("Oil Pressure: %.2f psi", latestData.oilPressure);
        return;
}

void GUI::drawFuelFlowGraph(double currTime){
    if (!fuelFlowBuff.Data.empty() && ImPlot::BeginPlot("Fuel Flow Plot", ImVec2(-1, 200))) {
            ImPlot::SetupAxisLimits(ImAxis_X1, currTime - HISTORY, currTime, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 30, ImGuiCond_Always);

            ImPlot::PlotLineG("Fuel Flow",
                [](int idx, void* data) -> ImPlotPoint {
                    const ImVector<ImVec2>* vec = static_cast<const ImVector<ImVec2>*>(data);
                    const ImVec2& pt = (*vec)[idx];
                    return ImPlotPoint(pt.x, pt.y);
                },
                &fuelFlowBuff.Data, fuelFlowBuff.Data.size());
            
            ImPlot::EndPlot();
        }
        ImGui::Text("Fuel Flow: %.2f gph", latestData.fuelFlow);
        return;
}

// Display numeric sensor data on screen
void GUI::drawTextData(){
    ImGui::Text("Temperature: %.2f F", latestData.temperature);
    ImGui::Text("Oil Temperature: %.2f F", latestData.oilTemperature);
    ImGui::Spacing();
    ImGui::Text("Throttle: %.2f %%", latestData.throttle);
    ImGui::Text("Total Fuel: %.2f gal", latestData.fuelCap);
    ImGui::Spacing();
    ImGui::Text("Pitch: %.2f °", latestData.pitch);
    ImGui::Text("Pitch Rate: %.2f °/s", latestData.pitchRate);
    ImGui::Spacing();
    ImGui::Text("Roll: %.2f °", latestData.roll);
    ImGui::Text("Roll Rate: %.2f °/s", latestData.rollRate);
    ImGui::Spacing();
    ImGui::Text("Yaw: %.2f °", latestData.yaw);
    ImGui::Text("Yaw Rate: %.2f °/s", latestData.yawRate);
    return;
} 

// Render all current anomaly alerts with color-coded status
void GUI::drawAnomilies() {
    // Air Temp
    DrawAlertLine("Below Op Temp", latestAlerts.belowOperatingTemp);
    DrawAlertLine("Above Op Temp", latestAlerts.aboveOperatingTemp);

    ImGui::Spacing();

    // Altitude
    DrawAlertLine("Above Altitude Ceiling", latestAlerts.aboveAltitudeCeiling);

    ImGui::Spacing();

    // Speed
    DrawAlertLine("Stall Speed", latestAlerts.stallSpeed);
    DrawAlertLine("Over Max Speed", latestAlerts.overMaxSpeed);
    DrawAlertLine("Over Safe Vert Speed", latestAlerts.overSafeVerticalSpeed);

    ImGui::Spacing();

    // Engine
    DrawAlertLine("Low Engine RPM", latestAlerts.lowEngineRPM);
    DrawAlertLine("High Engine RPM", latestAlerts.highEngineRPM);

    ImGui::Spacing();

    // Oil
    DrawAlertLine("High Oil Temperature", latestAlerts.highOilTemp);
    DrawAlertLine("Low Oil Pressure", latestAlerts.lowOilPressure);
    DrawAlertLine("High Oil Pressure", latestAlerts.highOilPressure);

    ImGui::Spacing();

    // Fuel
    DrawAlertLine("Low Fuel", latestAlerts.lowFuel);
    DrawAlertLine("Low Fuel Flow", latestAlerts.lowFuelFlow);
    DrawAlertLine("High Fuel Flow", latestAlerts.highFuelFlow);

    ImGui::Spacing();

    // Orientation
    DrawAlertLine("Pitch Exceeded", latestAlerts.pitchExceeded);
    DrawAlertLine("Pitch Rate Exceeded", latestAlerts.pitchRateExceeded);
    DrawAlertLine("Roll Exceeded", latestAlerts.rollExceeded);
    DrawAlertLine("Roll Rate Exceeded", latestAlerts.rollRateExceeded);
    DrawAlertLine("Yaw Rate Exceeded", latestAlerts.yawRateExceeded);
}

// Helper to render a single alert line with color depending on status
void GUI::DrawAlertLine(const char* label, bool triggered) {
    ImU32 color = triggered ? IM_COL32(255, 0, 0, 255) : IM_COL32(255, 255, 255, 255);
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::Text("%s: %s", label, triggered ? "True" : "False");
    ImGui::PopStyleColor();
}