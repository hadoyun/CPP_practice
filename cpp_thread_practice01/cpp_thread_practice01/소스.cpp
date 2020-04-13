#include <thread>
#include <iostream>
#include <atomic>
#include <mutex>
//mutual exclusion - ��ȣ ���� // ��װ� ���� �ϴ� ���� �������...!

int main()
{
	int a{}; // �Ӱ� ���� (critical section - �߿��� �κ�)

	//std::atomic<int> a{}; //�ѹ� ������ ���� �������� �ٸ� �����尡 �������� ���ϰ� ��,
	// �ٸ� �Լ��� ���ǵ� (��ȣ�� �Ӱ豸��)

	int b{ 77 };

	std::mutex mut{};

	std::thread thr_add
	{
		[&]()
		{
			for (int i = 0; i < 10000; ++i)
			{
				mut.lock();
				++a;	
				mut.unlock();
			}
		}
		//�̰� ��ü�� �ϳ��� ��ɾ�! 
		//thread�� �ʱ�ȭ
	};

	std::thread thr_subtract
	{
		[&]()
		{
			for (int i = 0; i < 10000; ++i)
			{
				mut.lock();
				--a;
				mut.unlock();
			}
		}
	};

	// �� �����尡 ���� �� ���� ��ٸ��ڴ�.
	// ���� ������� �ٽ� �Ͷ� ��ٷ���
	thr_add.join(); 
	thr_subtract.join();

	std::cout << a;
	//undefinded behavior 


	//�̸� ��ſ� []�� ����Ѵ�.
	/*[]()
	{

	};*/

	return 0;

}