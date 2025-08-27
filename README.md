# Real-Time Cessna Sensor Data Pipeline

A multithreaded, real-time sensor data simulation, ingestion, analysis, and logging system written in modern C++ for a Cessna 172 Skyhawk. It supports either simulated data (random generator) or live data streamed from FlightGear via UDP. The system includes CSV logging, basic anomaly detection, and a live Dear ImGui + ImPlot GUI with visual alerts.

## Key Features
- Modular, multithreaded architecture (producer/consumer with condition variables)
- Live FlightGear UDP intake (Winsock) or simulated generator
- Custom parser for FlightGear generic protocol (prefix-based tokens)
- CSV logging with timestamps + alert flags
- Basic anomaly detection (trend-based checks over ~10s windows and real-time checks)
- Thread-safe queue and condition variable system
- Real-time plotting using Dear ImGui, ImPlot, and OpenGL (via GLAD)
- Visual alerts for abnormal sensor behavior

## Dependencies
- C++17
- Winsock2 (Windows only)
- FlightGear (for live mode)
- Dear ImGui & ImPlot (for upcoming GUI support)
- GLFW (for the window)
- GLAD (OpenGL function loader)

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
