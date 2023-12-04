#include "server.hpp"
int main() {
    DinoScale::DinoScale ds = DinoScale::DinoScale();
    ds.createRoute("GET", "/", "index.html");
    ds.createRoute("GET", "/hello", "hello.html");
    ds.createRoute("GET", "/night", "night.html");

    ds.startListening();
    return 0;
}