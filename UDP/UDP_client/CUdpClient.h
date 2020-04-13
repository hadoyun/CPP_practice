#pragma once
#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
//�ֽ� ������ 2.2 ����
//���̺귯�� ������ header�� ������ dll (����) , lib (����) ���Ϸ�
#include <iostream>

class CUdpClient
{
public:
	CUdpClient() { startup(); createSocket(); };
	~CUdpClient() { cleanup(); };

private:
	bool startup()
	{
		WSADATA wsa_data{};
		//
		int error{ WSAStartup(MAKEWORD(2, 2), &wsa_data) };

		if (error != 0)
		{
			// printf , place holder = %�� ���� 
			// �����μ��Լ�(Variadic function) : �μ��� ���� �� �� ���� ����ڰ� ������ ��� ���� �� �ִ� �Լ�
			// __cdecl C��� �⺻ �Լ� ȣ�� �Ծ� / �̰��� ������ �⺻ �Լ� �Ծ�
			printf("error! code: %d\n", error);

			return false;
		}
		return true;
	}

	bool createSocket()
	{
		m_server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		//WSA�� ������ ������ ���ؼ��� WSAGetLastError()�� �´�.
		if (m_server_socket == INVALID_SOCKET)
		{
			printf("error! code: %d\n", WSAGetLastError());

			return false;
		}

		return true;
	}

	bool cleanup()
	{
		//int error = closesocket(m_server_socket); �ɺ� ������
		int socket_error = closesocket(m_server_socket);

		if (socket_error == SOCKET_ERROR)
		{
			printf("error! code: %d\n", WSAGetLastError());
		}

		//���࿡ ȣ���� ���ϸ� ������� �˾Ƽ� ȣ�����ش�. 
		//but ���ᰡ �ȵ� �������� WSAStartup�� �����ϸ� ������ �� �� �ִ�.
		int error{ WSACleanup() };

		if (error != 0)
		{
			printf("error! code: %d\n", error);

			return false;
		}
		return true;
	}

public:
	void setServerAddr(const char* ip, u_short port)
	{
		sockaddr_in addr{};
		addr.sin_family = AF_INET;

		addr.sin_port = htons(port);

		/*addr.sin_addr.S_un.S_un_b.s_b1 = 192;
		addr.sin_addr.S_un.S_un_b.s_b2 = 168;
		addr.sin_addr.S_un.S_un_b.s_b3 = 35;
		addr.sin_addr.S_un.S_un_b.s_b4 = 148;*/

		//sz - string zero-terminated �ι��ڷ� ������ ���ڿ�
		int error = inet_pton(AF_INET, ip, &addr.sin_addr);

		if (error != 1)
		{
			printf("error! code: %d\n", WSAGetLastError());

			return;
		}

		m_server_addr = addr;
	}


public:
	//�ڷḦ �޴� �Լ�
	bool receive()
	{
		sockaddr_in from_addr{};

		int from_addr_size = sizeof(from_addr);

		//�� ����Ʈ�� �޾Ҵ��� ������
		int byte_count{ recvfrom(m_server_socket, m_buf, m_buffer_size, 0, (sockaddr*)&from_addr, &from_addr_size) };

		if (byte_count <= 0)
		{
			return false;
		}

		return true;
	}

	bool send(const char* message)
	{
		int len{ (int)strlen(message) };

		int byte_count = sendto(m_server_socket, message, len, 0, (sockaddr*)&m_server_addr, sizeof(m_server_addr));

		if (byte_count > 0)
		{
			return true;
		}

		return false;
	}

	const char* getBuf() const
	{
		return m_buf;
	}

private:
	static const int m_buffer_size{ 1024 };

private:
	SOCKET m_server_socket{};
	char m_buf[m_buffer_size]{};

private:
	sockaddr_in m_server_addr{ };
};
