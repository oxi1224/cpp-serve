#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <map>
#include <vector>

namespace http {
	class Response {
	public:
		void setStatus(int code);
		void setHeader(std::string header, std::string value);
		void setContent(std::string body, std::string contentType);
		void addCookie(std::string cookie);
		std::string toString();

	private:
		std::string getTimeStamp();

		int m_statusCode = 404;
		std::string m_statusText = "Not Found";
		std::map<std::string, std::string> m_headers{};
		std::string m_body = "";
		std::vector<std::string> m_cookies{};
	};
}

#endif