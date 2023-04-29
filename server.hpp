#ifndef INCLUDE_DINOSCALE
#define INCLUDE_DINOSCALE

#include <winsock2.h>

#include <string>
#include <unordered_map>
namespace DinoScale {
class DinoScale {
   private:
    const int maxSimultaneousConnections = 20;
    const int maxBufferSize = 30720;
    static const std::unordered_map<std::string, byte> httpVerbToNumberMapping;

    SOCKET sock;
    SOCKET newSocket;

    int socketAddressLength;
    struct sockaddr_in socketAddress;
    std::string machineIpAddress;

    std::string serverMessage;

    std::unordered_map<std::string, std::string> routeToFileMapList[5];
    int startServer();
    void closeServer();
    void acceptConnection();
    void parseHttpRequest(const char*, std::string&, std::string&, std::string&, std::string&);
    void sendResponse();
    void prepareResponse(std::string, std::string, std::string);
    template <typename T>
    void log(T) const;
    void exitWithError(std::string);

   public:
    DinoScale(std::string = "127.0.0.1", u_short = 6969);
    void createRoute(std::string method, std::string route, std::string path);
    void startListening();
    ~DinoScale();
};
}  // namespace DinoScale
#endif