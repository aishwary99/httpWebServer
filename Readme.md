#Bro HTTP Server
Bro HTTP Server is a lightweight C++ HTTP server designed for serving static resources and handling dynamic content through user-defined URL mappings. It is built to handle basic HTTP requests and responses.

###Features
####Static Resource Serving: Serve static resources such as HTML, CSS, and JavaScript files from a specified folder.

####Dynamic URL Mapping: Define dynamic content by mapping URLs to user-defined functions that handle specific HTTP methods (e.g., GET).

####Platform Support: The server code includes platform-specific handling for both Windows and Linux.

###Getting Started
####Prerequisites
C++ Compiler: Ensure that you have a C++ compiler installed on your machine. For Linux, you can use g++, and for Windows, consider using a compiler like MinGW.

####Installation
Clone the Repository: Clone this repository to your local machine.


##Additional Notes -

```
Order of Server Side Sockets -

1. Socket Creation
2. Binding the Socket
3. Listening on the specified port
4. Accepting connections
```