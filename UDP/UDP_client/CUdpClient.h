#pragma once
#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
//최신 버전이 2.2 버전
//라이브러리 선언은 header에 구현은 dll (동적) , lib (정적) 파일로
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
			// printf , place holder = %로 시작 
			// 가변인수함수(Variadic function) : 인수의 수를 알 수 없고 사용자가 지정한 대로 변할 수 있는 함수
			// __cdecl C언어 기본 함수 호출 규약 / 이것이 없으면 기본 함수 규약
			printf("error! code: %d\n", error);

			return false;
		}
		return true;
	}

	bool createSocket()
	{
		m_server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		//WSA의 에러를 가지고 위해서는 WSAGetLastError()로 온다.
		if (m_server_socket == INVALID_SOCKET)
		{
			printf("error! code: %d\n", WSAGetLastError());

			return false;
		}

		return true;
	}

	bool cleanup()
	{
		//int error = closesocket(m_server_socket); 심볼 재정의
		int socket_error = closesocket(m_server_socket);

		if (socket_error == SOCKET_ERROR)
		{
			printf("error! code: %d\n", WSAGetLastError());
		}

		//만약에 호출을 안하면 윈도우즈가 알아서 호출해준다. 
		//but 종료가 안된 시점에서 WSAStartup을 실행하면 오류가 날 수 있다.
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

		//sz - string zero-terminated 널문자로 끝나는 문자열
		int error = inet_pton(AF_INET, ip, &addr.sin_addr);

		if (error != 1)
		{
			printf("error! code: %d\n", WSAGetLastError());

			return;
		}

		m_server_addr = addr;
	}


public:
	//자료를 받는 함수
	bool receive()
	{
		sockaddr_in from_addr{};

		int from_addr_size = sizeof(from_addr);

		//몇 바이트를 받았는지 리턴함
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
