#pragma once
#include "SharedPtr.hpp"
#include "Exceptions.hpp"
#include <cstddef>

template <typename T>
class Node
{
    template <class U>
    friend class LinkedList;

private:
    SharedPtr<T> data;
    Node<T> *prev;
    Node<T> *next;

    Node(const T &val) : data(new T(val)), prev(nullptr), next(nullptr) {}
};

template <typename T>
class LinkedList
{
private:
    Node<T> *head;
    Node<T> *tail;

public:
    class Iterator
    {
    private:
        Node<T> *ptr;

    public:
        Iterator(Node<T> *p = nullptr) : ptr(p) {}

        T &operator*() { return *(ptr->data); }
        const T &operator*() const { return *(ptr->data); }

        T *operator->() { return &(*(ptr->data)); }
        const T *operator->() const { return &(*(ptr->data)); }

        Iterator &operator++()
        {
            ptr = ptr->next;
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }

        bool operator==(const Iterator &other) const { return ptr == other.ptr; }
        bool operator!=(const Iterator &other) const { return ptr != other.ptr; }
    };

    LinkedList() : head(nullptr), tail(nullptr) {}

    LinkedList(T *items, std::size_t count);
    LinkedList(const LinkedList<T> &list);

    ~LinkedList()
    {
        Node<T> *temp;
        while (head != nullptr)
        {
            temp = head->next;
            delete head;
            head = temp;
        }
    }

    T GetFirst() const
    {
        if (head == nullptr)
            throw EmptyStructureException("LinkedList::GetFirst");
        return *(head->data);
    }

    T GetLast() const
    {
        if (tail == nullptr)
            throw EmptyStructureException("LinkedList::GetLast");
        return *(tail->data);
    }

    T Get(std::size_t index) const;

    SharedPtr<T> GetSharedPtr(std::size_t index) const;

    LinkedList<T> *GetSubList(std::size_t startIndex, std::size_t endIndex) const;
    std::size_t GetLength() const;

    void Append(T item);
    void Prepend(T item);
    void InsertAt(T item, std::size_t index);
    void Concat(LinkedList<T> &list);

    Iterator begin() const { return Iterator(head); }
    Iterator end() const { return Iterator(nullptr); }
};

template <typename T>
std::size_t LinkedList<T>::GetLength() const
{
    std::size_t len = 0;
    for (Iterator it = begin(); it != end(); ++it)
        ++len;
    return len;
}

template <typename T>
LinkedList<T>::LinkedList(T *items, std::size_t count) : head(nullptr), tail(nullptr)
{
    for (std::size_t i = 0; i < count; ++i)
    {
        Node<T> *newNode = new Node<T>(items[i]);
        if (!head)
            head = tail = newNode;
        else
        {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
    }
}

template <typename T>
LinkedList<T>::LinkedList(const LinkedList<T> &list) : LinkedList()
{
    for (Iterator it = list.begin(); it != list.end(); ++it)
        Append(*it);
}

template <typename T>
T LinkedList<T>::Get(std::size_t index) const
{
    std::size_t len = GetLength();
    if (index >= len)
        throw IndexOutOfRange(index, len, "LinkedList::Get");
    Node<T> *temp = head;
    for (std::size_t i = 0; i < index; ++i)
        temp = temp->next;
    return *(temp->data);
}

template <typename T>
SharedPtr<T> LinkedList<T>::GetSharedPtr(std::size_t index) const
{
    std::size_t len = GetLength();
    if (index >= len)
        throw IndexOutOfRange(index, len, "LinkedList::GetSharedPtr");
    Node<T> *temp = head;
    for (std::size_t i = 0; i < index; ++i)
        temp = temp->next;
    return temp->data; 
}

template <typename T>
LinkedList<T> *LinkedList<T>::GetSubList(std::size_t startIndex, std::size_t endIndex) const
{
    std::size_t len = GetLength();
    if (startIndex > endIndex || endIndex >= len)
        throw IndexOutOfRange(endIndex, len, "LinkedList::GetSubList");
    LinkedList<T> *result = new LinkedList<T>();
    Iterator it = begin();
    for (std::size_t i = 0; i < startIndex; ++i)
        ++it;
    for (std::size_t i = startIndex; i <= endIndex; ++i)
    {
        result->Append(*it);
        ++it;
    }
    return result;
}

template <typename T>
void LinkedList<T>::Append(T item)
{
    Node<T> *newTail = new Node<T>(item);
    if (head == nullptr)
        head = tail = newTail;
    else
    {
        tail->next = newTail;
        newTail->prev = tail;
        tail = newTail;
    }
}

template <typename T>
void LinkedList<T>::Prepend(T item)
{
    Node<T> *newHead = new Node<T>(item);
    if (head == nullptr)
        head = tail = newHead;
    else
    {
        head->prev = newHead;
        newHead->next = head;
        head = newHead;
    }
}

template <typename T>
void LinkedList<T>::InsertAt(T item, std::size_t index)
{
    std::size_t len = GetLength();
    if (index > len)
        throw IndexOutOfRange(index, len + 1, "LinkedList::InsertAt");
    if (index == 0)
    {
        Prepend(item);
        return;
    }
    if (index == len)
    {
        Append(item);
        return;
    }
    Node<T> *current = head;
    for (std::size_t i = 0; i < index; ++i)
        current = current->next;
    Node<T> *newNode = new Node<T>(item);
    newNode->next = current;
    newNode->prev = current->prev;
    current->prev->next = newNode;
    current->prev = newNode;
}

template <typename T>
void LinkedList<T>::Concat(LinkedList<T> &list)
{
    if (list.head == nullptr)
        return;
    if (head == nullptr)
    {
        head = list.head;
        tail = list.tail;
    }
    else
    {
        tail->next = list.head;
        list.head->prev = tail;
        tail = list.tail;
    }
    list.head = list.tail = nullptr;
}