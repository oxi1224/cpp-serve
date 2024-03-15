#ifdef _WIN32
#include <winsock2.h>
#endif

#ifdef __unix__
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define SOCKET int
#endif

#include <iostream>
#include <sstream>
#include <time.h>
#include <functional>
#include <vector>
#include <map>
#include <filesystem>
#include <fstream>
#include <bitset>
#include <thread>
#include "http.hpp"
#include "response.hpp"
#include "request.hpp"
#include "cookie.hpp"
#include "util.hpp"
#include "constants.hpp"

using namespace http;
using namespace http::util;
namespace fs = std::filesystem;
const int BUFF_SIZE = 1024;

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