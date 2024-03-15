# CPP-SERVE

## Table of Contents
1. [Description](#description)
2. [Installation](#installation)
3. [Features](#features)
4. [Usage](#usage)
5. [Documentation](#documentation)
6. [License](#license)
7. [Authors](#authors)

## Description
cpp-serve is an HTTP server written from scratch with different useful features and it supports both Windows and Linux

## Installation
(**REQUIRES CPP 17 AT MINIMUM**)
Install via CMAKE or use the standalone ``cpp-serve.hpp``
## Usage
See [features](#features) and /examples/ in source code.

## Features
**Automatically serving static files**
*Note: **The path is relative to the executable unless given an absolute path***
*Note: If you have any non standard file extensions, you can add their mime type with ``http::AddMime(".ext", "mime/type")``*
```cpp
http::Server srv("0.0.0.0", 4000, false); // address, port, dev_mode
// Contents of ./static will not be available client-side
// This feature uses folder based structuring e.g
// ./index.html -> turns into /
// ./sercret/index.html -> turns into /secret/
srv.setStaticFolderPath("./static");
srv.start();
```
**Live reload in developer mode (use with automatic file serving)**
```cpp
// Initialize the server in developer mode
http::Server srv("0.0.0.0", 4000, true); // address, port, dev_mode
srv.setStaticFolderPath("./static");
srv.start();
```
**Manually asigning code to paths**
```cpp
http::Server srv("0.0.0.0", 4000, false); // address, port, dev_mode
srv.assign(
	"/" // path
	"GET" // method
	[](http::Request* req, http::Response* res) {
		res->setStatus(200); // set status code to 200
		res->setContent("hello world", "text/plain"); // set content
	}
);
srv.start();
```
**Intercepting requests**
Use this if you want to for example, restrict automatically served pages.
*Note: this does not stop main request handler from executing.*
```cpp
http::Server srv("0.0.0.0", 4000, true); // address, port, dev_mode
srv.addIntercept(
	"/admin/" // path
	"GET", // method
	[](http::Request* req, http::Response* res) {
		std::string loginCookie = req->getCookie("id"); // get "id" cookie from request
		if (!isAuthorized(loginCookie)) { // arbitrary code to check authorization
			res->setStatus(303); // set status code 303
			res->setHeader("Location", "/"); // redirect user to /
		}
	}
);
srv.start();
```
**Utility cookie class**
```cpp
http::Cookie uuidCookie;
uuidCookie.name = "id";
uuidCookie.value = "some-id";
uuidCookie.path = "/";
uuidCookie.sameSite = "Strict";
uuidCookie.httpOnly = true;
uuidCookie.setExpiryTime(3600);
res->addCookie(uuidCookie.toString()); // makes the client set the following cookie
```
## Documentation
``Server``
```cpp
class Server {
	public:
	std::string address;
	int port;
	bool dev;
	int max_threads = 1;
	
	// constructor
	Server(std::string addr, int port, bool dev = false);
	~Server();
	
	// Starts the server
	int start();

	// Closes the server
	void close();
	
	// Asigns a path to given callback
	void assign(std::string path, std::string method, Callback callback);

	// Adds an intercept to given path
	void addIntercept(std::string path, std::string method, Callback callback);

	// Sets the static folder path
	void setStaticFolderPath(std::string path);
};
```

``Response``
```cpp
class Response {
	public:
	// Sets the status code, the message is automatically set
	void setStatus(int code);

	// Sets a given header
	void setHeader(std::string header, std::string value);

	// Sets the body of the response
	void setContent(std::string body, std::string contentType);

	// Adds a cookie to the response. TAKES A STRING
	void addCookie(std::string cookie);

	// Converts to a HTTP cookie
	std::string toString();
};
```
``Request``
```cpp
class Request {
	public:
	// Path of the request e.g / or /user/panel/
	std::string path;
	// The method used
	std::string method;
	
	// Takes in the raw HTTP request as a string
	Request(std::string rawRequest);
	
	// Parses the request (called automatically)
	void parseRequest();

	// Gets a given header
	std::string getHeader(std::string header);

	// Gets a given URL param, e.g /panel?key=some-key&admin=false
	std::string getParam(std::string param);

	// Gets a given cookie by it's ID
	std::string getCookie(std::string cookie);

	// Returns the request body
	std::string getBody();
}
```
``Cookie``
```cpp
class Cookie {
	public:
	std::string name = "";
	std::string value = "";
	std::string expires = "";
	std::string sameSite = "";
	std::string domain = "";
	std::string path = "";
	bool secure = false;
	bool httpOnly = false;
	int maxAge = 0;
	
	// Sets the expiration time, accepts time in seconds, e.g 3600 will make the cookie expire in 1hr
	void setExpiryTime(long int seconds);
	// Turns the cookie into a HTTP cookie string
	std::string toString() const;
};
```

## License
[Attribution-NonCommercial 4.0 International License](LICENSE.md)

## Authors
- [Oxi1224](https://github.com/oxi1224)
