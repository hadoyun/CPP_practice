#include <iostream>


using namespace std;

void printMultiplycationTable()
{
    cout << "구구단" << endl;

    for(int i = 1; i < 10; ++i)
    {
        for(int j = 1; j < 10; ++j)
        {
            cout << j << ' ' << "곱하기" << i << ' ' << "는? " << i*j << endl;
        }
    }
}

int main()
{
    printMultiplycationTable();

    return 0;
}