#pragma once

#include <vector>
#include <functional>
#include <atomic>
#include <chrono>
#include <thread>

namespace Sorting
{
    struct SortMetrics
    {
        unsigned long long comparisons = 0;
        unsigned long long swaps = 0;
        unsigned long long durationMs = 0;
    };

    // ProgressCallback signature: (comparisons, swaps, current_array_state, is_finished)
    using ProgressCallback = std::function<void(unsigned long long, unsigned long long, const std::vector<int>&, bool)>;

    struct SortControl
    {
        std::atomic<bool> cancel{ false };
        std::atomic<bool> pause{ false };
    };

    /**
     * @brief Sắp xếp nổi bọt (Bubble Sort)
     * 
     * @param arr Mảng dữ liệu cần sắp xếp
     * @param callback Hàm callback để vẽ biểu đồ real-time (tùy chọn)
     * @param control Biến điều khiển tạm dừng / hủy bỏ (tùy chọn)
     * @param callbackPeriod Chu kỳ gọi callback (ví dụ: cứ sau 100 thao tác) để tránh quá tải UI
     * @return SortMetrics Kết quả thống kê (số so sánh, hoán đổi, thời gian chạy)
     */
    inline SortMetrics BubbleSort(
        std::vector<int>& arr,
        ProgressCallback callback = nullptr,
        SortControl* control = nullptr,
        int callbackPeriod = 100
    ) {
        SortMetrics metrics;
        auto startTime = std::chrono::high_resolution_clock::now();

        size_t n = arr.size();
        bool swapped;
        unsigned long long opCount = 0;

        for (size_t i = 0; i < n - 1; ++i) {
            swapped = false;
            for (size_t j = 0; j < n - i - 1; ++j) {
                // Kiểm tra lệnh hủy từ luồng chính
                if (control && control->cancel.load()) {
                    break;
                }
                // Kiểm tra lệnh tạm dừng
                while (control && control->pause.load()) {
                    if (control->cancel.load()) break;
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }

                metrics.comparisons++;
                opCount++;

                if (arr[j] > arr[j + 1]) {
                    std::swap(arr[j], arr[j + 1]);
                    metrics.swaps++;
                    swapped = true;
                    opCount++;
                }

                // Gọi callback để vẽ biểu đồ và cập nhật UI theo định kỳ
                if (callback && callbackPeriod > 0 && opCount % callbackPeriod == 0) {
                    callback(metrics.comparisons, metrics.swaps, arr, false);
                }
            }

            if (control && control->cancel.load()) {
                break;
            }

            // Nếu vòng lặp trong không có phần tử nào hoán đổi -> mảng đã sắp xếp
            if (!swapped) {
                break;
            }
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        metrics.durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

        // Gửi trạng thái hoàn tất cuối cùng
        if (callback) {
            callback(metrics.comparisons, metrics.swaps, arr, true);
        }

        return metrics;
    }

    /**
     * @brief Sắp xếp chèn (Insertion Sort)
     * 
     * @param arr Mảng dữ liệu cần sắp xếp
     * @param callback Hàm callback để vẽ biểu đồ real-time (tùy chọn)
     * @param control Biến điều khiển tạm dừng / hủy bỏ (tùy chọn)
     * @param callbackPeriod Chu kỳ gọi callback (ví dụ: cứ sau 100 thao tác) để tránh quá tải UI
     * @return SortMetrics Kết quả thống kê (số so sánh, hoán đổi, thời gian chạy)
     */
    inline SortMetrics InsertionSort(
        std::vector<int>& arr,
        ProgressCallback callback = nullptr,
        SortControl* control = nullptr,
        int callbackPeriod = 100
    ) {
        SortMetrics metrics;
        auto startTime = std::chrono::high_resolution_clock::now();

        size_t n = arr.size();
        unsigned long long opCount = 0;

        for (size_t i = 1; i < n; ++i) {
            // Kiểm tra lệnh hủy từ luồng chính
            if (control && control->cancel.load()) {
                break;
            }
            // Kiểm tra lệnh tạm dừng
            while (control && control->pause.load()) {
                if (control->cancel.load()) break;
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }

            int key = arr[i];
            int j = static_cast<int>(i) - 1;

            while (true) {
                if (control && control->cancel.load()) {
                    break;
                }

                if (j < 0) {
                    break;
                }

                metrics.comparisons++;
                opCount++;

                // So sánh khóa với các phần tử đã sắp xếp
                if (arr[j] > key) {
                    arr[j + 1] = arr[j]; // Dịch chuyển
                    metrics.swaps++;    // Đếm mỗi lần dịch chuyển như một thao tác ghi đè (swaps/moves)
                    opCount++;
                    j--;
                }
                else {
                    break;
                }

                // Gọi callback định kỳ
                if (callback && callbackPeriod > 0 && opCount % callbackPeriod == 0) {
                    callback(metrics.comparisons, metrics.swaps, arr, false);
                }
            }

            arr[j + 1] = key;
            metrics.swaps++; // Đếm việc chèn phần tử khóa vào vị trí cuối cùng là một thao tác hoán đổi/ghi
            opCount++;

            if (callback && callbackPeriod > 0 && opCount % callbackPeriod == 0) {
                callback(metrics.comparisons, metrics.swaps, arr, false);
            }
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        metrics.durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

        // Gửi trạng thái hoàn tất cuối cùng
        if (callback) {
            callback(metrics.comparisons, metrics.swaps, arr, true);
        }

        return metrics;
    }
}
