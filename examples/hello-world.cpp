#include "include/http.hpp"

using namespace http;
int main() {
	Server srv = Server("0.0.0.0", 4000, false);
	srv.assign(
		"/",
		"GET",
		[](Request* req, Response* res) {
			res->setStatus(200);
			res->setContent("<body>Hello</body>", "text/html");
		}
	);
	srv.start();
	return 0;
}