#if __WIN32

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#elif __linux__

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// TODO : remove this and rather change the variable to be different based on
// type of operating system on which the code is getting compiled.
#define SOCKET int

#endif

#ifndef DINOSCALE_H_
#define DINOSCALE_H_

#include "constants/methods.hpp"
#include "constants/statuses.hpp"
#include "logger/Logger.hpp"

#endif

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace DinoScale {
class DinoScale {
   private:
    const int        maxSimultaneousConnections = 20;
    static const int maxBufferSize = 30720;
    Logger           logger;

    SOCKET sock;
    SOCKET newSocket;

    socklen_t          socketAddressLength;
    struct sockaddr_in socketAddress;

    const std::string machineIpAddress;
    std::string       serverMessage;

    std::unordered_map<HTTPMethod, std::string> routeToFileMapList[5];

    /* function to start a server */
    int startServer() {
        int err;

#if _WIN32
        /* in case of windows, we need to initialize the sockets before
        working with the sockets hence requesting socket version of 2.2 */

        WSADATA wsaData;
        WORD    windowsVersionRequested = MAKEWORD(2, 2);

        err = WSAStartup(windowsVersionRequested, &wsaData) != 0;
        if (err != 0) {
            exitWithError("WSAStartup failed");
            return 1;
        }
#endif

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            exitWithError("invalid socket");
            return 1;
        }

        err = bind(sock, (sockaddr*)&socketAddress, socketAddressLength);
        if (err < 0) {
            exitWithError("cannot connect socket to address");
            return 1;
        }
        return 0;
    }

    void closeServer() {
#ifdef _WIN32
        closesocket(sock);
        closesocket(newSocket);
        WSACleanup();
#endif
    }

    void acceptConnection() {
        newSocket =
            accept(sock, (sockaddr*)&socketAddress, &socketAddressLength);

        if (newSocket < 0) {
            std::ostringstream oss;
            oss << "Server failed to accept incoming connection from ADDRESS: "
                << inet_ntoa(socketAddress.sin_addr)
                << "; PORT: " << ntohs(socketAddress.sin_port);
            exitWithError(oss.str());
        }
    }

    void parseHttpRequest(const char* buffer, std::string& method,
                          std::string& route, std::string& headers,
                          std::string& body) {
        std::string request(buffer);
        std::size_t pos = request.find("\r\n\r\n");

        if (pos == std::string::npos) {
            exitWithError("invalid request: no end of headers found");
        }

        headers = request.substr(0, pos);
        body = request.substr(pos + 4);  // Skip the "\r\n\r\n" sequence

        std::string line = headers.substr(0, request.find_first_of('\r'));
        std::istringstream iss(line);
        std::string        value;

        int count = 0;

        while (std::getline(iss, value, ' ')) {
            logger.Log("value : " + value);

            if (count == 0) {
                // for the first iteration, the output is the http method
                method = value;
            } else if (count == 1) {
                // for the second method the output is route
                route = value;
            } else {
                // currently have no idea what is the use of the other
                // parameters of header
                break;
            }
            count++;
        }
    }
    void sendResponse() {
        int  bytesSent;
        long totalBytesSent = 0;

        while (totalBytesSent < serverMessage.size()) {
            bytesSent =
                send(newSocket, serverMessage.c_str(), serverMessage.size(), 0);
            if (bytesSent < 0) {
                break;
            }
            totalBytesSent += bytesSent;
        }

        if (totalBytesSent == serverMessage.size()) {
            logger.Log("------ Server Response sent to client ------");
        } else {
            logger.Log("Error sending response to client.");
        }
    }

    void prepareResponse(HTTPMethod method, std::string route,
                         std::string body) {
        if (method == HTTPMethod::GET) {
            std::string        htmlFile;
            std::ostringstream oss;
            std::stringstream  strStream;
            std::ifstream      requestedFile;
            std::string        fileName = "error.html";

            oss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";

            int verbPosition = method;

            std::unordered_map<std::string, std::string>& routeToFileMap =
                routeToFileMapList[verbPosition];

            for (auto routeToFile : routeToFileMap)
                std::cout << routeToFile.first << " " << routeToFile.second
                          << std::endl;

            logger.Log("route is " + route + ".");
            if (routeToFileMap.find(route) != routeToFileMap.end()) {
                fileName = routeToFileMap.at(route);
            }
            requestedFile.open(fileName, std::ios::in);

            if (requestedFile
                    .is_open()) {  // always check whether the file is open
                strStream << requestedFile.rdbuf();  // read the file
            }
            htmlFile = strStream.str();
            // log(htmlFile);
            oss << htmlFile.size() << "\n\n" << htmlFile;

            serverMessage = oss.str();
            // htmlFile.clear();
        } else if (method == HTTPMethod::POST) {
        } else if (method == HTTPMethod::PUT) {
        } else if (method == HTTPMethod::DELETE) {
        }
    }

    void exitWithError(std::string errorMessage) {
#ifdef _WIN32
        log(WSAGetLastError());
#endif

        logger.Log(errorMessage, LogLevel::Error);
        exit(1);
    }

   public:
    DinoScale(std::string machineIpAddress = "127.0.0.1", u_short port = 6969)
        : machineIpAddress(machineIpAddress) {
        // TODO :: Fix how to dereference shared_ptr in cpp
        this->logger = Logger::GetInstance();

        socketAddress.sin_family = AF_INET;
        socketAddress.sin_port = htons(port);
        socketAddress.sin_addr.s_addr =
            inet_addr(this->machineIpAddress.c_str());
        socketAddressLength = sizeof(socketAddress);

        // routeToFileMapList = new std::unordered_map<std::string,
        // std::string>[httpVerbToNumberMapping.size()];

        int serverStartingError = startServer();
        if (serverStartingError != 0) {
            std::ostringstream oss;
            oss << "Failed to start server with PORT: "
                << ntohs(socketAddress.sin_port);
            logger.Log(oss.str());
        };
    }
    void createRoute(HTTPMethod method, std::string route, std::string path) {
        // todo : need to cause an error if unknown http verb is used
        // if (httpVerbToNumberMapping.find(method) ==
        // httpVerbToNumberMapping.end()) {
        //     log("here");
        //     exitWithError("http verb not defined");
        // }

        // log("create routes");
        // int verbPosition = httpVerbToNumberMapping.at(method);
        // if (routeToFileMapList[verbPosition].find(route) !=
        //     routeToFileMapList[verbPosition].end()) {
        //     exitWithError("route " + route + " is already defined");
        // } else {
        //     log("route " + route + " added");
        //     routeToFileMapList[verbPosition].insert({route, path});
        // }
    }

    void startListening() {
        std::ostringstream oss;

        if (listen(this->sock, this->maxSimultaneousConnections) < 0) {
            exitWithError("Socket listen failed");
        }

        oss << "\n*** Listening on ADDRESS: "
            << inet_ntoa(socketAddress.sin_addr)
            << " PORT: " << ntohs(socketAddress.sin_port) << " ***\n\n";

        std::string listeningString = oss.str();
        logger.Log(listeningString);

        int bytesRecieved;
        while (true) {
            logger.Log("----- Waiting for a new connection -----");
            // waits until a request comes to the server
            acceptConnection();

            // recieving requests from client
            char buffer[DinoScale::maxBufferSize] = {0};
            int  bytesReceived = recv(newSocket, buffer, maxBufferSize, 0);
            if (bytesReceived < 0) {
                exitWithError(
                    "failed to receive bytes from client socket connection");
            }

            std::string method, route, headers, body;
            parseHttpRequest(buffer, method, route, headers, body);
            prepareResponse(method, route, body);

            std::ostringstream oss;
            oss << "------ Received Request from client ------";
            logger.Log(oss.str());

            sendResponse();
        }
    }

    ~DinoScale() { closeServer(); }
};
}  // namespace DinoScale
