#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <functional>
#include <vector>
#include <map>
#include "response.hpp"
#include "request.hpp"
#include "cookie.hpp"
#include "util.hpp"
#include "constants.hpp"

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
#endif