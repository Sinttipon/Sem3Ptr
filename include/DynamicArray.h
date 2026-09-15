#pragma once
#include "UniquePtr.h"
#include "Exceptions.hpp"
#include <cstddef>
#include <initializer_list>

template <typename T>
class DynamicArray
{
private:
    UniquePtr<T[]> data;
    std::size_t size;

public:
    using iterator = T *;
    using const_iterator = const T *;

    DynamicArray() : data(nullptr), size(0) {}

    DynamicArray(std::size_t size) : size(size)
    {
        data = UniquePtr<T[]>(new T[size]());
    }

    DynamicArray(T *items, std::size_t count)
    {
        if (items == nullptr && count > 0)
            throw InvalidArgument("DynamicArray::DynamicArray: нулевой указатель на элементы");
        size = count;
        data = UniquePtr<T[]>(new T[size]);
        for (std::size_t i = 0; i < count; ++i)
            data[i] = items[i];
    }

    DynamicArray(const DynamicArray<T> &other) : size(other.size)
    {
        data = UniquePtr<T[]>(new T[size]);
        for (std::size_t i = 0; i < size; ++i)
            data[i] = other.data[i];
    }

    DynamicArray(std::initializer_list<T> list) : size(list.size())
    {
        data = UniquePtr<T[]>(new T[size]);
        std::size_t i = 0;
        for (const T &value : list)
        {
            data[i++] = value;
        }
    }

    DynamicArray<T> &operator=(const DynamicArray<T> &other)
    {
        if (this != &other)
        {
            data = UniquePtr<T[]>(new T[other.size]);
            size = other.size;
            for (std::size_t i = 0; i < size; ++i)
                data[i] = other.data[i];
        }
        return *this;
    }

    ~DynamicArray() = default; 

    T Get(std::size_t index) const
    {
        if (index >= size)
            throw IndexOutOfRange(index, size, "DynamicArray::Get");
        return data[index];
    }

    std::size_t GetSize() const { return size; }

    T *GetRawData() const { return data.get(); }

    void Set(std::size_t index, T value)
    {
        if (index >= size)
            throw IndexOutOfRange(index, size, "DynamicArray::Set");
        data[index] = value;
    }

    void Resize(std::size_t newSize)
    {
        if (newSize == 0)
        {
            data.reset();
            size = 0;
            return;
        }

        UniquePtr<T[]> newData(new T[newSize]());
        std::size_t elementsToCopy = (newSize < size) ? newSize : size;
        for (std::size_t i = 0; i < elementsToCopy; ++i)
            newData[i] = data[i];

        data = std::move(newData);
        size = newSize;
    }

    iterator begin() { return data.get(); }
    iterator end() { return data.get() + size; }
    const_iterator begin() const { return data.get(); }
    const_iterator end() const { return data.get() + size; }
};