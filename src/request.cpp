#include <iostream>
#include <sstream>
#include <string>
#include "request.hpp"

using namespace http;

Request::Request(std::string rawRequest) {
	m_rawRequest = rawRequest;
	parseRequest();
}

void Request::parseRequest() {
	std::istringstream stream(m_rawRequest);
	std::string line;

	// get method, path and params
	std::getline(stream, line);
	std::istringstream pathLineStream(line);
	std::string _path;
	std::getline(pathLineStream, method, ' ');
	std::getline(pathLineStream, _path, ' ');
	std::getline(pathLineStream, m_httpVersion);

	size_t paramsPos = _path.find("?");
	if (paramsPos != std::string::npos) {
		path = _path.substr(0, paramsPos);
		parseParams(_path.substr(paramsPos + 1));
	} else {
		path = _path;
	}

	// get headers
	while (std::getline(stream, line) && !line.empty()) {
		if (line.empty() || line == "\r") {
			break;
		}
		std::istringstream lineStream(line);
		std::string header, value;
		if (std::getline(lineStream, header, ':') && std::getline(lineStream, value)) {
			value.erase(value.find_first_of(' '), 1);
			if (header == "Cookie") {
				parseCookies(value);
				continue;
			}
			m_headers[header] = value;
		}
	}

	// get body
	std::string body;
	while (std::getline(stream, line)) {
		body += line + "\n";
	}
	size_t lastNewLine = body.find_last_of("\n");
	if (lastNewLine != std::string::npos) {
		body.erase(lastNewLine);
	}
	m_body = body;
}

std::string Request::getHeader(std::string header) {
	auto entry = m_headers.find(header);
	if (entry == m_headers.end()) {
		return "";
	}
	return entry->second;
}

std::string Request::getParam(std::string param) {
	auto entry = m_params.find(param);
	if (entry == m_params.end()) {
		return "";
	}
	return entry->second;
}

std::string Request::getCookie(std::string cookie) {
	auto entry = m_cookies.find(cookie);
	if (entry == m_cookies.end()) {
		return "";
	}
	return entry->second;
}

std::string Request::getBody() {
	return m_body;
}

void Request::parseParams(std::string rawParams) {
	std::istringstream lineStream(rawParams);
	std::string key, val;
	while (std::getline(lineStream, key, '=') && std::getline(lineStream, val, '&')) {
		key.erase(0, key.find_first_not_of(" \t\r\n"));
		val.erase(val.find_last_not_of(" \t\r\n") + 1);
		m_params[key] = val;
	}
}

void Request::parseCookies(std::string rawCookies) {
	std::istringstream lineStream(rawCookies);
	std::string key, val;
	while (std::getline(lineStream, key, '=') && std::getline(lineStream, val, ';')) {
		key.erase(0, key.find_first_not_of(" \t\r\n"));
		val.erase(val.find_last_not_of(" \t\r\n") + 1);
		m_cookies[key] = val;
	}
}