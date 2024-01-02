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

class Request {};

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
            if (clientSocketDescriptor < 0) {
                // not yet decided
            }

            forward_list<string> requestBufferDS;
            forward_list<string>::iterator requestBufferDSIterator;
            requestBufferDSIterator = requestBufferDS.before_begin();

            int requestBufferDSSize = 0;
            int requestDataCount = 0;

            // collecting request contents (in bytes)
            while (true) {
                requestLength = recv(clientSocketDescriptor, requestBuffer, sizeof(requestBuffer) - sizeof(char), 0);
                if (requestLength == 0) break;
                requestBuffer[requestLength] = '\0';
                // not optimal : as string copying will be done behind the scenes
                requestBufferDSIterator = requestBufferDS.insert_after(requestBufferDSIterator, string(requestBuffer));
                requestBufferDSSize++;
                requestDataCount += requestLength;
            }

            if (requestBufferDSSize > 0) {
                char *requestData = new char[requestDataCount + 1];
                char *requestDataPointer = requestData;
                const char *temp;

                requestBufferDSIterator = requestBufferDS.begin();
                while (requestBufferDSIterator != requestBufferDS.end()) {
                    temp = (*requestBufferDSIterator).c_str();
                    while (*temp) {
                        *requestDataPointer = *temp;
                        requestDataPointer++;
                        temp++;
                    }
                    ++requestBufferDSIterator;
                }
                *requestDataPointer = '\0';
                requestBufferDS.clear();

                printf("--- Request data begin ---\n");
                printf("%s\n", requestData);
                printf("--- Request data end ---\n");

                // code to parse request goes here
                delete [] requestData;
            } else {
                // case : if no data received
            }

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