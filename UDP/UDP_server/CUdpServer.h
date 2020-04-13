#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
//최신 버전이 2.2 버전
//라이브러리 선언은 header에 구현은 dll (동적) , lib (정적) 파일로
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

	//서버에서만 필요함
	bool bindSocket()
	{
		sockaddr_in addr{};
		addr.sin_family = AF_INET;
		// 0 ~ 1023 - 잘 알려진 포트 번호 
		//endian / big endian - 큰 자릿수가 앞쪽에 온다. / little endian - 작은 자릿수가 앞쪽에 온다.
		//윈도우즈의 기본 데이터 저장 방식은 litte endian 이다.
		//네트워크의 표준 저장 방식은 big endian 이다. 
		//바이트 순서가 달라진 50000; 50000은 C350 임으로 htons 함수에서 50 C3이 된다.
		addr.sin_port = htons(SERVICE_PORT);
		//IPv4 - 4바이트로 이루어짐 unsinged char 4개로 이루어짐
		//b - byte, w - word(2byte) dw - double word(4byte)
		//addr.sin_addr.S_un.S_un_b.s_b1 = 192;
		//addr.sin_addr.S_un.S_un_b.s_b2 = 168;
		//addr.sin_addr.S_un.S_un_b.s_b3 = 35;
		//addr.sin_addr.S_un.S_un_b.s_b4 = 148;
		//회문 숫자가 아니면 htonl을 사용해야 한다.
		addr.sin_addr.S_un.S_addr = INADDR_ANY;

		//주소를 담아두는 자료형은 *(포인터)이고, 주소를 얻어오는 것은 앞에 &이다.
		//&addr는 sockaddr_in의 주소이고, 함수가 원하는 자료형은 sockaddr의 포인터로 강제 형변환을 시도했었음;
		//즉, '주소를 담아두는 것은 포인터'이다.
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

		//sizeof는 배열의 크기가 아니라 배열의 크기이다.
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
		//node 각각 연결되어있는 하나의 객체
		//servicename == 특정 포트 네임 
		//pp == 포인터 포인터 - 동적 할당을 한다는 의미이다. 
		//이 함수를 거치면서 result 
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

		//result 값은 남아있다.
		freeaddrinfo(result);
	}

public:
	//자료를 받는 함수, 주는 함수
	bool receive()
	{
		sockaddr_in c_addr{};

		int c_add{ sizeof(c_addr) };

		//몇 바이트를 받았는지 리턴함
		int byte_count{ recvfrom(m_server_socket, m_buf, m_buffer_size, 0, (sockaddr*)&c_addr, &c_add) };

		if (byte_count > 0)
		{
			// if문 안에 중복이다!
			//entry == 항목
			for (const auto& client : m_v_c_addrs)
			{
				if (client.sin_addr.S_un.S_addr == c_addr.sin_addr.S_un.S_addr)
				{
					return true;
				}
			}
			//blocking function - std::mutex::lock() 이 가능할때까지 계속 기다린다.
			//dead lock - 무한 대기 상태
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

			//0보다 클때만 성공함 - sendto는 실패했을 경우 0 이하의 숫자를 리턴할 수 있다.
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
	// 클래스 안에서 상수를 만든다. -> static으로 접근이 이 클래스 안에서만 가능한 상수로 만들거나
	// 전역 변수로 선언한다.
	// 정수형(char, int)은 const 를 사용한다.
	// 비정수형(float)은 constexpr 까지 사용 해야한다.
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
	//이름 바꾸자.... client 인 지모르잖앙...
	// 임계구역~
	std::vector<sockaddr_in> m_v_c_addrs{};
};
