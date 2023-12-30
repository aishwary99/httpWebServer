#include <iostream>
#include <string.h>
#include <map>
#include <forward_list>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
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

class Bro {
private:
    string staticResourcesFolder;
    map<string, void (*)(Request &, Response &)> urlMappings;
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
            this->urlMappings.insert(pair<string, void (*)(Request &, Response &)> (url, callBack));
        }
    }
    void listen(int portNumber, void (*callBack)(Error &)) {
        // initializing socket api for windows platform
        // WSADATA wsaData;
        // WORD word = MAKEWORD(1, 1);
        // WSAStartup(word, &wsaData);

        char requestBuffer[4096];
        int requestLength;

        int serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocketDescriptor < 0) {
            // failed to create socket
            Error error("Unable to create socket");
            // WSACleanup();
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
            // WSACleanup();
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
            // WSACleanup();
            Error error("Unable to accept client connections");
            callBack(error);
            return;
        }

        Error error("");
        callBack(error);
        
        // listening from client
        struct sockaddr_in clientSocketInformation;
        socklen_t clientSocketInformationSize = sizeof(clientSocketInformation);
        while (true) {
            int clientSocketDescriptor = accept(serverSocketDescriptor, (struct sockaddr *) &clientSocketInformation, &clientSocketInformationSize);
            if (clientSocketDescriptor < 0) {
                // not yet decided
            }

            // extracting information about the request
            // requestLength : how many bytes will be coming up
            requestLength = recv(clientSocketDescriptor, requestBuffer, sizeof(requestBuffer), 0);
            if (requestLength > 0) {
                for (int index = 0; index < requestLength; index++) {
                    printf("%c", requestBuffer[index]);
                }

                // creating header followed by the response body
                const char *response = 
                "HTTP/1.1 200 OK\r\n"
                "Connection: close\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: 100\r\n\r\n"
                "<html><head><title>Thinking Machines</title></head>"
                "<body><h1>Thinking Machines</h1>"
                "<h3>Cool place</h3>"
                "</body></html>";

                // sending back the response
                send(clientSocketDescriptor, response, strlen(response), 0);
            }
        }
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