// sensor_pipeline/src/main.cpp
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <cstdio>   // for printf
#pragma comment(lib, "ws2_32.lib")  // Link Winsock Library

// Main function to generate random sensor data and log it
// This simulates a Cessna 172 Skyhawk sensor data logging system
int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }

    // Create a UDP socket
    SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return 1;
    }

    // Bind the socket to port 5500
    sockaddr_in recvAddr{};
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(5500); // Must match FlightGear output port
    recvAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(udpSocket, (SOCKADDR*)&recvAddr, sizeof(recvAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(udpSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Listening for UDP data on port 5500..." << std::endl;

    // Receive and print incoming UDP packets
    char buf[512];
    sockaddr_in senderAddr;
    int senderAddrSize = sizeof(senderAddr);

    while (true) {
        int bytesReceived = recvfrom(udpSocket, buf, sizeof(buf), 0, (SOCKADDR*)&senderAddr, &senderAddrSize);
        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "recvfrom failed: " << WSAGetLastError() << std::endl;
            break;
        }

        std::cout << "Received " << bytesReceived << " bytes: ";
        // Print data in readable format
        for (int i = 0; i < bytesReceived; ++i) {
            if (isprint(static_cast<unsigned char>(buf[i]))) {
                std::cout << buf[i];
            } else {
                std::cout << ".";
            }
        }
        std::cout << std::endl;
    }

    closesocket(udpSocket);
    WSACleanup();
    return 0;
}