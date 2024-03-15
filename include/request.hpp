#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <map>

namespace http {
	class Request {
	public:
		std::string path;
		std::string method;

		Request(std::string rawRequest);

		void parseRequest();
		std::string getHeader(std::string header);
		std::string getParam(std::string param);
		std::string getCookie(std::string cookie);
		std::string getBody();
	private:
		void parseParams(std::string rawParams);
		void parseCookies(std::string rawCookies);

		std::string m_rawRequest;
		std::string m_httpVersion;
		std::string m_body;
		std::map<std::string, std::string> m_params{};
		std::map<std::string, std::string> m_headers{};
		std::map<std::string, std::string> m_cookies{};
	};
}
#endif