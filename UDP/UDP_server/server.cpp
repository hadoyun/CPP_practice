#include "CUdpServer.h"
#include <iostream>

//TODO client �ϼ�

int main()
{
	CUdpServer server{};

	printf("Server Address: %s:%d \n" ,server.getHostIp().c_str(), server.getServicePort());

	//�Ȱ��� ���� �ص� ��� �ϴ��Ŀ� ���� �޶�����.
	if (server.receive())
	{
		printf("�ޱ� �޾Ҿ��; \n");
	}

	const char* message = server.getBuf();

	printf(message);

	server.sendToAll("��ȣ~~");

	return 0;
}