#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
//�ֽ� ������ 2.2 ����
//���̺귯�� ������ header�� ������ dll (����) , lib (����) ���Ϸ�
#include <iostream>
#include <vector>
#include <string.h>
#include <mutex>

class CUdpServer
{
public:
	CUdpServer()
	{ 
		startup(); 
		createSocket(); 
		bindSocket(); 
		setHostAddr(); 
	}
	~CUdpServer()
	{
		cleanup(); 
	}

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

	//���������� �ʿ���
	bool bindSocket()
	{
		sockaddr_in addr{};
		addr.sin_family = AF_INET;
		// 0 ~ 1023 - �� �˷��� ��Ʈ ��ȣ 
		//endian / big endian - ū �ڸ����� ���ʿ� �´�. / little endian - ���� �ڸ����� ���ʿ� �´�.
		//���������� �⺻ ������ ���� ����� litte endian �̴�.
		//��Ʈ��ũ�� ǥ�� ���� ����� big endian �̴�. 
		//����Ʈ ������ �޶��� 50000; 50000�� C350 ������ htons �Լ����� 50 C3�� �ȴ�.
		addr.sin_port = htons(SERVICE_PORT);
		//IPv4 - 4����Ʈ�� �̷���� unsinged char 4���� �̷����
		//b - byte, w - word(2byte) dw - double word(4byte)
		//addr.sin_addr.S_un.S_un_b.s_b1 = 192;
		//addr.sin_addr.S_un.S_un_b.s_b2 = 168;
		//addr.sin_addr.S_un.S_un_b.s_b3 = 35;
		//addr.sin_addr.S_un.S_un_b.s_b4 = 148;
		//ȸ�� ���ڰ� �ƴϸ� htonl�� ����ؾ� �Ѵ�.
		addr.sin_addr.S_un.S_addr = INADDR_ANY;

		//�ּҸ� ��Ƶδ� �ڷ����� *(������)�̰�, �ּҸ� ������ ���� �տ� &�̴�.
		//&addr�� sockaddr_in�� �ּ��̰�, �Լ��� ���ϴ� �ڷ����� sockaddr�� �����ͷ� ���� ����ȯ�� �õ��߾���;
		//��, '�ּҸ� ��Ƶδ� ���� ������'�̴�.
		int error{ bind(m_server_socket, (sockaddr*)&addr, sizeof(addr)) };

		//SOCKET_ERROR
		if (error == SOCKET_ERROR)
		{
			printf("error! code: %d\n", WSAGetLastError());

			return false;
		}

		return true;
	}

	void setHostAddr()
	{
		char buf[256]{};

		//sizeof�� �迭�� ũ�Ⱑ �ƴ϶� �迭�� ũ���̴�.
		if (gethostname(buf, (int)(sizeof(buf) / sizeof(char))) == SOCKET_ERROR)
		{
			printf("error! code: %d\n", WSAGetLastError());

			return;
		}

		ADDRINFOA hint{};
		hint.ai_family = PF_INET;

		ADDRINFOA* result{};

		// PSTR == char*
		// PCSTR == const char*
		//node ���� ����Ǿ��ִ� �ϳ��� ��ü
		//servicename == Ư�� ��Ʈ ���� 
		//pp == ������ ������ - ���� �Ҵ��� �Ѵٴ� �ǹ��̴�. 
		//�� �Լ��� ��ġ�鼭 result 
		int error{ getaddrinfo(buf, nullptr, &hint, &result) };

		if (error != 0)
		{
			printf("error! code: %d\n", error);

			return;
		}

		//sockaddr
		memcpy(&m_host_addr, result->ai_addr, sizeof(m_host_addr));

		char ip[16]{};

		inet_ntop(AF_INET, &m_host_addr.sin_addr, ip, sizeof(ip));

		m_host_ip = ip;

		//result ���� �����ִ�.
		freeaddrinfo(result);
	}

public:
	//�ڷḦ �޴� �Լ�, �ִ� �Լ�
	bool receive()
	{
		sockaddr_in c_addr{};

		int c_add{ sizeof(c_addr) };

		//�� ����Ʈ�� �޾Ҵ��� ������
		int byte_count{ recvfrom(m_server_socket, m_buf, m_buffer_size, 0, (sockaddr*)&c_addr, &c_add) };

		if (byte_count > 0)
		{
			// if�� �ȿ� �ߺ��̴�!
			//entry == �׸�
			for (const auto& client : m_v_c_addrs)
			{
				if (client.sin_addr.S_un.S_addr == c_addr.sin_addr.S_un.S_addr)
				{
					return true;
				}
			}
			//blocking function - std::mutex::lock() �� �����Ҷ����� ��� ��ٸ���.
			//dead lock - ���� ��� ����
			if (m_mtx.try_lock())
			{
				m_v_c_addrs.emplace_back(c_addr);

				m_mtx.unlock();
			}

			return true;
		}

		return false;
	}

	bool sendToAll(const char* message) const
	{
		int len{ (int)strlen(message) };

		/*int i{};

		while (message[i] != '\0')
		{
			++i;
		}*/

		bool success{ true };

		for (const auto& c_addr : m_v_c_addrs)
		{
			int byte_count = sendto(m_server_socket, message, len, 0, (sockaddr*)&c_addr, sizeof(c_addr));

			//0���� Ŭ���� ������ - sendto�� �������� ��� 0 ������ ���ڸ� ������ �� �ִ�.
			if (byte_count <= 0)
			{
				success = false;
			}
		}

		return success;
	}

public:
	const char* getBuf() const
	{
		return m_buf;
	}

	const std::string& getHostIp() const
	{
		return m_host_ip;
	}

	unsigned short getServicePort() const
	{
		return SERVICE_PORT;
	}

private:
	// Ŭ���� �ȿ��� ����� �����. -> static���� ������ �� Ŭ���� �ȿ����� ������ ����� ����ų�
	// ���� ������ �����Ѵ�.
	// ������(char, int)�� const �� ����Ѵ�.
	// ��������(float)�� constexpr ���� ��� �ؾ��Ѵ�.
	static const int m_buffer_size{ 1024 };
	const unsigned short SERVICE_PORT{ 15000 };
	


private:
	SOCKET m_server_socket{};
	char m_buf[m_buffer_size]{};
	sockaddr_in m_host_addr{};
	std::string m_host_ip{};

private:
	std::mutex m_mtx{};

private:
	//�̸� �ٲ���.... client �� �����ݾ�...
	// �Ӱ豸��~
	std::vector<sockaddr_in> m_v_c_addrs{};
};
