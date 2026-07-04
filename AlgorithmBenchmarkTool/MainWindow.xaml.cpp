#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include "DataGenerator.h"
#include "BubbleInsertionSort.h"
#include "MoreSorts.h"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

using namespace winrt;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::Graphics::Canvas;
using namespace winrt::Microsoft::Graphics::Canvas::UI::Xaml;

namespace
{
    winrt::Windows::UI::Color Rgb(uint8_t r, uint8_t g, uint8_t b)
    {
        winrt::Windows::UI::Color c{};
        c.A = 255; c.R = r; c.G = g; c.B = b;
        return c;
    }
}

namespace winrt::AlgorithmBenchmarkTool::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();

        SizeSlider().ValueChanged(
            [this](auto const&, auto const&)
            {
                int v = static_cast<int>(SizeSlider().Value());
                SizeText().Text(L"Size: " + winrt::to_hstring(v));
            });

        auto dq = Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
        m_timer = dq.CreateTimer();
        m_timer.Interval(std::chrono::milliseconds(50));
        m_timer.Tick([this](auto const&, auto const&) { OnTick(); });
    }

    MainWindow::~MainWindow()
    {
        StopRun();
    }

    void MainWindow::RunButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (m_running) return;
        StartRun();
    }

    void MainWindow::StopButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (!m_running) return;
        StopRun();
        StatusText().Text(L"Stopped");
    }

    void MainWindow::StartRun()
    {
        StopRun();

        m_channels.clear();
        m_control.cancel.store(false);
        m_control.pause.store(false);
        m_maxComparisons = 1;
        m_maxSwaps = 1;

        // Doc che do hien thi (chi doc luc bam RUN, khong doi giua chung).
        m_autoScale = AutoScaleBox().IsChecked().Value();
        m_slowMo    = SlowMoBox().IsChecked().Value();

        const int size = static_cast<int>(SizeSlider().Value());
        int typeIndex = DataTypeBox().SelectedIndex();
        if (typeIndex < 0) typeIndex = 0;

        const std::vector<int> original = DataGen::Make(typeIndex, size);

        // Hien loai du lieu + kich thuoc dang chay len goc chart (tu giai thich khi demo).
        const wchar_t* typeNames[] = { L"Random", L"Sorted", L"Reversed", L"Partial" };
        const wchar_t* typeName = typeNames[(typeIndex >= 0 && typeIndex < 4) ? typeIndex : 0];
        DataInfoText().Text(L"Data: " + winrt::hstring{ typeName } +
                            L"   n=" + winrt::to_hstring(size));

        struct Entry {
            bool checked; std::wstring name; std::wstring bigO; uint8_t r, g, b;
            std::function<Sorting::SortMetrics(std::vector<int>&, Sorting::ProgressCallback,
                                               Sorting::SortControl*, int)> fn;
        };
        std::vector<Entry> entries = {
            { BubbleBox().IsChecked().Value(),    L"Bubble",    L"O(n^2)",      220,  60,  60, &Sorting::BubbleSort },
            { InsertionBox().IsChecked().Value(), L"Insertion", L"O(n^2)",       60, 140, 220, &Sorting::InsertionSort },
            { ShellBox().IsChecked().Value(),     L"Shell",     L"O(n log^2 n)", 40, 170,  90, &Sorting::ShellSort },
            { MergeBox().IsChecked().Value(),     L"Merge",     L"O(n log n)",  200, 140,  40, &Sorting::MergeSort },
            { QuickBox().IsChecked().Value(),     L"Quick",     L"O(n log n)",  150,  80, 200, &Sorting::QuickSort },
        };

        for (auto& e : entries)
        {
            if (!e.checked) continue;
            auto ch = std::make_unique<Channel>();
            ch->name = e.name;
            ch->complexity = e.bigO;
            ch->color = Rgb(e.r, e.g, e.b);
            ch->run = e.fn;
            ch->histComparisons.reserve(kMaxPoints);
            ch->histSwaps.reserve(kMaxPoints);
            m_channels.push_back(std::move(ch));
        }

        if (m_channels.empty())
        {
            StatusText().Text(L"Hay chon it nhat mot thuat toan.");
            return;
        }

        const bool slowMo = m_slowMo;
        // Slow-mo: goi callback day hon + nghi mot chut de co nhieu diem mau.
        const int period = slowMo ? (std::max)(1, size / 400)
                                  : (std::max)(50, size / 50);

        for (auto& chPtr : m_channels)
        {
            Channel* ch = chPtr.get();
            std::vector<int> data = original;   // ban copy rieng

            ch->worker = std::thread(
                [ch, data = std::move(data), period, slowMo, this]() mutable
                {
                    Sorting::ProgressCallback cb =
                        [ch, slowMo](unsigned long long cmp, unsigned long long sw,
                                     const std::vector<int>&, bool /*done*/)
                        {
                            ch->comparisons.store(cmp);
                            ch->swaps.store(sw);
                            // Moi lan callback duoc goi -> luu MOT diem vao pending.
                            // Nho vay so diem = so lan callback (theo so phep toan),
                            // KHONG phu thuoc timer UI. Data nho van co du diem de ve.
                            {
                                std::lock_guard<std::mutex> lk(ch->sampleMutex);
                                ch->pendingC.push_back(static_cast<float>(cmp));
                                ch->pendingS.push_back(static_cast<float>(sw));
                            }
                            if (slowMo)
                                std::this_thread::sleep_for(std::chrono::milliseconds(3));
                        };

                    // Do wall-clock ngay tai day de co thoi gian that (micro giay),
                    // vi voi data nho durationMs cua thuat toan co the = 0.
                    auto t0 = std::chrono::high_resolution_clock::now();
                    Sorting::SortMetrics m = ch->run(data, cb, &m_control, period);
                    auto t1 = std::chrono::high_resolution_clock::now();
                    auto micros = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

                    // Chot diem cuoi cung (gia tri hoan chinh) vao pending.
                    {
                        std::lock_guard<std::mutex> lk(ch->sampleMutex);
                        ch->pendingC.push_back(static_cast<float>(m.comparisons));
                        ch->pendingS.push_back(static_cast<float>(m.swaps));
                    }

                    ch->comparisons.store(m.comparisons);
                    ch->swaps.store(m.swaps);
                    ch->durationMs.store(m.durationMs);
                    ch->durationMicros.store(static_cast<unsigned long long>(micros));
                    ch->finished.store(true);
                });
        }

        m_running = true;
        RunButton().IsEnabled(false);
        StopButton().IsEnabled(true);
        StatusText().Text(m_slowMo ? L"Running (slow-mo)..." : L"Running...");
        ResultList().Items().Clear();
        m_timer.Start();
    }

    void MainWindow::StopRun()
    {
        m_control.cancel.store(true);
        if (m_timer) m_timer.Stop();

        for (auto& ch : m_channels)
            if (ch->worker.joinable())
                ch->worker.join();

        m_running = false;
        RunButton().IsEnabled(true);
        StopButton().IsEnabled(false);
    }

    void MainWindow::OnTick()
    {
        bool allDone = true;

        for (auto& ch : m_channels)
        {
            // Gom TAT CA diem callback da day vao pending ke tu tick truoc.
            // Day moi la nguon diem chinh (khong con tu chup theo timer).
            std::vector<float> gotC, gotS;
            {
                std::lock_guard<std::mutex> lk(ch->sampleMutex);
                gotC.swap(ch->pendingC);
                gotS.swap(ch->pendingS);
            }
            for (float v : gotC) ch->histComparisons.push_back(v);
            for (float v : gotS) ch->histSwaps.push_back(v);

            // Neu qua nhieu diem thi LAM THUA DEU tren TOAN BO qua trinh
            // (decimate): bo bot 1 diem giua moi cap, giu lai khoang mot nua.
            // Cach nay giu nguyen HINH DANG tu dau den cuoi, khong bi mat khuc
            // dau nhu kieu erase phia truoc (truoc day Bubble chi con "duoi" phang).
            // hc va hs luon cung do dai nen dung chung mot chi so de 2 duong khop nhau.
            if (ch->histComparisons.size() > kMaxPoints)
            {
                std::vector<float> dc; dc.reserve(ch->histComparisons.size() / 2 + 2);
                std::vector<float> dsw; dsw.reserve(ch->histSwaps.size() / 2 + 2);
                const size_t last = ch->histComparisons.size() - 1;
                for (size_t i = 0; i < ch->histComparisons.size(); i += 2)
                {
                    dc.push_back(ch->histComparisons[i]);
                    dsw.push_back(ch->histSwaps[i]);
                }
                // Luon giu diem cuoi cung (gia tri hoan chinh) du no o vi tri le.
                if ((last % 2) != 0)
                {
                    dc.push_back(ch->histComparisons[last]);
                    dsw.push_back(ch->histSwaps[last]);
                }
                ch->histComparisons.swap(dc);
                ch->histSwaps.swap(dsw);
            }

            const unsigned long long c = ch->comparisons.load();
            const unsigned long long s = ch->swaps.load();
            if (c > m_maxComparisons) m_maxComparisons = c;
            if (s > m_maxSwaps)       m_maxSwaps = s;
            if (!ch->finished.load()) allDone = false;
        }

        SortCanvas().Invalidate();

        if (allDone)
        {
            StopRun();
            StatusText().Text(L"Done");
            FillResultTable();
        }
    }

    void MainWindow::FillResultTable()
    {
        ResultList().Items().Clear();

        // Xep hang theo thoi gian that (micro giay) de biet cai nao NHANH nhat.
        std::vector<Channel*> ranked;
        for (auto& ch : m_channels) ranked.push_back(ch.get());
        std::sort(ranked.begin(), ranked.end(),
                  [](Channel* a, Channel* b)
                  { return a->durationMicros.load() < b->durationMicros.load(); });

        int rank = 1;
        for (Channel* ch : ranked)
        {
            const unsigned long long us = ch->durationMicros.load();
            // Hien thi ms co phan thap phan de data nho khong bi lam tron ve 0.
            const double ms = static_cast<double>(us) / 1000.0;

            std::wstringstream row;
            row << L"#" << rank << L"  " << ch->name
                << L"  |  time: " << std::fixed << std::setprecision(3) << ms << L" ms";
            ResultList().Items().Append(box_value(winrt::hstring{ row.str() }));
            ++rank;
        }
    }

    void MainWindow::SortCanvas_Draw(CanvasControl const& sender,
                                     CanvasDrawEventArgs const& args)
    {
        auto ds = args.DrawingSession();

        const float W = static_cast<float>(sender.ActualWidth());
        const float H = static_cast<float>(sender.ActualHeight());
        if (W < 8 || H < 8) return;

        const size_t n = m_channels.size();
        if (n == 0) return;

        const int cols = static_cast<int>(std::ceil(std::sqrt(static_cast<double>(n))));
        const int rows = static_cast<int>(std::ceil(static_cast<double>(n) / cols));

        const float gap = 8.0f;
        const float cellW = (W - gap * (cols + 1)) / cols;
        const float cellH = (H - gap * (rows + 1)) / rows;
        if (cellW < 20 || cellH < 20) return;

        for (size_t idx = 0; idx < n; ++idx)
        {
            const int r = static_cast<int>(idx) / cols;
            const int c = static_cast<int>(idx) % cols;
            const float px = gap + c * (cellW + gap);
            const float py = gap + r * (cellH + gap);
            DrawPanel(ds, *m_channels[idx], px, py, cellW, cellH);
        }
    }

    void MainWindow::DrawPanel(CanvasDrawingSession const& ds, Channel& ch,
                               float px, float py, float cellW, float cellH)
    {
        const auto axisColor  = Rgb(200, 200, 200);
        const auto frameColor = Rgb(150, 150, 150);
        const auto swapColor  = Rgb(0, 0, 0);       // den: duong swaps

        ds.DrawRectangle(px, py, cellW, cellH, frameColor, 1.0f);

        const float padL = 6.0f, padR = 6.0f, padT = 34.0f, padB = 6.0f;
        const float plotX = px + padL;
        const float plotY = py + padT;
        const float plotW = cellW - padL - padR;
        const float plotH = cellH - padT - padB;
        if (plotW < 4 || plotH < 4) return;

        // Xac dinh thang do cho panel nay.
        float panelMax;
        if (m_autoScale)
        {
            float ownMax = 1.0f;
            for (float v : ch.histComparisons) ownMax = (std::max)(ownMax, v);
            for (float v : ch.histSwaps)       ownMax = (std::max)(ownMax, v);
            panelMax = ownMax;
        }
        else
        {
            panelMax = static_cast<float>((std::max)(m_maxComparisons, m_maxSwaps));
        }
        if (panelMax < 1.0f) panelMax = 1.0f;

        // Tieu de + so lieu + max cua panel (de khong giau thong tin do lon).
        std::wstringstream title;
        title << ch.name << L" " << ch.complexity
              << L"   cmp=" << ch.comparisons.load()
              << L" swap=" << ch.swaps.load();
        ds.DrawText(winrt::hstring{ title.str() }, px + padL, py + 4.0f, ch.color);

        std::wstringstream sub;
        sub << (m_autoScale ? L"auto max=" : L"max=")
            << static_cast<unsigned long long>(panelMax);
        ds.DrawText(winrt::hstring{ sub.str() }, px + padL, py + 19.0f, axisColor);

        // Truc
        ds.DrawLine(plotX, plotY + plotH, plotX + plotW, plotY + plotH, axisColor, 1.0f);
        ds.DrawLine(plotX, plotY, plotX, plotY + plotH, axisColor, 1.0f);

        // Truc X trai kin be ngang panel theo SO DIEM da thu (np), khong dung
        // kMaxPoints co dinh. Neu chia cho kMaxPoints thi duong it diem se bi
        // ep thanh vach doc / stub ngan ben trai -> nhin "lon xon".
        const auto& hc = ch.histComparisons;
        const auto& hs = ch.histSwaps;
        const size_t np = hc.size();      // hc va hs luon cung do dai
        if (np < 2) return;
        const float stepX = plotW / static_cast<float>(np - 1);

        // --- Ve swaps TRUOC: net DEN (1.6px) ---
        for (size_t i = 0; i + 1 < hs.size(); ++i)
        {
            const float x1 = plotX + i * stepX;
            const float x2 = plotX + (i + 1) * stepX;
            const float y1 = plotY + plotH - (hs[i]     / panelMax) * plotH;
            const float y2 = plotY + plotH - (hs[i + 1] / panelMax) * plotH;
            ds.DrawLine(x1, y1, x2, y2, swapColor, 1.6f);
        }

        // --- Ve comparisons SAU: mau panel, net MANH (2px), nam TREN ---
        for (size_t i = 0; i + 1 < hc.size(); ++i)
        {
            const float x1 = plotX + i * stepX;
            const float x2 = plotX + (i + 1) * stepX;
            const float y1 = plotY + plotH - (hc[i]     / panelMax) * plotH;
            const float y2 = plotY + plotH - (hc[i + 1] / panelMax) * plotH;
            ds.DrawLine(x1, y1, x2, y2, ch.color, 2.0f);
        }
    }
}
