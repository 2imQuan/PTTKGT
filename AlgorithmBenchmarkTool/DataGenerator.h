#pragma once

// ============================================================================
//  DataGenerator: chuan hoa 4 ham sinh du lieu cua Nguoi 4.
//  (Goc: MergeSort.cpp o thu muc repo - da doi ten cho dung chuc nang.)
//
//  Tat ca thuat toan chay tren CUNG mot mang goc, moi thuat toan mot ban copy.
//  DataType khop thu tu ComboBox cua Nguoi 5: 0=Random 1=Sorted 2=Reversed 3=Partial
// ============================================================================

#include <vector>
#include <cstdlib>
#include <algorithm>

namespace DataGen
{
    enum class DataType : int
    {
        Random   = 0,
        Sorted   = 1,
        Reversed = 2,
        Partial  = 3
    };

    inline std::vector<int> GenerateRandom(int n)
    {
        std::vector<int> a;
        a.reserve(n);
        for (int i = 0; i < n; ++i)
            a.push_back(std::rand() % 100000);
        return a;
    }

    inline std::vector<int> GenerateSorted(int n)
    {
        std::vector<int> a;
        a.reserve(n);
        for (int i = 1; i <= n; ++i)
            a.push_back(i);
        return a;
    }

    inline std::vector<int> GenerateReversed(int n)
    {
        std::vector<int> a;
        a.reserve(n);
        for (int i = n; i >= 1; --i)
            a.push_back(i);
        return a;
    }

    // Gan sap xep: mang tang dan roi hoan doi ngau nhien n/10 cap.
    inline std::vector<int> GeneratePartial(int n)
    {
        std::vector<int> a = GenerateSorted(n);
        if (n <= 1) return a;

        int swaps = n / 10;
        for (int i = 0; i < swaps; ++i)
        {
            int x = std::rand() % n;
            int y = std::rand() % n;
            std::swap(a[x], a[y]);
        }
        return a;
    }

    inline std::vector<int> Make(DataType type, int n)
    {
        if (n < 0) n = 0;
        switch (type)
        {
        case DataType::Sorted:   return GenerateSorted(n);
        case DataType::Reversed: return GenerateReversed(n);
        case DataType::Partial:  return GeneratePartial(n);
        case DataType::Random:
        default:                 return GenerateRandom(n);
        }
    }

    inline std::vector<int> Make(int typeIndex, int n)
    {
        return Make(static_cast<DataType>(typeIndex), n);
    }
}
