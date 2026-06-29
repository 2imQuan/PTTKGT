#include <iostream>
#include "QuickSort&ShellSort.h"
using namespace std;

int main() {
    vector<int> arr = { 5, 3, 8, 1, 9, 2, 7, 4, 6 };

    SortStats q = quickSort(arr);
    cout << "Quick Sort -> Comparisons: " << q.comparisons
        << " | Swaps: " << q.swaps
        << " | Time: " << q.timeMs << " ms" << endl;

    SortStats s = shellSort(arr);
    cout << "Shell Sort -> Comparisons: " << s.comparisons
        << " | Swaps: " << s.swaps
        << " | Time: " << s.timeMs << " ms" << endl;

    return 0;
}