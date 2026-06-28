#pragma once

// ============================================================================
//  FakeSort: a stand-in algorithm used ONLY to test the pipeline
//      background thread -> atomic counters -> DispatcherTimer -> Win2D chart
//
//  It does not really sort. It sleeps a little and bumps the counters by
//  random amounts. Each instance gets a different "speed" so the 5 lines on
//  the chart grow at visibly different rates.
//
//  Because it implements ISortAlgorithm, the UI code does NOT need to change
//  when you later swap these out for the real Bubble/Insertion/Quick/Merge/
//  Shell sorts.
// ============================================================================

#include "SortAlgorithm.h"

#include <atomic>
#include <chrono>
#include <random>
#include <string>
#include <thread>
#include <vector>

namespace pttk
{
    class FakeSort : public ISortAlgorithm
    {
    public:
        FakeSort(std::wstring name, double speed)
            : m_name(std::move(name)), m_speed(speed) {}

        std::wstring name() const override { return m_name; }

        void sort(std::vector<int>& data,
                  SortMetrics& m,
                  const std::atomic<bool>& stop) override
        {
            std::mt19937 rng{ std::random_device{}() };
            std::uniform_int_distribution<int> step(1, 40);

            // Pretend each algorithm needs a different total amount of work,
            // scaled by the input size so the slider still matters later.
            const long long target =
                static_cast<long long>(data.size()) * 12
                + static_cast<long long>(40000 * m_speed);

            while (!stop.load() && m.comparisons.load() < target)
            {
                const int c = static_cast<int>(step(rng) * m_speed) + 1;
                const int s = static_cast<int>(c * 0.4);

                m.comparisons.fetch_add(c);
                m.swaps.fetch_add(s);

                std::this_thread::sleep_for(std::chrono::milliseconds(8));
            }

            m.finished.store(true);
        }

    private:
        std::wstring m_name;
        double       m_speed;   // bigger = grows faster
    };
}
