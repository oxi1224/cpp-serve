#ifndef HTTP_UTIL_HPP
#define HTTP_UTIL_HPP

#include <iostream>

namespace http {
	namespace util {
		void log(std::string message, bool err = false);
	}
}

#endif