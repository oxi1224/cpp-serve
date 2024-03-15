#ifndef CONSTANT_HPP
#define CONSTANT_HPP

#include <map>
#include <iostream>

namespace http {
	namespace constants {
		extern const char* liveReloadInject;
		extern const std::map<int, std::string> StatusCodes;
		extern std::map<std::string, std::string> mimeTypes;
	}
}

#endif