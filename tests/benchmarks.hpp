#pragma once
#include "UniquePtr.hpp"
#include "SharedPtr.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <fstream>
#include <memory>
#include <string>
#include <functional>

struct BenchResult
{
    std::string name;
    size_t n;
    double create_ms;
    double copy_ms;
    double destroy_ms;
    double total_ms;
    size_t overhead_bytes;
};

inline double measure_ms(std::function<void()> f)
{
    auto t0 = std::chrono::high_resolution_clock::now();
    f();
    auto t1 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(t1 - t0).count();
}

inline BenchResult bench_raw(size_t n)
{
    BenchResult r{"Сырой указатель", n, 0, 0, 0, 0, 0};
    std::vector<int *> v(n);

    r.create_ms = measure_ms([&]()
                             { 
        for (size_t i = 0; i < n; ++i) 
            v[i] = new int((int)i); });

    r.copy_ms = measure_ms([&]()
                           { std::vector<int *> v2 = v; });

    r.destroy_ms = measure_ms([&]()
                              { 
        for (size_t i = 0; i < n; ++i) 
            delete v[i]; });

    r.total_ms = r.create_ms + r.copy_ms + r.destroy_ms;
    return r;
}

inline BenchResult bench_unique(size_t n)
{
    BenchResult r{"Наш UniquePtr", n, 0, 0, 0, 0, sizeof(void *)};
    std::vector<UniquePtr<int>> v(n);

    r.create_ms = measure_ms([&]()
                             { 
        for (size_t i = 0; i < n; ++i) 
            v[i] = UniquePtr<int>(new int((int)i)); });

    r.copy_ms = measure_ms([&]()
                           { 
        std::vector<UniquePtr<int>> v2(n); 
        for (size_t i = 0; i < n; ++i) 
            v2[i] = std::move(v[i]); });

    r.destroy_ms = measure_ms([&]()
                              { v.clear(); });

    r.total_ms = r.create_ms + r.copy_ms + r.destroy_ms;
    return r;
}

inline BenchResult bench_shared(size_t n)
{
    BenchResult r{"Наш SharedPtr", n, 0, 0, 0, 0, sizeof(void *) + sizeof(size_t)};
    std::vector<SharedPtr<int>> v(n);

    r.create_ms = measure_ms([&]()
                             { 
        for (size_t i = 0; i < n; ++i) 
            v[i] = SharedPtr<int>(new int((int)i)); });

    r.copy_ms = measure_ms([&]()
                           { 
        std::vector<SharedPtr<int>> v2(n); 
        for (size_t i = 0; i < n; ++i) 
            v2[i] = v[i]; });

    r.destroy_ms = measure_ms([&]()
                              { v.clear(); });

    r.total_ms = r.create_ms + r.copy_ms + r.destroy_ms;
    return r;
}

inline BenchResult bench_stl_shared(size_t n)
{
    BenchResult r{"STL shared_ptr", n, 0, 0, 0, 0, sizeof(void *) * 2 + sizeof(size_t) * 2};
    std::vector<std::shared_ptr<int>> v(n);

    r.create_ms = measure_ms([&]()
                             { 
        for (size_t i = 0; i < n; ++i) 
            v[i] = std::make_shared<int>((int)i); });

    r.copy_ms = measure_ms([&]()
                           { 
        std::vector<std::shared_ptr<int>> v2(n); 
        for (size_t i = 0; i < n; ++i) 
            v2[i] = v[i]; });

    r.destroy_ms = measure_ms([&]()
                              { v.clear(); });

    r.total_ms = r.create_ms + r.copy_ms + r.destroy_ms;
    return r;
}

inline void run_benchmarks()
{
    std::cout << "\n=== ЗАПУСК НАГРУЗОЧНЫХ ТЕСТОВ ===\n";

    std::vector<size_t> sizes = {1000, 10000, 100000, 1000000};
    std::vector<BenchResult> results;

    for (size_t n : sizes)
    {
        std::cout << "  Обработка N = " << n << " ...\n";
        results.push_back(bench_raw(n));
        results.push_back(bench_unique(n));
        results.push_back(bench_shared(n));
        results.push_back(bench_stl_shared(n));
    }

    std::cout << "\n"
              << std::left
              << std::setw(12) << "N"
              << std::setw(18) << "Тип указателя"
              << std::setw(14) << "Создание(мс)"
              << std::setw(14) << "Копирование(мс)"
              << std::setw(14) << "Удаление(мс)"
              << std::setw(14) << "Итого(мс)"
              << std::setw(16) << "Накладные(Б/эл)"
              << "\n"
              << std::string(100, '-') << "\n";

    for (auto &r : results)
    {
        std::cout << std::setw(12) << r.n
                  << std::setw(18) << r.name
                  << std::fixed << std::setprecision(2)
                  << std::setw(14) << r.create_ms
                  << std::setw(14) << r.copy_ms
                  << std::setw(14) << r.destroy_ms
                  << std::setw(14) << r.total_ms
                  << std::setw(16) << r.overhead_bytes
                  << "\n";
    }

    std::ofstream csv("benchmarks.csv");
    csv << "N,Type,Create_ms,Copy_ms,Destroy_ms,Total_ms,Overhead_bytes\n";
    for (auto &r : results)
    {
        csv << r.n << "," << r.name << ","
            << r.create_ms << "," << r.copy_ms << ","
            << r.destroy_ms << "," << r.total_ms << ","
            << r.overhead_bytes << "\n";
    }
    std::cout << "\n[УСПЕХ] Результаты сохранены в файл benchmarks.csv\n";
}