#include <iostream>
#include <string>
using namespace std;

// Amit [The Bro Programmer]
class Error {
public:
    bool hasError() {
        return false;
    }
    string getError() {
        return "";
    }
};

class Request {};

class Response {
public:
    void setContentType(string contentType) {
        // do nothing right now
    }
    Response &operator<<(string content) {
        return *this;
    }
};

class Bro {
public:
    void setStaticResourcesFolder(string staticResourcesFolder) {
        // do nothing right now
    }
    void get(string urlPattern, void (*callBack)(Request &, Response &)) {
        // do nothing right now
    }
    void listen(int portNumber, void (*callBack)(Error &)) {
        // do nothing right now
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