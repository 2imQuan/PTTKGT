#pragma once

// ============================================================================
//  MoreSorts.h - Quick / Merge / Shell Sort theo DUNG contract cua Nguoi 2.
//
//  Cung namespace Sorting, cung chu ky ham, cung quy uoc dem:
//    - comparisons++ moi lan SO SANH hai phan tu
//    - swaps++       moi lan HOAN DOI / GHI DE phan tu
//    - callback goi dinh ky (moi callbackPeriod thao tac) tren THREAD NEN
//    - control->cancel / control->pause de dung / tam dung
//
//  Nguoi 3 (Quick, Shell) va Nguoi 4 (Merge) co the thay phan than ham
//  bang code cua minh, MIEN LA giu nguyen chu ky va quy uoc dem o tren.
//  (Yeu cau cua thay: logic cot loi tu cai dat, khong dung std::sort.)
//
//  Phu thuoc: BubbleInsertionSort.h (dinh nghia SortMetrics, ProgressCallback,
//  SortControl). File nay chi bo sung them 3 thuat toan.
// ============================================================================

#include "BubbleInsertionSort.h"

#include <vector>
#include <chrono>
#include <thread>

namespace Sorting
{
    namespace detail
    {
        // Kiem tra pause/cancel dung chung cho ca 3 thuat toan.
        // Tra ve true neu can DUNG han (cancel).
        inline bool CheckControl(SortControl* control)
        {
            if (!control) return false;
            while (control->pause.load())
            {
                if (control->cancel.load()) return true;
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            return control->cancel.load();
        }

        inline void MaybeCallback(const ProgressCallback& cb, int period,
                                  unsigned long long& opCount,
                                  const SortMetrics& m,
                                  const std::vector<int>& arr)
        {
            if (cb && period > 0 && opCount % period == 0)
                cb(m.comparisons, m.swaps, arr, false);
        }
    }

    // ------------------------------------------------------------------
    // SHELL SORT
    // ------------------------------------------------------------------
    inline SortMetrics ShellSort(
        std::vector<int>& arr,
        ProgressCallback callback = nullptr,
        SortControl* control = nullptr,
        int callbackPeriod = 100)
    {
        SortMetrics metrics;
        auto startTime = std::chrono::high_resolution_clock::now();

        size_t n = arr.size();
        unsigned long long opCount = 0;

        for (size_t gap = n / 2; gap > 0; gap /= 2)
        {
            for (size_t i = gap; i < n; ++i)
            {
                if (detail::CheckControl(control)) goto done;

                int temp = arr[i];
                size_t j = i;

                while (j >= gap)
                {
                    metrics.comparisons++;
                    opCount++;
                    if (arr[j - gap] > temp)
                    {
                        arr[j] = arr[j - gap];   // ghi de / dich chuyen
                        metrics.swaps++;
                        opCount++;
                        j -= gap;
                        detail::MaybeCallback(callback, callbackPeriod, opCount, metrics, arr);
                    }
                    else
                    {
                        break;
                    }
                }
                arr[j] = temp;
                metrics.swaps++;
                opCount++;
                detail::MaybeCallback(callback, callbackPeriod, opCount, metrics, arr);
            }
        }

    done:
        auto endTime = std::chrono::high_resolution_clock::now();
        metrics.durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            endTime - startTime).count();
        if (callback) callback(metrics.comparisons, metrics.swaps, arr, true);
        return metrics;
    }

    // ------------------------------------------------------------------
    // QUICK SORT (Lomuto partition, de quy)
    // ------------------------------------------------------------------
    namespace detail
    {
        inline void QuickSortRun(std::vector<int>& arr, int low, int high,
                                 SortMetrics& metrics, unsigned long long& opCount,
                                 const ProgressCallback& callback, int callbackPeriod,
                                 SortControl* control, bool& cancelled)
        {
            if (cancelled || low >= high) return;
            if (CheckControl(control)) { cancelled = true; return; }

            int pivot = arr[high];
            int i = low - 1;

            for (int j = low; j < high; ++j)
            {
                if (CheckControl(control)) { cancelled = true; return; }

                metrics.comparisons++;
                opCount++;
                if (arr[j] < pivot)
                {
                    ++i;
                    std::swap(arr[i], arr[j]);
                    metrics.swaps++;
                    opCount++;
                    MaybeCallback(callback, callbackPeriod, opCount, metrics, arr);
                }
            }
            std::swap(arr[i + 1], arr[high]);
            metrics.swaps++;
            opCount++;
            MaybeCallback(callback, callbackPeriod, opCount, metrics, arr);

            int p = i + 1;
            QuickSortRun(arr, low, p - 1, metrics, opCount, callback, callbackPeriod, control, cancelled);
            QuickSortRun(arr, p + 1, high, metrics, opCount, callback, callbackPeriod, control, cancelled);
        }
    }

    inline SortMetrics QuickSort(
        std::vector<int>& arr,
        ProgressCallback callback = nullptr,
        SortControl* control = nullptr,
        int callbackPeriod = 100)
    {
        SortMetrics metrics;
        auto startTime = std::chrono::high_resolution_clock::now();

        unsigned long long opCount = 0;
        bool cancelled = false;
        if (!arr.empty())
            detail::QuickSortRun(arr, 0, static_cast<int>(arr.size()) - 1,
                                 metrics, opCount, callback, callbackPeriod,
                                 control, cancelled);

        auto endTime = std::chrono::high_resolution_clock::now();
        metrics.durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            endTime - startTime).count();
        if (callback) callback(metrics.comparisons, metrics.swaps, arr, true);
        return metrics;
    }

    // ------------------------------------------------------------------
    // MERGE SORT (de quy, tron 2 nua)
    // ------------------------------------------------------------------
    namespace detail
    {
        inline void Merge(std::vector<int>& arr, int left, int mid, int right,
                          SortMetrics& metrics, unsigned long long& opCount,
                          const ProgressCallback& callback, int callbackPeriod)
        {
            std::vector<int> tmp;
            tmp.reserve(right - left + 1);

            int i = left, j = mid + 1;
            while (i <= mid && j <= right)
            {
                metrics.comparisons++;
                opCount++;
                if (arr[i] <= arr[j])
                    tmp.push_back(arr[i++]);
                else
                    tmp.push_back(arr[j++]);
            }
            while (i <= mid)  tmp.push_back(arr[i++]);
            while (j <= right) tmp.push_back(arr[j++]);

            for (int k = 0; k < static_cast<int>(tmp.size()); ++k)
            {
                arr[left + k] = tmp[k];   // ghi de khi tron
                metrics.swaps++;
                opCount++;
                MaybeCallback(callback, callbackPeriod, opCount, metrics, arr);
            }
        }

        inline void MergeSortRun(std::vector<int>& arr, int left, int right,
                                 SortMetrics& metrics, unsigned long long& opCount,
                                 const ProgressCallback& callback, int callbackPeriod,
                                 SortControl* control, bool& cancelled)
        {
            if (cancelled || left >= right) return;
            if (CheckControl(control)) { cancelled = true; return; }

            int mid = left + (right - left) / 2;
            MergeSortRun(arr, left, mid, metrics, opCount, callback, callbackPeriod, control, cancelled);
            MergeSortRun(arr, mid + 1, right, metrics, opCount, callback, callbackPeriod, control, cancelled);
            if (cancelled) return;
            Merge(arr, left, mid, right, metrics, opCount, callback, callbackPeriod);
        }
    }

    inline SortMetrics MergeSort(
        std::vector<int>& arr,
        ProgressCallback callback = nullptr,
        SortControl* control = nullptr,
        int callbackPeriod = 100)
    {
        SortMetrics metrics;
        auto startTime = std::chrono::high_resolution_clock::now();

        unsigned long long opCount = 0;
        bool cancelled = false;
        if (!arr.empty())
            detail::MergeSortRun(arr, 0, static_cast<int>(arr.size()) - 1,
                                 metrics, opCount, callback, callbackPeriod,
                                 control, cancelled);

        auto endTime = std::chrono::high_resolution_clock::now();
        metrics.durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            endTime - startTime).count();
        if (callback) callback(metrics.comparisons, metrics.swaps, arr, true);
        return metrics;
    }
}
