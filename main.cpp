#include "server.hpp"

int main() {
    DinoScale::DinoScale ds = DinoScale::DinoScale("127.0.0.1", 6969);
    ds.startListening();
    return 0;
}