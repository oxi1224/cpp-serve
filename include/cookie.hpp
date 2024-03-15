#ifndef COOKIE_HPP
#define COOKIE_HPP

#include <iostream>

namespace http {
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

		void setExpiryTime(long int seconds);
		std::string toString() const;

	private:
		std::string getTimeStamp(long int seconds);
	};
}

#endif