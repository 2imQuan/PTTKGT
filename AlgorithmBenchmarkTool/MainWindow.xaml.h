#pragma once

#include "MainWindow.g.h"
#include "BubbleInsertionSort.h"   // Sorting::SortMetrics / ProgressCallback / SortControl

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace winrt::AlgorithmBenchmarkTool::implementation
{
    // Mot thuat toan + du lieu rieng de chay va ve.
    // Bo dem la atomic: thread nen (callback) ghi, UI thread (timer/draw) doc.
    struct Channel
    {
        std::wstring name;
        std::wstring complexity;                 // Big-O de hien tren tieu de panel
        winrt::Windows::UI::Color color{};

        std::atomic<unsigned long long> comparisons{ 0 };
        std::atomic<unsigned long long> swaps{ 0 };
        std::atomic<unsigned long long> durationMs{ 0 };
        std::atomic<unsigned long long> durationMicros{ 0 };   // do wall-clock chinh xac hon
        std::atomic<bool>               finished{ false };

        // Lich su mau de VE (chi UI thread cham vao, sau khi gom tu pending).
        std::vector<float> histComparisons;   // duong mau
        std::vector<float> histSwaps;          // duong den

        // Buffer trung gian: callback (thread nen) GHI, UI thread GOM ra hist.
        // Nho vay du thuat toan chay xong trong 1ms van luu du diem de ve.
        std::mutex         sampleMutex;
        std::vector<float> pendingC;
        std::vector<float> pendingS;

        std::thread        worker;

        // Ham chay that (Bubble/Insertion/Quick/Merge/Shell) theo contract Nguoi 2.
        std::function<Sorting::SortMetrics(
            std::vector<int>&, Sorting::ProgressCallback,
            Sorting::SortControl*, int)> run;
    };

    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();
        ~MainWindow();

        void RunButton_Click(
            winrt::Windows::Foundation::IInspectable const& sender,
            winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void StopButton_Click(
            winrt::Windows::Foundation::IInspectable const& sender,
            winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);

        void SortCanvas_Draw(
            winrt::Microsoft::Graphics::Canvas::UI::Xaml::CanvasControl const& sender,
            winrt::Microsoft::Graphics::Canvas::UI::Xaml::CanvasDrawEventArgs const& args);

    private:
        void OnTick();          // UI thread: doc atomic -> history -> ve + bang
        void StartRun();
        void StopRun();
        void FillResultTable(); // cap nhat ListView khi xong
        void DrawPanel(
            winrt::Microsoft::Graphics::Canvas::CanvasDrawingSession const& ds,
            Channel& ch, float px, float py, float cellW, float cellH);

        static constexpr size_t kMaxPoints = 300;

        std::vector<std::unique_ptr<Channel>> m_channels;
        Sorting::SortControl                  m_control;   // cancel/pause dung chung
        bool                                  m_running{ false };

        // Thang do chung cho moi panel (che do "cung thang do").
        unsigned long long m_maxComparisons{ 1 };
        unsigned long long m_maxSwaps{ 1 };

        // Cac che do hien thi (doc tu checkbox luc bam RUN).
        bool m_autoScale{ false };
        bool m_slowMo{ false };

        winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer m_timer{ nullptr };
    };
}

namespace winrt::AlgorithmBenchmarkTool::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
