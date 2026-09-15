#pragma once
#include "UniquePtr.h"
#include <cstddef>
#include <type_traits>
#include <utility>

template <typename T>
class SharedPtr
{
    template <typename U>
    friend class SharedPtr;

private:
    T *ptr_;
    size_t *refCount_;

    void release_()
    {
        if (refCount_ && --(*refCount_) == 0)
        {
            delete ptr_;
            delete refCount_;
        }
        ptr_ = nullptr;
        refCount_ = nullptr;
    }

public:
    SharedPtr(T *p = nullptr)
        : ptr_(p), refCount_(p ? new size_t(1) : nullptr) {}
    SharedPtr(std::nullptr_t) noexcept : ptr_(nullptr), refCount_(nullptr) {}

    SharedPtr(const SharedPtr &other) noexcept
        : ptr_(other.ptr_), refCount_(other.refCount_)
    {
        if (refCount_)
            ++(*refCount_);
    }

    SharedPtr &operator=(const SharedPtr &other)
    {
        if (this != &other)
        {
            if (other.refCount_)
                ++(*other.refCount_);
            release_();
            ptr_ = other.ptr_;
            refCount_ = other.refCount_;
        }
        return *this;
    }

    SharedPtr(SharedPtr &&other) noexcept
        : ptr_(other.ptr_), refCount_(other.refCount_)
    {
        other.ptr_ = nullptr;
        other.refCount_ = nullptr;
    }

    SharedPtr &operator=(SharedPtr &&other) noexcept
    {
        if (this != &other)
        {
            release_();
            ptr_ = other.ptr_;
            refCount_ = other.refCount_;
            other.ptr_ = nullptr;
            other.refCount_ = nullptr;
        }
        return *this;
    }

    template <typename U,
              typename = std::enable_if_t<std::is_convertible_v<U *, T *>>>
    SharedPtr(const SharedPtr<U> &other) noexcept
        : ptr_(other.ptr_), refCount_(other.refCount_)
    {
        if (refCount_)
            ++(*refCount_);
    }

    template <typename U,
              typename = std::enable_if_t<std::is_convertible_v<U *, T *>>>
    SharedPtr &operator=(const SharedPtr<U> &other)
    {
        if (other.refCount_)
            ++(*other.refCount_);
        release_();
        ptr_ = other.ptr_;
        refCount_ = other.refCount_;
        return *this;
    }

    template <typename U,
              typename = std::enable_if_t<std::is_convertible_v<U *, T *>>>
    SharedPtr(SharedPtr<U> &&other) noexcept
        : ptr_(other.ptr_), refCount_(other.refCount_)
    {
        other.ptr_ = nullptr;
        other.refCount_ = nullptr;
    }

    template <typename U,
              typename = std::enable_if_t<std::is_convertible_v<U *, T *>>>
    SharedPtr(UniquePtr<U> &&other)
        : ptr_(other.release()), refCount_(ptr_ ? new size_t(1) : nullptr) {}

    ~SharedPtr() { release_(); }

    T &operator*() const { return *ptr_; }
    T *operator->() const { return ptr_; }
    T *get() const { return ptr_; }

    size_t use_count() const noexcept
    {
        return refCount_ ? *refCount_ : 0;
    }

    explicit operator bool() const noexcept { return ptr_ != nullptr; }

    void reset(T *p = nullptr)
    {
        release_();
        if (p)
        {
            ptr_ = p;
            refCount_ = new size_t(1);
        }
    }

    void swap(SharedPtr &other) noexcept
    {
        std::swap(ptr_, other.ptr_);
        std::swap(refCount_, other.refCount_);
    }
};

template <typename T>
class SharedPtr<T[]>
{
    template <typename U>
    friend class SharedPtr;

private:
    T *ptr_;
    size_t *refCount_;

    void release_()
    {
        if (refCount_ && --(*refCount_) == 0)
        {
            delete[] ptr_;
            delete refCount_;
        }
        ptr_ = nullptr;
        refCount_ = nullptr;
    }

public:
    SharedPtr(T *p = nullptr)
        : ptr_(p), refCount_(p ? new size_t(1) : nullptr) {}
    SharedPtr(std::nullptr_t) noexcept : ptr_(nullptr), refCount_(nullptr) {}

    SharedPtr(const SharedPtr &other) noexcept
        : ptr_(other.ptr_), refCount_(other.refCount_)
    {
        if (refCount_)
            ++(*refCount_);
    }

    SharedPtr &operator=(const SharedPtr &other)
    {
        if (this != &other)
        {
            if (other.refCount_)
                ++(*other.refCount_);
            release_();
            ptr_ = other.ptr_;
            refCount_ = other.refCount_;
        }
        return *this;
    }

    SharedPtr(SharedPtr &&other) noexcept
        : ptr_(other.ptr_), refCount_(other.refCount_)
    {
        other.ptr_ = nullptr;
        other.refCount_ = nullptr;
    }

    SharedPtr &operator=(SharedPtr &&other) noexcept
    {
        if (this != &other)
        {
            release_();
            ptr_ = other.ptr_;
            refCount_ = other.refCount_;
            other.ptr_ = nullptr;
            other.refCount_ = nullptr;
        }
        return *this;
    }

    ~SharedPtr() { release_(); }

    T &operator[](size_t i) const { return ptr_[i]; }
    T *get() const { return ptr_; }
    size_t use_count() const noexcept { return refCount_ ? *refCount_ : 0; }
    explicit operator bool() const noexcept { return ptr_ != nullptr; }

    void reset(T *p = nullptr)
    {
        release_();
        if (p)
        {
            ptr_ = p;
            refCount_ = new size_t(1);
        }
    }
};