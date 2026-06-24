#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::AlgorithmBenchmarkTool::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
    }

    int32_t MainWindow::MyProperty()
    {
        return m_myProperty;
    }

    void MainWindow::MyProperty(int32_t value)
    {
        m_myProperty = value;
    }

    void MainWindow::RunButton_Click(
        IInspectable const&,
        RoutedEventArgs const&)
    {
        ResultList().Items().Clear();

        if (BubbleBox().IsChecked().Value())
        {
            ResultList().Items().Append(
                box_value(L"Bubble Sort"));
        }

        if (InsertionBox().IsChecked().Value())
        {
            ResultList().Items().Append(
                box_value(L"Insertion Sort"));
        }

        if (QuickBox().IsChecked().Value())
        {
            ResultList().Items().Append(
                box_value(L"Quick Sort"));
        }

        if (MergeBox().IsChecked().Value())
        {
            ResultList().Items().Append(
                box_value(L"Merge Sort"));
        }

        if (ShellBox().IsChecked().Value())
        {
            ResultList().Items().Append(
                box_value(L"Shell Sort"));
        }
    }
}