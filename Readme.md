# Bro HTTP Server
Bro HTTP Server is a lightweight C++ HTTP server designed for serving static resources and handling dynamic content through user-defined URL mappings. It is built to handle basic HTTP requests and responses.

### Features
####Static Resource Serving: Serve static resources such as HTML, CSS, and JavaScript files from a specified folder.

#### Dynamic URL Mapping: 
Define dynamic content by mapping URLs to user-defined functions that handle specific HTTP methods (e.g., GET).

#### Platform Support: 
The server code includes platform-specific handling for both Windows and Linux.

### Getting Started
#### Prerequisites
C++ Compiler: Ensure that you have a C++ compiler installed on your machine. For Linux, you can use g++, and for Windows, consider using a compiler like MinGW.

#### Installation
Clone the Repository: Clone this repository to your local machine.

```
git clone <repository-url>
g++ -o bro_server main.cpp
./bro_server
```
Access the Server: Open a web browser and navigate to http://localhost:6060 to access the default route.


# Configuration
#### Static Resources Folder: 
Set the folder path for serving static resources by calling the setStaticResourcesFolder method in the main function.

```
bro.setStaticResourcesFolder("your/static/resources/folder");
```

#### URL Mapping: 
Define dynamic content by using the get method in the main function. Provide a URL and a lambda function to handle the corresponding HTTP method.

```
bro.get("/", [](Request &request, Response &response) {
    // Your response handling logic here
});
```

# Examples
#### Serving Static Resources
The following example demonstrates how to serve static HTML content from the specified folder.

```
bro.setStaticResourcesFolder("static_html");
Dynamic Content Mapping
Map dynamic content to a specific URL and handle it with a lambda function.
```

```
bro.get("/api/data", [](Request &request, Response &response) {
    // Your dynamic content generation logic here
});
```

## Additional Notes -

```
Order of Server Side Sockets -
1. Socket Creation
2. Binding the Socket
3. Listening on the specified port
4. Accepting connections
```

## Upcoming Features -
1. Adding post type request/response parsing functionality.