//==deletePP();
//�������� �������� ����Ѵٸ�, ������ ������ �Ѵ� �ٲ� �� �ִ�.
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
	//������ ����
	//*ppp = nullptr
}

//�����͸� �ٲٰ� ������ �������� ������ **(pp)�� �޴´�.
void deletePP(int** pp)
{
	messPPP(&pp);

	//delete *pp;
	//*pp = nullptr;
}

int main() {
	//���������� null�� ����Ű�� da�� �����. 
	int* da{};

	//HEAP�� 5�� ����Ű�� ������ ���� ����� da
	da = new int{ 5 };

	//heap ������ �ִ� ���� �Ҵ�� �޸𸮰� 7�� �ȴ�. 
	*da = 7;

	{
		//int* da_copy = da;

		//delete�� heap ������ �ִ� ���� �Ҵ�� �޸𸮿� ������ ���´�.
		//delete da;

		//da_copy�� ����Ű�� heap ������ ���� ������ �Ұ���������, da�� ��۸������Ͱ� �ȴ�.
		//delete da_copy;
	}

	{
		//�Լ��� �μ��� ����� ���� ������ da�� ��۸������Ͱ� �ȴ�.

		//deleteP(da);
		//deletePP(&da);

		//delete da;
	}

	//(�츮�� ü����) ������ ���� �� �� �ִ�...!
	deleteRef(da);



	return 0;
}