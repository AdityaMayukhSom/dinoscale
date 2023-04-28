#ifndef INCLUDE_DINOSCALE
#define INCLUDE_DINOSCALE

#include <winsock2.h>

#include <string>

namespace DinoScale {
class DinoScale {
   private:
    const int maxSimultaneousConnections = 20;
    const int maxBufferSize = 30720;

    SOCKET sock;
    SOCKET newSocket;

    int socketAddressLength;
    struct sockaddr_in socketAddress;
    std::string machineIpAddress;

    std::string serverMessage;

    int startServer();
    void closeServer();
    void acceptConnection();
    void sendResponse();

    template <typename T>
    void log(T) const;
    void exitWithError(std::string);

   public:
    DinoScale(std::string, u_short);
    void startListening();
    ~DinoScale();
};
}  // namespace DinoScale
#endif