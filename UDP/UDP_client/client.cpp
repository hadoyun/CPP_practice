#include "CUdpClient.h"

int main()
{
	CUdpClient client{};

	client.setServerAddr("192.168.35.229", 15000);

	client.send("¾ßÈ£~");

	client.receive();

	printf(client.getBuf());

	return 0;
}