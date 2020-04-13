#include "CUdpServer.h"
#include <iostream>

//TODO client 완성

int main()
{
	CUdpServer server{};

	printf("Server Address: %s:%d \n" ,server.getHostIp().c_str(), server.getServicePort());

	//똑같은 일을 해도 어디서 하느냐에 따라서 달라진다.
	if (server.receive())
	{
		printf("받긴 받았어양; \n");
	}

	const char* message = server.getBuf();

	printf(message);

	server.sendToAll("야호~~");

	return 0;
}