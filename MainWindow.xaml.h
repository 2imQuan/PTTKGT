#pragma once

#include "MainWindow.g.h"
#include "SortAlgorithm.h"

#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <winrt/Microsoft.Graphics.Canvas.UI.Xaml.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Windows.UI.h>

namespace winrt::PTTK::implementation
{
    // One algorithm + its private data needed to run and draw it.
    struct Channel
    {
        std::unique_ptr<pttk::ISortAlgorithm> algo;
        pttk::SortMetrics                      metrics;     // updated by worker
        winrt::Windows::UI::Color              color{};
        std::vector<float>                     history;     // sampled comparisons
        std::thread                            worker;
    };

    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        void OnRunClick(winrt::Windows::Foundation::IInspectable const& sender,
                        winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void OnStopClick(winrt::Windows::Foundation::IInspectable const& sender,
                         winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

        void SortCanvas_Draw(
            winrt::Microsoft::Graphics::Canvas::UI::Xaml::CanvasControl const& sender,
            winrt::Microsoft::Graphics::Canvas::UI::Xaml::CanvasDrawEventArgs const& args);

    private:
        void BuildChannels();   // create the 5 fake algorithms
        void StartRun();
        void StopRun();         // signals stop + joins all worker threads
        void OnTick();          // sample atomics -> history -> invalidate

        // How many samples to keep on screen (scrolling window).
        static constexpr size_t kMaxPoints = 240;

        std::vector<std::unique_ptr<Channel>> m_channels;
        std::atomic<bool>                     m_stop{ false };
        bool                                  m_running{ false };
        long long                             m_maxValue{ 1 };  // Y auto-scale

        winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer m_timer{ nullptr };
    };
}

namespace winrt::PTTK::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
