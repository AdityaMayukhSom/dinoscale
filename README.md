# DinoScale

DinoScale is a lightweight C++ backend web library that allows you to quickly create and deploy web applications. It provides an easy-to-use API for creating and managing routes, handling HTTP requests and responses, and listening for incoming connections.

## Features

- Simple and easy-to-use API
- Lightweight and fast
- Support for GET, POST, PUT, DELETE HTTP methods
- Support for serving static files
- Easy to integrate with other C++ libraries

## Getting Started

To get started with DinoScale, you need to download and include "server.hpp" inside the file containing main function. Once you have included DinoScale, you can use the following code snippet to create a basic server:

```cpp
DinoScale::DinoScale ds = DinoScale::DinoScale();
ds.createRoute("GET", "/", "index.html");
ds.createRoute("GET", "/hello", "hello.html");
ds.createRoute("GET", "/night", "night.html");

ds.startListening();
```

This code creates a DinoScale object, adds three routes for the HTTP GET method, and starts listening for incoming connections on the default port (6969).

To compile the code into executable, use the following command (currently only windows is supported but linux will be supported soon)

```bash
g++ -o main.exe main.cpp server.cpp -lsw2_32
```

## Conclusion

DinoScale is a lightweight and easy-to-use C++ backend web library that allows you to quickly create and deploy web applications. It provides a simple and intuitive API for creating and managing routes, handling HTTP requests and responses, and serving static files.
