#include <iostream>
#include <string.h>
#include <map>
#include <forward_list>
#include <unistd.h>

// windows libraries

#ifdef _WIN32
#include <windows.h>
#endif

// linux libraries

#ifdef linux
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

using namespace std;

// A utility class to do necessary validations
class Validator {
    private:
        Validator() {}
    public:
        static bool isValidMIMEType(string &mimeType) {
            return true;
        }
        static bool isValidPath(string &path) {
            return true;
        }
        static bool isValidURLFormat(string &url) {
            return true;
        }
};

// Utility class for String related operations
class StringUtility {
    private:
    StringUtility() {}

    public:
    static void toLowerCase(char *input) {
        if (input == NULL) return;
        for (; *input; input++) {
            if (*input >= 65 && *input <= 90) {
                *input += 32;
            }
        }
    }
};

class HttpErrorStatusUtility {
    private:
    HttpErrorStatusUtility() {}
    
    public:
    static void sendBadRequestError(int clientSocketDescriptor) {
        // will be done later
    }
    static void sendHttpVersionNotSupportedError(int clientSocketDescriptor, char *httpVersion) {
        // will be done later        
    }
    static void sendNotFoundError(int clientSocketDescriptor, char *requestURI) {
        // optimization will be done later
        char content[1000];
        char header[200];
        char response[1200];
        
        // generating response html content
        sprintf(content, "<!Doctype html><html lang='en'><head><meta charset='utf-8'><title>404 Error</title></head><body>Requested Resource [%s] Not Found</body>", requestURI);
        
        // generating response header
        int contentLength = strlen(content);
        sprintf(header, "HTTP/1.1 404 Not Found\r\nContent-Type:text/html\nContent-Length:%d\nConnection: close\r\n\r\n", contentLength);

        strcpy(response, header);
        strcat(response, content);

        // sending response : header + payload
        send(clientSocketDescriptor, response, strlen(response), 0);
    }
    static void sendMethodNotAllowedError(int clientSocketDescriptor, char *method, char *requestURI) {
        // will be done later        
    }
};

// Amit [The Bro Programmer]
class Error {
private:
    string error;
public:
    Error(string error) {
        this->error = error;
    }
    bool hasError() {
        return this->error.length() > 0;
    }
    string getError() {
        return this->error;
    }
};

class Request {
    private:
    char *method;
    char *requestURI;
    char *httpVersion;

    Request(char *method, char *requestURI, char *httpVersion) {
        this->method = method;
        this->requestURI = requestURI;
        this->httpVersion = httpVersion;
    }

    friend class Bro;
};

class Response {
private:
    string contentType;
    forward_list<string> content;
    unsigned long contentLength;

    forward_list<string>::iterator contentIterator;
public:
    Response() {
        this->contentLength = 0;
        this->contentIterator = this->content.before_begin();
    }
    ~Response() {

    }
    void setContentType(string contentType) {
        // validate contentType
        if (Validator::isValidMIMEType(contentType)) {
            this->contentType = contentType;    
        }
    }
    Response & operator<<(string content) {
        this->contentLength += content.length();
        this->contentIterator = this->content.insert_after(this->contentIterator, content);
        return *this;
    }

    friend class HttpResponseUtility;
};

class HttpResponseUtility {
    private:
    HttpResponseUtility() {}

    public:
    static void sendResponse(int clientSocketDescriptor, Response &response) {
        char header[200];
        int contentLength = response.contentLength;
        
        sprintf(header, "HTTP/1.1 200 OK\r\nContent-Type:text/html\nContent-Length:%d\nConnection: close\r\n\r\n", contentLength);
        
        send(clientSocketDescriptor, header, strlen(header), 0);
        
        auto contentIterator = response.content.begin();
        while (contentIterator != response.content.end()) {
            string input = *contentIterator;
            send(clientSocketDescriptor, input.c_str(), input.length(), 0);
            ++contentIterator;
        }
    }
};

// an enumerated data type for request methods
enum __request_method__{
    __GET__,
    __PUT__,
    __POST__,
    __DELETE__,
    __CONNECT__,
    __TRACE__,
    __HEAD__,
    __OPTIONS__
};

// structure to represent url mapping
typedef struct __url__mapping {
    __request_method__ requestMethod;
    void (*mappedFunction)(Request &, Response &);
}URLMapping;

class Bro {
private:
    string staticResourcesFolder;
    map<string, URLMapping> urlMappings;
public:
    Bro() {

    }
    ~Bro() {

    }
    void setStaticResourcesFolder(string staticResourcesFolder) {
        if (Validator::isValidPath(staticResourcesFolder)) {
            this->staticResourcesFolder = staticResourcesFolder;
        } else {
            // not yet decided
        }
    }
    void get(string url, void (*callBack)(Request &, Response &)) {
        if (Validator::isValidURLFormat(url)) {
            this->urlMappings.insert(pair<string, URLMapping> (url, {__GET__, callBack}));
        }
    }
    void listen(int portNumber, void (*callBack)(Error &)) {
        // initializing socket api for windows platform
        #ifdef _WIN32
        WSADATA wsaData;
        WORD word = MAKEWORD(1, 1);
        WSAStartup(word, &wsaData);
        #endif

        char requestBuffer[4097];
        int requestLength;

        int serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocketDescriptor < 0) {
            // failed to create socket
            Error error("Unable to create socket");
            #ifdef _WIN32
            WSACleanup();
            #endif
            callBack(error);
            return; 
        }

        // creating socket structure
        struct sockaddr_in serverSocketInformation;
        serverSocketInformation.sin_family = AF_INET;
        serverSocketInformation.sin_port = htons(portNumber);
        serverSocketInformation.sin_addr.s_addr = htonl(INADDR_ANY);

        // binding the socket
        int successCode = 0;
        successCode = bind(serverSocketDescriptor, (struct sockaddr *) &serverSocketInformation, sizeof(serverSocketInformation));
        if (successCode < 0) {
            // failed to bind socket on specified port
            close(serverSocketDescriptor);
            #ifdef _WIN32
            WSACleanup();
            #endif
            char errorMessage[101];
            sprintf(errorMessage, "Unable to bind socket to port : %d", portNumber);
            Error error(errorMessage);
            callBack(error);
            return;
        }

        // listening for the connections on socket created before
        successCode = ::listen(serverSocketDescriptor, 10);
        if (successCode < 0) {
            close(serverSocketDescriptor);
            #ifdef _WIN32
            WSACleanup();
            #endif
            Error error("Unable to accept client connections");
            callBack(error);
            return;
        }

        Error error("");
        callBack(error);
        
        // listening from client
        struct sockaddr_in clientSocketInformation;
        
        #ifdef _WIN32
        int clientSocketInformationSize = sizeof(clientSocketInformation);
        #endif

        #ifdef linux
        socklen_t clientSocketInformationSize = sizeof(clientSocketInformation);
        #endif
        
        while (true) {
            int clientSocketDescriptor = accept(serverSocketDescriptor, (struct sockaddr *) &clientSocketInformation, &clientSocketInformationSize);
            requestLength = recv(clientSocketDescriptor, requestBuffer, sizeof(requestBuffer) - sizeof(char), 0);

            if (requestLength == 0 || requestLength == -1) {
                close(clientSocketDescriptor);
                continue;
            }


            int index;
            char *method;
            char *requestURI;
            char *httpVersion;

            method = requestBuffer;
            index = 0;
            while (requestBuffer[index] && requestBuffer[index] != ' ') {
                index++;
            }

            if (requestBuffer[index] == '\0') {
                HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }

            requestBuffer[index] = '\0';
            index++;

            if (requestBuffer[index] == ' ' || requestBuffer[index] == '\0') {
                HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }

            StringUtility::toLowerCase(method);

            if (!(strcmp(method, "get") ||
                strcmp(method, "put") ||
                strcmp(method, "post") ||
                strcmp(method, "delete") ||
                strcmp(method, "options") ||
                strcmp(method, "trace") ||
                strcmp(method, "head") ||
                strcmp(method, "connect")
                )) {
                HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }

            requestURI = requestBuffer + index;
            while (requestBuffer[index] && requestBuffer[index] != ' ') index++;

            if (requestBuffer[index] == '\0') {
                HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }

            requestBuffer[index] = '\0';
            index++;

            if (requestBuffer[index] == ' ' || requestBuffer[index] == '\0') {
                HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }

            httpVersion = requestBuffer + index;
            while (requestBuffer[index] && requestBuffer[index] != '\r' && requestBuffer[index] != '\n') index++;

            if (requestBuffer[index] == '\0') {
                HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }

            if (requestBuffer[index] == '\r' && requestBuffer[index + 1] != '\n') {
                HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }

            if (requestBuffer[index] == '\r') {
                requestBuffer[index] = '\0';
                index = index + 2;
            } else {
                requestBuffer[index] = '\0';
                index = index + 1;
            }

            StringUtility::toLowerCase(httpVersion);
            if (strcmp(httpVersion, "http/1.1") != 0) {
                HttpErrorStatusUtility::sendHttpVersionNotSupportedError(clientSocketDescriptor, httpVersion);
                close(clientSocketDescriptor);
                continue;
            }

            auto urlMappingsIterator = urlMappings.find(requestURI);
            if (urlMappingsIterator == urlMappings.end()) {
                HttpErrorStatusUtility::sendNotFoundError(clientSocketDescriptor, requestURI);
                close(clientSocketDescriptor);
                continue;
            }

            URLMapping urlMapping = urlMappingsIterator->second;
            if (urlMapping.requestMethod == __GET__ and strcmp(method, "get") != 0) {
                HttpErrorStatusUtility::sendMethodNotAllowedError(clientSocketDescriptor, method, requestURI);
                close(clientSocketDescriptor);
                continue;
            }

            Request request(method, requestURI, httpVersion);
            Response response;

            urlMapping.mappedFunction(request, response);
            HttpResponseUtility::sendResponse(clientSocketDescriptor, response);
            close(clientSocketDescriptor);
        }

        #ifdef _WIN32
        WSACleanup();
        #endif
    }
};

// Bobby [The Web Application Developer]
int main() {
    Bro bro;
    bro.setStaticResourcesFolder("whatever");
    bro.get("/", [](Request &request, Response &response) {
        const char *html = R""""(
            <!Doctype html> 
            <html lang='en'>
            <head>
                <meta charset='utf-8'>
                <title>HelloWorld</title>
            </head>
            <body>
                <h1>Welcome</h1>
                <h3>Administrator</h3>
                <a href='getCustomers'> Customers List </a>
            </body>
            </html>
        )"""";
        response.setContentType("text/html");
        response << html;
    });

    bro.get("/getCustomers", [](Request &request, Response &response) {
        const char *html = R""""(
            <!Doctype html> 
            <html lang='en'>
            <head>
                <meta charset='utf-8'>
                <title>HelloWorld</title>
            </head>
            <body>
                <h1> LIST OF CUSTOMERS </h1>
                <ul>
                    <li> Ramesh </li>
                    <li> Suresh </li>
                </ul>
                <a href='/'> Home </a>
            </body>
            </html>
        )"""";
        response.setContentType("text/html");
        response << html;
    });

    bro.listen(6060, [](Error &error) {
        if (error.hasError()) {
            cout << error.getError() << endl;
            return;
        }
        cout << "Bro Http Server is ready to accept requests on port: 6060" << endl;
    });

    return 0;
}