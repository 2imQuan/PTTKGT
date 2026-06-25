#include <iostream>
#include <vector>
#include "AlgorithmBenchmarkTool/BubbleInsertionSort.h"

int main() {
    std::cout << "============================================" << std::endl;
    std::cout << "  CHUONG TRINH KIEM THU THUAT TOAN C++ (NGUOI 2)" << std::endl;
    std::cout << "============================================" << std::endl;

    // Mảng dữ liệu mẫu
    std::vector<int> testData = { 24, 12, 35, 2, 89, 45, 6, 17 };

    // 1. Chạy thử Bubble Sort
    std::cout << "\n--- CHAY THU: BUBBLE SORT ---" << std::endl;
    std::vector<int> dataBubble = testData;
    Sorting::SortMetrics bubbleStats = Sorting::BubbleSort(dataBubble);
    
    std::cout << "Mang sau sap xep: ";
    for (int val : dataBubble) {
        std::cout << val << " ";
    }
    std::cout << "\nSo lan so sanh (Comparisons): " << bubbleStats.comparisons << std::endl;
    std::cout << "So lan hoan doi (Swaps): " << bubbleStats.swaps << std::endl;
    std::cout << "Thoi gian chay: " << bubbleStats.durationMs << "ms" << std::endl;

    // 2. Chạy thử Insertion Sort
    std::cout << "\n--- CHAY THU: INSERTION SORT ---" << std::endl;
    std::vector<int> dataInsertion = testData;
    Sorting::SortMetrics insertionStats = Sorting::InsertionSort(dataInsertion);
    
    std::cout << "Mang sau sap xep: ";
    for (int val : dataInsertion) {
        std::cout << val << " ";
    }
    std::cout << "\nSo lan so sanh (Comparisons): " << insertionStats.comparisons << std::endl;
    std::cout << "So lan hoan doi/dich (Swaps/Moves): " << insertionStats.swaps << std::endl;
    std::cout << "Thoi gian chay: " << insertionStats.durationMs << "ms" << std::endl;

    std::cout << "\n============================================" << std::endl;
    return 0;
}
