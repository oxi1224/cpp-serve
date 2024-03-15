#include "include/http.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

// this is just an example, do not use this

std::string verifiedID = "1";

const std::string login = "secret-login";
const std::string password = "secret-password";

using namespace http;
int main() {
	Server srv = Server("0.0.0.0", 4000, false);
	srv.max_threads = 2;
	srv.setStaticFolderPath("./static");

	srv.assign(
		"/api/login/",
		"POST",
		[](Request* req, Response* res) {
			std::istringstream body(req->getBody());
			std::string _login, _password;
			std::getline(body, _login);
			std::getline(body, _password);
			if (_login == login && _password == password) {
				res->setStatus(200);

				Cookie uuidCookie;
				uuidCookie.name = "id";
				uuidCookie.value = verifiedID;
				uuidCookie.path = "/";
				uuidCookie.sameSite = "Strict";
				uuidCookie.httpOnly = true;
				uuidCookie.setExpiryTime(3600);
				res->addCookie(uuidCookie.toString());
			} else {
				res->setContent("Invalid credentials", "text/plain");
			}
		}
	);

	srv.addIntercept(
		"/protected/",
		"GET",
		[](Request* req, Response* res) {
			std::string id = req->getCookie("id");
			if (id != verifiedID) {
				res->setStatus(303); // Unauthorized
				res->setHeader("Location", "/"); // Redirect to /
			}
		}
	);
	srv.start();
	return 0;
}