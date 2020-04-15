#include <thread>
#include <iostream>
#include <atomic>
#include <mutex>
//mutual exclusion - 상호 배제 // 잠그고 해제 하는 것을 해줘야함...!

void plus(int a)
{
	for (int i = 0; i < 10000; ++i)
	{
		++a;
	}
}

int main()
{
	unsigned int aaa = (unsigned int)'임';
	int a{}; // 임계 구역 (critical section - 중요한 부분)

	//std::atomic<int> a{}; //한번 연산을 끝날 때까지는 다른 쓰레드가 접근하지 못하게 함,
	// 다른 함수가 못건듦 (보호된 임계구역)

	int b{ 77 };

	std::mutex mut{};
	



	//이름 대신에 []를 사용한다.
	/*[]()
	{

	};*/

	return 0;

}