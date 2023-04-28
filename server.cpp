#include "server.hpp"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

DinoScale::DinoScale::DinoScale(
    const std::string machineIpAddress,
    const u_short port = 3000)
    : machineIpAddress(machineIpAddress) {
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(port);
    socketAddress.sin_addr.s_addr = inet_addr(this->machineIpAddress.c_str());
    socketAddressLength = sizeof(socketAddress);

    int serverStartingError = startServer();
    if (serverStartingError != 0) {
        std::ostringstream oss;
        oss << "Failed to start server with PORT: " << ntohs(socketAddress.sin_port);
        log(oss.str());
    }
}

int DinoScale::DinoScale::startServer() {
    // requesting socket version of 2.2
    int err;
    WSADATA wsaData;
    WORD windowsVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(windowsVersionRequested, &wsaData) != 0;
    if (err != 0) {
        exitWithError("WSAStartup failed");
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        exitWithError("invalid socket");
        return 1;
    }

    err = bind(sock, (sockaddr *)&socketAddress, socketAddressLength);
    if (err < 0) {
        exitWithError("cannot connect socket to address");
        return 1;
    }
    return 0;
}

void DinoScale::DinoScale::startListening() {
    std::ostringstream oss;

    if (listen(this->sock, this->maxSimultaneousConnections) < 0) {
        exitWithError("Socket listen failed");
    }

    oss << "\n*** Listening on ADDRESS: " << inet_ntoa(socketAddress.sin_addr)
        << " PORT: " << ntohs(socketAddress.sin_port) << " ***\n\n";

    std::string listeningString = oss.str();
    log(listeningString);

    int bytesRecieved;
    while (true) {
        log("----- Waiting for a new connection -----\n");
        // waits until a request comes to the server
        acceptConnection();

        // recieving requests from client
        char buffer[maxBufferSize] = {0};
        int bytesReceived = recv(newSocket, buffer, maxBufferSize, 0);
        if (bytesReceived < 0) {
            exitWithError("failed to receive bytes from client socket connection");
        }

        oss.str("");
        oss.clear();
        oss << "------ Received Request from client ------\n";
        log(oss.str());

        sendResponse();
    }
}

void DinoScale::DinoScale::acceptConnection() {
    newSocket = accept(sock, (sockaddr *)&socketAddress, &socketAddressLength);
    if (newSocket < 0) {
        std::ostringstream oss;
        oss << "Server failed to accept incoming connection from ADDRESS: "
            << inet_ntoa(socketAddress.sin_addr) << "; PORT: "
            << ntohs(socketAddress.sin_port);
        exitWithError(oss.str());
    }
}

void DinoScale::DinoScale::sendResponse() {
    int bytesSent;
    long totalBytesSent = 0;
    std::ostringstream oss;

    std::string htmlFile = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
    oss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: "
        << htmlFile.size() << "\n\n"
        << htmlFile;

    serverMessage = oss.str();

    while (totalBytesSent < serverMessage.size()) {
        bytesSent = send(newSocket, serverMessage.c_str(), serverMessage.size(), 0);
        if (bytesSent < 0) {
            break;
        }
        totalBytesSent += bytesSent;
    }

    if (totalBytesSent == serverMessage.size()) {
        log("------ Server Response sent to client ------\n");
    } else {
        log("Error sending response to client.");
    }
}

void DinoScale::DinoScale::closeServer() {
    closesocket(sock);
    closesocket(newSocket);
    WSACleanup();
}

DinoScale::DinoScale::~DinoScale() {
    closeServer();
}

template <typename T>
void DinoScale::DinoScale::log(T message) const {
    std::cout << message << std::endl;
}

void DinoScale::DinoScale::exitWithError(std::string errorMessage) {
    log(WSAGetLastError());
    log(errorMessage);
    exit(1);
}