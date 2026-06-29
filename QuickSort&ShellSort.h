#pragma once
#include <vector>
#include <chrono>
using namespace std;

struct SortStats {
    long long comparisons = 0;
    long long swaps = 0;
    double timeMs = 0;
};

// ==================== QUICK SORT ====================

int partition(vector<int>& arr, int low, int high, SortStats& stats) {
    int pivot = arr[high];   // chọn phần tử cuối làm pivot
    int i = low - 1;

    for (int j = low; j < high; j++) {
        stats.comparisons++;
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
            stats.swaps++;
        }
    }
    swap(arr[i + 1], arr[high]);
    stats.swaps++;
    return i + 1;
}

void quickSortHelper(vector<int>& arr, int low, int high, SortStats& stats) {
    if (low < high) {
        int pi = partition(arr, low, high, stats);
        quickSortHelper(arr, low, pi - 1, stats);
        quickSortHelper(arr, pi + 1, high, stats);
    }
}

SortStats quickSort(vector<int> arr) {
    SortStats stats;
    auto start = chrono::high_resolution_clock::now();

    quickSortHelper(arr, 0, (int)arr.size() - 1, stats);

    auto end = chrono::high_resolution_clock::now();
    stats.timeMs = chrono::duration<double, milli>(end - start).count();
    return stats;
}

// ==================== SHELL SORT ====================

SortStats shellSort(vector<int> arr) {
    SortStats stats;
    int n = arr.size();
    auto start = chrono::high_resolution_clock::now();

    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            int j = i;
            while (j >= gap && arr[j - gap] > temp) {
                stats.comparisons++;
                arr[j] = arr[j - gap];
                stats.swaps++;
                j -= gap;
            }
            stats.comparisons++;
            arr[j] = temp;
        }
    }

    auto end = chrono::high_resolution_clock::now();
    stats.timeMs = chrono::duration<double, milli>(end - start).count();
    return stats;
}
