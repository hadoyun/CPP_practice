//==deletePP();
//포인터의 참조형을 사용한다면, 원본과 포인터 둘다 바꿀 수 있다.
void deleteRef(int*& ref)
{
	delete ref;
	ref = nullptr;
}

void deleteP(int* p)
{
	delete p;
}

void messPPP(int*** ppp)
{
	*ppp = nullptr;
}

void cannotMessPPP(int** const* ppp)
{
	//컴파일 오류
	//*ppp = nullptr
}

//포인터를 바꾸고 싶을때 포인터의 포인터 **(pp)를 받는다.
void deletePP(int** pp)
{
	messPPP(&pp);

	//delete *pp;
	//*pp = nullptr;
}

int main() {
	//물리적으로 null을 가리키는 da가 생긴다. 
	int* da{};

	//HEAP에 5를 가리키는 공간이 새로 생기고 da
	da = new int{ 5 };

	//heap 영역에 있는 동적 할당된 메모리가 7이 된다. 
	*da = 7;

	{
		//int* da_copy = da;

		//delete는 heap 영역에 있는 동적 할당된 메모리에 접근을 막는다.
		//delete da;

		//da_copy가 가르키는 heap 영역의 값은 접근이 불가능해지고, da는 댕글링포인터가 된다.
		//delete da_copy;
	}

	{
		//함수의 인수는 복사된 값이 들어감으로 da는 댕글링포인터가 된다.

		//deleteP(da);
		//deletePP(&da);

		//delete da;
	}

	//(우리가 체감상) 원본에 접근 할 수 있다...!
	deleteRef(da);



	return 0;
}