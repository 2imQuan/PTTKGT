#pragma once

#include "MainWindow.g.h"

namespace winrt::AlgorithmBenchmarkTool::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void RunButton_Click(
            Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& args);

    private:
        int32_t m_myProperty{};
    };
}

namespace winrt::AlgorithmBenchmarkTool::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}