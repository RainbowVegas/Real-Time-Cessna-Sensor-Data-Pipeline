# Real-Time Cessna Sensor Data Pipeline

A multithreaded, real-time sensor data simulation, ingestion, and logging system written in modern C++ for a Cessna 172 Skyhawk. This project supports both simulated (randomly generated) sensor data and live data streamed from FlightGear via UDP. Designed with scalability in mind, it includes built-in logging, basic anomaly detection, and is being expanded to support real-time data visualization through an interactive GUI.

## Key Features
- Modular architecture with multithreading support
- Real-time data acquisition from simulated values or FlightGear
- UDP socket communication (Windows Winsock-based)
- Custom parser for FlightGear's generic protocol
- CSV logging with timestamps
- Basic anomaly detection (e.g., RPM, pitch rate)
- Thread-safe queue and condition variable system
- Real-time plotting using Dear ImGui and ImPlot

## Planned Features
- Visual alerts for abnormal sensor behavior
- Refined threshold logic for anomaly detection - better but will continue to get worked on

## Dependencies
- C++17
- Winsock2 (Windows only)
- FlightGear (for live mode)
- Dear ImGui & ImPlot (for upcoming GUI support)

## How to Build
Ensure you have a C++17 compatible compiler and `mingw32-make` for Windows:
```
mingw32-make
```
You should also have FlightGear installed:
```
https://www.flightgear.org/download/
```
After it is installed your going to have to copy the sensor.xml file in the src folder to FlightGear's protocol folder:
```
C:\Users\user\FlightGear\Downloads\fgdata_2024_1\Protocol
```
When it's copied open FlightGear go to settings, additional settings, and add this line:
```
--generic=socket,out,10,127.0.0.1,5500,udp,sensor
```
This sets up the data that will be streamed from FlightGear to the port the sensor pipline will be listening to.

## How to Run
After running the make file run the .exe with random or flightgear:
```
./sensor_pipeline random        # For simulated data
./sensor_pipeline flightgear    # For live FlightGear data
```
The pipeline is now listening to the port, now run FlightGear. The Cessna is the default plane already installed on FlightGear so hit fly! 

## Example Output (CSV Format)
```
2025-07-04T13:47:37Z,55.63,945.49,118.21,37.69,2770.21,2770.21,60.59,180.75,...
```

## Directory Structure
```
├── src/
│   ├── pipe_main.cpp
|   ├── AlertManager.cpp
|   ├── GUI.cpp
│   ├── SensorReader.cpp
│   ├── Logger.cpp
|   ├── sensor.xml
|   ├── AlertFlags.hpp
|   ├── SensorData.hpp
|   ├── AlertManager.hpp
|   ├── GUI.hpp
│   ├── SensorReader.hpp
│   └── Logger.hpp
├── data/
│   └── sensor_log.csv
├── vendor/
│   ├── include/
│   ├── lib/
│   └── src/
├── imgui.ini
├── Makefile
├── sensor_pipeline.exe
└── README.md
```

## Help Resources

### FlightGear
- [Creating a Generic Protocol](https://wiki.flightgear.org/Howto:Create_a_generic_protocol#%3Coutput%3E)
- [Property Browser Reference](https://wiki.flightgear.org/Property_browser)

### Windows Networking (Winsock)
- [Getting Started with Winsock](https://learn.microsoft.com/en-us/windows/win32/winsock/getting-started-with-winsock)

### Cessna 172P Documentation
- [Pilot Operating Handbook (POH)](https://www.befa.org/wp-content/uploads/2019/12/POH-Cessna-172P.pdf)

## License
MIT License (include license file if applicable).

