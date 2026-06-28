#pragma once

// ============================================================================
//  Shared contract for ALL sorting algorithms (Person 6 owns this file).
//
//  Person 2 / 3 / 4 only write the body of sort(). While comparing or
//  swapping elements they MUST bump the counters in SortMetrics.
//  The counters are atomic because the algorithm runs on a background
//  thread while the UI thread reads them every 50 ms to draw the chart.
// ============================================================================

#include <atomic>
#include <string>
#include <vector>

namespace pttk
{
    struct SortMetrics
    {
        std::atomic<long long> comparisons{ 0 };
        std::atomic<long long> swaps{ 0 };
        std::atomic<bool>      finished{ false };

        void reset()
        {
            comparisons.store(0);
            swaps.store(0);
            finished.store(false);
        }
    };

    // Every real algorithm implements this. The UI does not care which
    // concrete class it is, so plugging in Bubble/Insertion/Quick/Merge/Shell
    // later is just "add one more object to the list".
    class ISortAlgorithm
    {
    public:
        virtual ~ISortAlgorithm() = default;

        virtual std::wstring name() const = 0;

        // data : a PRIVATE copy of the input array (never shared across threads)
        // m    : counters the algorithm keeps updating while it runs
        // stop : cancellation flag for the Stop button / window close
        virtual void sort(std::vector<int>& data,
                           SortMetrics& m,
                           const std::atomic<bool>& stop) = 0;
    };
}
