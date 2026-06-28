#include <iostream>
#include <vector>
using namespace std;

vector<int> generateRandom(int n)
{
    vector<int> arr;
    for(int i=0;i<n;i++)
        arr.push_back(rand()%10000);
    return arr;
}

vector<int> generateSorted(int n)
{
    vector<int> arr;
    for(int i=1;i<=n;i++)
        arr.push_back(i);
    return arr;
}

vector<int> generateReversed(int n)
{
    vector<int> arr;
    for(int i=n;i>=1;i--)
        arr.push_back(i);
    return arr;
}

vector<int> generatePartial(int n)
{
    vector<int> arr;

    for(int i=1;i<=n;i++)
        arr.push_back(i);

    int swaps=n/10;

    for(int i=0;i<swaps;i++)
    {
        int a=rand()%n;
        int b=rand()%n;

        swap(arr[a],arr[b]);
    }

    return arr;
}
