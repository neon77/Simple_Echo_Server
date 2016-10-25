// EchoServer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

std::string CurrentTime()
{
	time_t now = ::time(0);
	struct tm tmstruct;
	localtime_s(&tmstruct, &now);
	char buf[100];
	strftime(buf, sizeof(buf), "[%Y-%m-%d %X] ", &tmstruct);
	return buf;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << CurrentTime() << "need port!" << std::endl;
		system("pause");
		return -1;
	}
	unsigned short port = atoi(argv[1]);

	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (NO_ERROR != ret)
	{
		std::cout << CurrentTime() << "WSAStartup failed with error : " << ret << std::endl;
		system("pause");
		return -2;
	}

	SOCKET listensocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == listensocket)
	{
		std::cout << CurrentTime() << "create listen socket failed!" << std::endl;
		std::cout << CurrentTime() << "socket error [ " << WSAGetLastError() << " ]" << std::endl;
		system("pause");
		return -3;
	}
	
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	serveraddr.sin_port = htons(port);

	int bindret = ::bind(listensocket, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (SOCKET_ERROR == bindret)
	{
		std::cout << CurrentTime() << "bind failed!" << std::endl;
		std::cout << CurrentTime() << "socket error [ " << WSAGetLastError() << " ]" << std::endl;
		system("pause");
		return -4;
	}

	int listenret = ::listen(listensocket, 10);
	if (SOCKET_ERROR == listenret)
	{
		std::cout << CurrentTime() << "listen failed!" << std::endl;
		std::cout << CurrentTime() << "socket error [ " << WSAGetLastError() << " ]" << std::endl;
		system("pause");
		return -5;
	}

	std::cout << CurrentTime() << "port : " << port << std::endl;
	std::cout << CurrentTime() << "waiting for client to connect..." << std::endl;

	struct sockaddr_in clientaddr;
	memset(&clientaddr, 0, sizeof(clientaddr));
	int clientlen = sizeof(clientaddr);
	SOCKET clientsocket = ::accept(listensocket, (struct sockaddr*)&clientaddr, &clientlen);
	if (INVALID_SOCKET == clientsocket)
	{
		std::cout << CurrentTime() << "accept failed!" << std::endl;
		std::cout << CurrentTime() << "socket error [ " << WSAGetLastError() << " ]" << std::endl;
		closesocket(clientsocket);
		WSACleanup();
		system("pause");
		return -6;
	}
	else
	{
		char clientip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(clientaddr.sin_addr), clientip, INET_ADDRSTRLEN);
		std::cout << CurrentTime() << "ip [ " << clientip << " ] client connected!" << std::endl;
	}

	char recvbuffer[2048];
	int recvbufferlength = (int)sizeof(recvbuffer);
	int recvret = 0;
	int sendret = 0;
	while (true)
	{

		memset(recvbuffer, 0, sizeof(recvbuffer));
		recvret = recv(clientsocket, recvbuffer, recvbufferlength, 0);
		if (0 < recvret)
		{
			std::cout << CurrentTime() << recvret << " bytes received!" << std::endl;
		}
		else if (0 == recvret)
		{
			std::cout << CurrentTime() << "connection closed\n" << std::endl;
			break;
		}
		else
		{
			std::cout << CurrentTime() << "recv failed!" << std::endl;
			std::cout << CurrentTime() << "socket error [ " << WSAGetLastError() << " ]" << std::endl;
			break;
		}

		// send a response
		sendret = send(clientsocket, recvbuffer, recvret, 0);
		if (SOCKET_ERROR == sendret)
		{
			std::cout << CurrentTime() << "listen failed!" << std::endl;
			std::cout << CurrentTime() << "socket error [ " << WSAGetLastError() << " ]" << std::endl;
			break;
		}
	}

	closesocket(clientsocket);
	WSACleanup();
	
	system("pause");

	return 0;
}

