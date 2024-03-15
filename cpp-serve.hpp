#ifndef HTTP_HPP
#define HTTP_HPP

#include <iostream>
#include <functional>
#include <vector>
#include <map>
#include <sstream>
#include <time.h>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <bitset>
#include <thread>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#endif

#ifdef __unix__
#include <sys/socket.h>
#include <arpa/inet.h>
#define SOCKET int
#define SOCKADDR sockaddr
#endif

namespace http {
	namespace constants {
		extern const char* liveReloadInject;
		extern const std::map<int, std::string> StatusCodes;
		extern std::map<std::string, std::string> mimeTypes;
	}

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

	namespace util {
		void log(std::string message, bool err = false);
	}

	typedef std::function<void(Request*, Response*)> Callback;
	typedef std::map<std::string, std::map<std::string, Callback>> ListenerMap;
	void AddMime(std::string extension, std::string type);

	class Server {
	public:
		std::string s_address;
		int s_port;
		bool s_dev;
		int max_threads = 1;

		Server(std::string addr, int port, bool dev = false);
		~Server();

		int start();
		void close();

		void assign(std::string path, std::string method, Callback callback);
		void addIntercept(std::string path, std::string method, Callback callback);
		void setStaticFolderPath(std::string path);

	private:
		void listenForRequests();
		void loadStatic();

		SOCKET m_sock{};
		#ifdef _WIN32
		WSAData m_wsaData{};
		#endif
		sockaddr_in m_sockAddress{};
		unsigned int m_sockAddressLen{};
		ListenerMap m_listeners{};
		ListenerMap m_intercepts{};
		std::string m_staticPath = "";
		std::vector<unsigned int> m_fileSizes{};
		int m_threadCount = 0;
	};
}

using namespace http;
using namespace http::util;
namespace fs = std::filesystem;
const int BUFF_SIZE = 1024;

const char* http::constants::liveReloadInject = R""""(
<!-- Code Injected By Server -->
<script>
  const url = window.location.origin + "/dev/check-refresh";
  async function fetchStatus() {
    const res = await fetch(url);
    const text = await res.text();
    if (text == "true") {
      window.location.reload();
    }
  }

  (async () => {
    const url = window.location.origin + "/dev/check-refresh";
    try {
      await fetchStatus();
      setInterval(async () => await fetchStatus(), 2500);
    } catch (e) { }
  })();
</script>
)"""";

// source: https://developer.mozilla.org/en-US/docs/Web/HTTP/Status
const std::map<int, std::string> http::constants::StatusCodes = {
			{ 100, "Continue" },
			{ 101, "Switching Protocols" },
			{ 102, "Processing" },
			{ 103, "Early Hints" },
			{ 200, "OK" },
			{ 201, "Created" },
			{ 202, "Accepted" },
			{ 203, "Non-Authoritative Information" },
			{ 204, "No Content" },
			{ 205, "Reset Content" },
			{ 206, "Partial Content" },
			{ 207, "Multi-Status" },
			{ 208, "Already Reported" },
			{ 226, "IM Used" },
			{ 300, "Multiple Choices" },
			{ 301, "Moved Permanently" },
			{ 302, "Found" },
			{ 303, "See Other" },
			{ 304, "Not Modified" },
			{ 305, "Use Proxy" },
			{ 306, "unused" },
			{ 307, "Temporary Redirect" },
			{ 308, "Permanent Redirect" },
			{ 400, "Bad Request" },
			{ 401, "Unauthorized" },
			{ 402, "Payment Required" },
			{ 403, "Forbidden" },
			{ 404, "Not Found" },
			{ 405, "Method Not Allowed" },
			{ 406, "Not Acceptable" },
			{ 407, "Proxy Authentication Required" },
			{ 408, "Request Timeout" },
			{ 409, "Conflict" },
			{ 410, "Gone" },
			{ 411, "Length Required" },
			{ 412, "Precondition Failed" },
			{ 413, "Payload Too Large" },
			{ 414, "URI Too Long" },
			{ 415, "Unsupported Media Type" },
			{ 416, "Range Not Satisfiable" },
			{ 417, "Expectation Failed" },
			{ 418, "I'm a teapot" },
			{ 421, "Misdirected Request" },
			{ 422, "Unprocessable Content" },
			{ 423, "Locked" },
			{ 424, "Failed Dependency" },
			{ 425, "Too Early" },
			{ 426, "Upgrade Required" },
			{ 428, "Precondition Required" },
			{ 429, "Too Many Requests" },
			{ 431, "Request Header Fields Too Large" },
			{ 451, "Unavailable For Legal Reasons" },
			{ 500, "Internal Server Error" },
			{ 501, "Not Implemented" },
			{ 502, "Bad Gateway" },
			{ 503, "Service Unavailable" },
			{ 504, "Gateway Timeout" },
			{ 505, "HTTP Version Not Supported" },
			{ 506, "Variant Also Negotiates" },
			{ 507, "Insufficient Storage" },
			{ 508, "Loop Detected" },
			{ 510, "Not Extended" },
			{ 511, "Network Authentication Required" }
};

// source: https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types
std::map<std::string, std::string> http::constants::mimeTypes = {
			{".aac", "audio/aac"},
			{".abw", "application/x-abiword"},
			{".apng", "image/apng"},
			{".arc", "application/x-freearc"},
			{".avif", "image/avif"},
			{".avi", "video/x-msvideo"},
			{".azw", "application/vnd.amazon.ebook"},
			{".bin", "application/octet-stream"},
			{".bmp", "image/bmp"},
			{".bz", "application/x-bzip"},
			{".bz2", "application/x-bzip2"},
			{".cda", "application/x-cdf"},
			{".csh", "application/x-csh"},
			{".css", "text/css"},
			{".csv", "text/csv"},
			{".doc", "application/msword"},
			{".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
			{".eot", "application/vnd.ms-fontobject"},
			{".epub", "application/epub+zip"},
			{".gz", "application/gzip"},
			{".gif", "image/gif"},
			{".htm", "text/html"},
			{".html", "text/html"},
			{".ico", "image/vnd.microsoft.icon"},
			{".ics", "text/calendar"},
			{".jar", "application/java-archive"},
			{".jpeg", "image/jpeg"},
			{".js", "text/javascript"},
			{".json", "application/json"},
			{".jsonld", "application/ld+json"},
			{".mid", "audio/midi"},
			{".mjs", "text/javascript"},
			{".mp3", "audio/mpeg"},
			{".mp4", "video/mp4"},
			{".mpeg", "video/mpeg"},
			{".mpkg", "application/vnd.apple.installer+xml"},
			{".odp", "application/vnd.oasis.opendocument.presentation"},
			{".ods", "application/vnd.oasis.opendocument.spreadsheet"},
			{".odt", "application/vnd.oasis.opendocument.text"},
			{".oga", "audio/ogg"},
			{".ogv", "video/ogg"},
			{".ogx", "application/ogg"},
			{".opus", "audio/opus"},
			{".otf", "font/otf"},
			{".png", "image/png"},
			{".pdf", "application/pdf"},
			{".php", "application/x-httpd-php"},
			{".ppt", "application/vnd.ms-powerpoint"},
			{".pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
			{".rar", "application/vnd.rar"},
			{".rtf", "application/rtf"},
			{".sh", "application/x-sh"},
			{".svg", "image/svg+xml"},
			{".tar", "application/x-tar"},
			{".tif", "image/tiff"},
			{".ts", "video/mp2t"},
			{".ttf", "font/ttf"},
			{".txt", "text/plain"},
			{".vsd", "application/vnd.visio"},
			{".wav", "audio/wav"},
			{".weba", "audio/webm"},
			{".webm", "video/webm"},
			{".webp", "image/webp"},
			{".woff", "font/woff"},
			{".woff2", "font/woff2"},
			{".xhtml", "application/xhtml+xml"},
			{".xls", "application/vnd.ms-excel"},
			{".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
			{".xml", "application/xml"},
			{".xul", "application/vnd.mozilla.xul+xml"},
			{".zip", "application/zip"},
			{".3gp", "video/3gpp"},
			{".3g2", "video/3gpp2"},
			{".7z", "application/x-7z-compressed"}
};

void util::log(std::string message, bool err) {
	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);
	struct tm* parts = std::localtime(&now_c);
	std::string strHr = std::to_string(parts->tm_hour);
	std::string strMin = std::to_string(parts->tm_min);
	std::string strSec = std::to_string(parts->tm_sec);

	if (strHr.length() != 2) {
		strHr = "0" + strHr;
	}
	if (strMin.length() != 2) {
		strMin = "0" + strMin;
	}
	if (strSec.length() != 2) {
		strSec = "0" + strSec;
	}

	std::string timestamp = "[";
	timestamp += strHr;
	timestamp += ":";
	timestamp += strMin;
	timestamp += ":";
	timestamp += strSec;
	timestamp += "] ";

	if (err) {
		std::cerr << timestamp << message << std::endl;
	} else {
		std::cout << timestamp << message << std::endl;
	}
}

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

void Response::setStatus(int code) {
	auto kvp = constants::StatusCodes.find(code);
	if (kvp == constants::StatusCodes.end()) {
		std::cerr << "Invalid response status code provided" << std::endl;
		std::runtime_error err("Invalid response status code provided");
		throw err;
	}
	m_statusCode = kvp->first;
	m_statusText = kvp->second;
}

void Response::setHeader(std::string header, std::string value) {
	m_headers[header] = value;
}

void Response::setContent(std::string body, std::string contentType) {
	m_body = body;
	m_headers["Content-type"] = contentType;
}

void Response::addCookie(std::string cookie) {
	m_cookies.push_back(cookie);
}

std::string Response::toString() {
	std::string date = getTimeStamp();
	std::ostringstream res;
	res << "HTTP/1.1 " << m_statusCode << " " << m_statusText << std::endl;
	res << "Date: " << date << std::endl;
	for (const auto& kvp : m_headers) {
		res << kvp.first << ": " << kvp.second << std::endl;
	}
	for (std::string cookie : m_cookies) {
		res << "Set-Cookie: " << cookie << std::endl;
	}
	res << std::endl;
	res << m_body;
	return res.str();
}

std::string Response::getTimeStamp() {
	char dateStr[512];
	time_t now = time(0);
	struct tm tm = *gmtime(&now);
	strftime(dateStr, sizeof(dateStr), "%a, %d %b %Y %H:%M:%S GMT", &tm);
	return dateStr;
}

Server::Server(std::string addr, int port, bool dev) {
	s_address = addr;
	s_port = port;
	s_dev = dev;
}

Server::~Server() {
	close();
}

int Server::start() {

#ifdef _WIN32
	if (WSAStartup(MAKEWORD(2, 0), &m_wsaData) != 0) {
		log("WSAStartup Failed: " + WSAGetLastError(), true);
		return 1;
	}
#endif

	m_sock = socket(AF_INET, SOCK_STREAM, 0);
	m_sockAddress.sin_family = AF_INET;
	m_sockAddress.sin_addr.s_addr = inet_addr(s_address.c_str());
	m_sockAddress.sin_port = htons(s_port);

	m_sockAddressLen = sizeof(m_sockAddress);

	if (bind(m_sock, (SOCKADDR*)&m_sockAddress, sizeof(m_sockAddress)) < 0) {
		log("Binding failed", true);
		return 1;
	}

	if (listen(m_sock, 10) < 0) {
		log("Listening failed", true);
		return 1;
	}

	log("Listening on address: " + s_address + ":" + std::to_string(s_port));
	loadStatic();
	listenForRequests();
	return 0;
}

void Server::close() {

#ifdef _WIN32
	closesocket(m_sock);
	WSACleanup();
#elif __unix__
	::close(m_sock);
#endif

	exit(0);
}

void Server::listenForRequests() {
	while (true) {
#ifdef _WIN32
		SOCKET clientSock = accept(m_sock, (SOCKADDR*)&m_sockAddress, (int*)&m_sockAddressLen);
#else
		SOCKET clientSock = accept(m_sock, (SOCKADDR*)&m_sockAddress, &m_sockAddressLen);
#endif	

		if (clientSock < 0) {
			log("Connection rejected", true);
			continue;
		}

		while (m_threadCount >= max_threads) {} // wait for threads to free up

		std::thread t1([&, clientSock] {
			m_threadCount += 1;
			char requestBuffer[BUFF_SIZE] = {};

#ifdef _WIN32
			int receivedBytes = recv(clientSock, requestBuffer, sizeof(requestBuffer), 0);
#elif __unix__
			int receivedBytes = read(clientSock, requestBuffer, sizeof(requestBuffer));
#endif

			Request req = Request(std::string(requestBuffer));
			Response res = Response();

			auto pathEntry = m_listeners.find(req.path);
			if (pathEntry != m_listeners.end()) {
				auto methodEntry = pathEntry->second.find(req.method);
				if (methodEntry != pathEntry->second.end()) {
					methodEntry->second(&req, &res);
				}
			}

			auto interceptPathEntry = m_intercepts.find(req.path);
			if (interceptPathEntry != m_intercepts.end()) {
				auto interceptMethodEntry = interceptPathEntry->second.find(req.method);
				if (interceptMethodEntry != interceptPathEntry->second.end()) {
					interceptMethodEntry->second(&req, &res);
				}
			}

			std::string stringRes = res.toString();

#ifdef _WIN32
			send(clientSock, stringRes.c_str(), stringRes.size(), 0);
			closesocket(clientSock);
#elif __unix__
			write(clientSock, stringRes.c_str(), stringRes.size());
			::close(clientSock);
#endif

			m_threadCount -= 1;
			});
		t1.detach();
	}
}

void Server::loadStatic() {
	if (m_staticPath.empty()) return;
	bool dirExists = fs::exists(m_staticPath);
	if (!dirExists) {
		log("Provided static directory path does not exist. (" + m_staticPath + ")", true);
		log("Consider using an absolute path or make sure the path is correct.", true);
		exit(1);
	}

	fs::recursive_directory_iterator staticIter(m_staticPath);
	for (const auto& entry : staticIter) {
		if (entry.is_directory()) {
			continue;
		}
		std::string stringPath = entry.path().generic_string();
		std::string relativePath = stringPath.substr(m_staticPath.length());
		size_t fileExtensionPos = relativePath.find_last_of(".");
		std::string fileExtension = relativePath.substr(fileExtensionPos);
		std::string assignPath = relativePath;
		size_t lastSlashPos = relativePath.find_last_of("/");
		std::string filename = relativePath.substr(lastSlashPos + 1, (fileExtensionPos - 1) - (lastSlashPos + 1) + 1);
		if ((fileExtension == ".html" || fileExtension == ".htm") && filename == "index") {
			assignPath = relativePath.substr(0, lastSlashPos + 1);
		}

		std::string lastChar(1, assignPath[assignPath.length() - 1]);
		if (lastChar == "/") {
			assign(
				assignPath.substr(0, assignPath.length() - 1),
				"GET",
				[assignPath](http::Request* req, http::Response* res) {
					res->setStatus(301);
					res->setHeader("Location", assignPath);
				}
			);
		}

		assign(
			assignPath,
			"GET",
			[&s_dev = s_dev, stringPath](http::Request* req, http::Response* res) {
				std::ifstream file(stringPath, std::ios::binary);
				std::string content;
				content.assign(
					(std::istreambuf_iterator<char>(file)),
					(std::istreambuf_iterator<char>())
				);
				size_t extensionPos = stringPath.find_last_of(".");
				std::string extension = stringPath.substr(extensionPos);
				auto mimeEntry = constants::mimeTypes.find(extension);
				res->setStatus(500);
				if (mimeEntry != constants::mimeTypes.end()) {
					res->setStatus(200);
					if (s_dev && (extension == ".html" || extension == ".htm")) {
						size_t bodyEndPos = content.find("</body>");
						if (bodyEndPos != std::string::npos) {
							content.insert(bodyEndPos - 1, constants::liveReloadInject);
						}
					}
					res->setContent(content, mimeEntry->second);
				} else {
					log("Mime type for file extension " + extension + " not found", true);
				}
			}
		);

		if (s_dev) {
			assign(
				"/dev/check-refresh",
				"GET",
				[this](http::Request* req, http::Response* res) {
					std::string currentUri = req->getBody();
					res->setStatus(200);
					int i = 0;
					for (const auto& entry : fs::recursive_directory_iterator(m_staticPath)) {
						std::string stringPath = entry.path().generic_string();
						unsigned int fileSize = entry.file_size();
						if (i >= m_fileSizes.size()) {
							m_fileSizes.push_back(fileSize);
						} else if (m_fileSizes[i] != fileSize) {
							res->setContent("true", "text/plain");
							m_fileSizes[i] = fileSize;
							loadStatic();
							break;
						}
						i++;
					}
				}
			);
		}
	}
}

void Server::assign(std::string path, std::string method, Callback callback) {
	auto pathEntry = m_listeners.find(path);
	if (pathEntry == m_listeners.end()) {
		m_listeners[path] = std::map<std::string, Callback>{ {method, callback} };
	} else {
		pathEntry->second[method] = callback;
	}
}

void Server::addIntercept(std::string path, std::string method, Callback callback) {
	auto pathEntry = m_intercepts.find(path);
	if (pathEntry == m_intercepts.end()) {
		m_intercepts[path] = std::map<std::string, Callback>{ {method, callback} };
	} else {
		pathEntry->second[method] = callback;
	}
}

void Server::setStaticFolderPath(std::string path) {
	m_staticPath = path;
}

void http::AddMime(std::string extension, std::string type) {
	constants::mimeTypes[extension] = type;
}

#endif