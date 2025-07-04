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

## Current Development
- Real-time plotting using Dear ImGui and ImPlot
- Visual alerts for abnormal sensor behavior
- Toggleable dashboard views
- Refined threshold logic for anomaly detection

## Planned Features
- User-configurable thresholds via GUI
- Pause/resume streaming control
- Exportable graph snapshots
- Cloud syncing and telemetry upload
- Machine learning integration for early warning signals
- Aircraft profile selection support (e.g., Cessna, Boeing)

## How to Build
Ensure you have a C++17 compatible compiler and `mingw32-make` for Windows:
```
mingw32-make
```

## How to Run
```
./sensor_pipeline random        # For simulated data
./sensor_pipeline flightgear    # For live FlightGear data
```

## Example Output (CSV Format)
```
2025-07-04T13:47:37Z,55.63,945.49,118.21,37.69,2770.21,2770.21,60.59,180.75,...
```

## Directory Structure
```
├── src/
│   ├── main.cpp
│   ├── SensorReader.cpp
│   ├── Logger.cpp
│   ├── SensorReader.hpp
│   └── Logger.hpp
├── data/
│   └── sensor_log.csv
├── external/
│   ├── imgui/
│   └── implot/
└── README.md
```

## License
MIT License (include license file if applicable).

