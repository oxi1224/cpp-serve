#include <iostream>
#include <sstream>
#include <time.h>
#include "cookie.hpp"

using namespace http;

void Cookie::setExpiryTime(long int seconds) {
	expires = getTimeStamp(seconds);
}

std::string Cookie::toString() const {
	if (name.empty()) {
		std::cerr << "Cookie name is empty" << std::endl;
		throw std::runtime_error("Cookie name is empty");
	}
	std::ostringstream out;
	out << name << "=" << value;
	if (!expires.empty()) {
		out << "; Expires=" << expires;
	}
	if (maxAge > 0) {
		out << "; Max-Age=" << maxAge;
	}
	if (!domain.empty()) {
		out << "; Domain=" << domain;
	}
	if (!path.empty()) {
		out << "; Path=" << path;
	}
	if (!sameSite.empty()) {
		out << "; SameSite=" << sameSite;
	}
	if (secure) {
		out << "; Secure";
	}
	if (httpOnly) {
		out << "; HttpOnly";
	}
	return out.str();
}

std::string Cookie::getTimeStamp(long int seconds) {
	char dateStr[512];
	time_t current = time(0);
	time_t t = current + seconds;
	struct tm tm = *gmtime(&t);
	strftime(dateStr, sizeof(dateStr), "%a, %d %b %Y %H:%M:%S GMT", &tm);
	return dateStr;
}