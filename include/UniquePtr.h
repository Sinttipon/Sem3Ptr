#pragma once
#include <cstddef>
#include <type_traits>
#include <utility>
template <typename T>
class UniquePtr
{
    template <typename U>
    friend class UniquePtr;

private:
    T *ptr_;

public:
    ~UniquePtr() { delete ptr_; }

    UniquePtr(const UniquePtr &) = delete;
    UniquePtr &operator=(const UniquePtr &) = delete;

    UniquePtr(UniquePtr &&other) noexcept : ptr_(other.ptr_)
    {
        other.ptr_ = nullptr;
    }

    UniquePtr &operator=(UniquePtr &&other) noexcept
    {
        if (this != &other)
        {
            delete ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    template <typename U,
              typename = std::enable_if_t<std::is_convertible_v<U *, T *>>>
    UniquePtr(UniquePtr<U> &&other) noexcept : ptr_(other.ptr_)
    {
        other.ptr_ = nullptr;
    }

    template <typename U,
              typename = std::enable_if_t<std::is_convertible_v<U *, T *>>>
    UniquePtr &operator=(UniquePtr<U> &&other) noexcept
    {
        delete ptr_;
        ptr_ = other.ptr_;
        other.ptr_ = nullptr;
        return *this;
    }

    T &operator*() const { return *ptr_; }
    T *operator->() const { return ptr_; }
    T *get() const { return ptr_; }

    explicit operator bool() const noexcept { return ptr_ != nullptr; }

    T *release() noexcept
    {
        T *tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }

    void reset(T *p = nullptr)
    {
        if (ptr_ != p)
        {
            delete ptr_;
            ptr_ = p;
        }
    }

    void swap(UniquePtr &other) noexcept
    {
        std::swap(ptr_, other.ptr_);
    }
};

template <typename T>
class UniquePtr<T[]>
{
    template <typename U>
    friend class UniquePtr;

private:
    T *ptr_;

public:
    ~UniquePtr() { delete[] ptr_; }

    UniquePtr(const UniquePtr &) = delete;
    UniquePtr &operator=(const UniquePtr &) = delete;

    UniquePtr(UniquePtr &&other) noexcept : ptr_(other.ptr_)
    {
        other.ptr_ = nullptr;
    }

    UniquePtr &operator=(UniquePtr &&other) noexcept
    {
        if (this != &other)
        {
            delete[] ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    T &operator[](size_t i) const { return ptr_[i]; }
    T *get() const { return ptr_; }

    explicit operator bool() const noexcept { return ptr_ != nullptr; }

    T *release() noexcept
    {
        T *tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }

    void reset(T *p = nullptr)
    {
        if (ptr_ != p)
        {
            delete[] ptr_;
            ptr_ = p;
        }
    }
};