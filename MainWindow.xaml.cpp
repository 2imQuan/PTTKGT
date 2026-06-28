#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include "FakeSort.h"

#include <algorithm>
#include <sstream>

#include <winrt/Microsoft.UI.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Windows.Foundation.h>

using namespace winrt;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::Graphics::Canvas;
using namespace winrt::Microsoft::Graphics::Canvas::UI::Xaml;

namespace
{
    // Distinct colors for up to 5 (or more) algorithms.
    winrt::Windows::UI::Color MakeColor(uint8_t r, uint8_t g, uint8_t b)
    {
        return winrt::Windows::UI::ColorHelper::FromArgb(255, r, g, b);
    }
}

namespace winrt::PTTK::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
        BuildChannels();

        // Timer lives on the UI thread; its Tick fires on the UI thread,
        // so touching the canvas / text from OnTick is safe.
        auto dq = Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
        m_timer = dq.CreateTimer();
        m_timer.Interval(std::chrono::milliseconds(50));
        m_timer.Tick([this](auto const&, auto const&) { OnTick(); });
    }

    void MainWindow::BuildChannels()
    {
        // Swap these 5 FakeSort objects for the real algorithms later.
        // The rest of the file does not need to change.
        struct Def { const wchar_t* name; double speed; uint8_t r, g, b; };
        const Def defs[] = {
            { L"Bubble",    0.6, 220,  60,  60 },
            { L"Insertion", 0.9,  60, 140, 220 },
            { L"Shell",     1.6,  40, 170,  90 },
            { L"Merge",     2.4, 200, 140,  40 },
            { L"Quick",     3.0, 150,  80, 200 },
        };

        for (auto const& d : defs)
        {
            auto ch = std::make_unique<Channel>();
            ch->algo = std::make_unique<pttk::FakeSort>(d.name, d.speed);
            ch->color = MakeColor(d.r, d.g, d.b);
            ch->history.reserve(kMaxPoints);
            m_channels.push_back(std::move(ch));
        }
    }

    void MainWindow::OnRunClick(IInspectable const&, RoutedEventArgs const&)
    {
        if (m_running) return;
        StartRun();
    }

    void MainWindow::OnStopClick(IInspectable const&, RoutedEventArgs const&)
    {
        if (!m_running) return;
        StopRun();
        StatusText().Text(L"Stopped");
    }

    void MainWindow::StartRun()
    {
        // Make sure any previous run is fully stopped before restarting.
        StopRun();

        m_stop.store(false);
        m_maxValue = 1;

        // The shared "input data". Each algorithm gets its OWN copy so the
        // threads never touch the same array (correct multithreading).
        std::vector<int> original(2000);
        for (size_t i = 0; i < original.size(); ++i)
            original[i] = static_cast<int>(original.size() - i); // reversed

        for (auto& ch : m_channels)
        {
            ch->metrics.reset();
            ch->history.clear();

            pttk::ISortAlgorithm* algo = ch->algo.get();
            pttk::SortMetrics* metrics = &ch->metrics;
            std::vector<int> data = original;   // private copy

            ch->worker = std::thread(
                [algo, metrics, data = std::move(data), this]() mutable {
                    algo->sort(data, *metrics, m_stop);
                });
        }

        m_running = true;
        RunButton().IsEnabled(false);
        StopButton().IsEnabled(true);
        StatusText().Text(L"Running...");
        m_timer.Start();
    }

    void MainWindow::StopRun()
    {
        m_stop.store(true);
        if (m_timer) m_timer.Stop();

        for (auto& ch : m_channels)
        {
            if (ch->worker.joinable())
                ch->worker.join();
        }

        m_running = false;
        RunButton().IsEnabled(true);
        StopButton().IsEnabled(false);
    }

    void MainWindow::OnTick()
    {
        bool allDone = true;
        std::wstringstream legend;

        for (auto& ch : m_channels)
        {
            const long long c = ch->metrics.comparisons.load();
            const long long s = ch->metrics.swaps.load();

            ch->history.push_back(static_cast<float>(c));
            if (ch->history.size() > kMaxPoints)
                ch->history.erase(ch->history.begin());

            if (c > m_maxValue) m_maxValue = c;
            if (!ch->metrics.finished.load()) allDone = false;

            legend << ch->algo->name() << L": cmp=" << c
                   << L" swap=" << s << L"   ";
        }

        LegendText().Text(legend.str());
        SortCanvas().Invalidate();

        if (allDone)
        {
            StopRun();
            StatusText().Text(L"Done");
        }
    }

    void MainWindow::SortCanvas_Draw(CanvasControl const& sender,
                                     CanvasDrawEventArgs const& args)
    {
        auto ds = args.DrawingSession();

        const float w = static_cast<float>(sender.ActualWidth());
        const float h = static_cast<float>(sender.ActualHeight());
        if (w < 4 || h < 4) return;

        const float pad = 12.0f;
        const float maxVal = static_cast<float>(std::max<long long>(m_maxValue, 1));

        // Draw each algorithm as its own line.
        for (auto& ch : m_channels)
        {
            const auto& hist = ch->history;
            if (hist.size() < 2) continue;

            const float stepX =
                (w - 2 * pad) / static_cast<float>(kMaxPoints - 1);

            for (size_t i = 0; i + 1 < hist.size(); ++i)
            {
                const float x1 = pad + i * stepX;
                const float x2 = pad + (i + 1) * stepX;
                const float y1 = h - pad - (hist[i]     / maxVal) * (h - 2 * pad);
                const float y2 = h - pad - (hist[i + 1] / maxVal) * (h - 2 * pad);
                ds.DrawLine(x1, y1, x2, y2, ch->color, 2.0f);
            }
        }
    }
}
