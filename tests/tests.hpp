#pragma once
#include "UniquePtr.hpp"
#include "SharedPtr.hpp"
#include "DynamicArray.hpp"
#include "LinkedList.hpp"
#include "Exceptions.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <cassert>

struct Tracked
{
    static int alive_count;
    int value;

    Tracked(int v = 0) : value(v) { ++alive_count; }
    Tracked(const Tracked &o) : value(o.value) { ++alive_count; }
    ~Tracked() { --alive_count; }
};
int Tracked::alive_count = 0;

struct Animal
{
    virtual ~Animal() = default;
    virtual std::string speak() const { return "Ничего"; }
};

struct Dog : Animal
{
    std::string name;
    Dog(const std::string &n = "Мухтар") : name(n) {}
    std::string speak() const override { return "Голос"; }
};

static int tests_passed = 0;
static int tests_failed = 0;

#define RUN_TEST(name)                               \
    std::cout << "  [ТЕСТ] " << #name << " ... ";    \
    try                                              \
    {                                                \
        name();                                      \
        std::cout << "Пройден\n";                      \
        ++tests_passed;                              \
    }                                                \
    catch (const std::exception &e)                  \
    {                                                \
        std::cout << "Не пройден: " << e.what() << "\n"; \
        ++tests_failed;                              \
    }                                                \
    catch (...)                                      \
    {                                                \
        std::cout << "Не пройден: Неизвестная ошибка\n"; \
        ++tests_failed;                              \
    }

#define ASSERT_EQ(a, b)                               \
    if ((a) != (b))                                   \
    {                                                 \
        std::ostringstream oss;                       \
        oss << "Ожидалось: " << (a) << " == " << (b); \
        throw std::runtime_error(oss.str());          \
    }

#define ASSERT_TRUE(cond) \
    if (!(cond))          \
    throw std::runtime_error("Утверждение ложно: " #cond)

void test_unique_basic()
{
    UniquePtr<int> p(new int(42));
    ASSERT_TRUE(p);
    ASSERT_EQ(*p, 42);
}

void test_unique_move()
{
    UniquePtr<int> a(new int(10));
    UniquePtr<int> b = std::move(a);
    ASSERT_TRUE(!a);
    ASSERT_EQ(*b, 10);
}

void test_unique_array()
{
    Tracked::alive_count = 0;
    {
        UniquePtr<Tracked[]> arr(new Tracked[3]);
        ASSERT_EQ(Tracked::alive_count, 3);
        arr[1].value = 99;
        ASSERT_EQ(arr[1].value, 99);
    }
    ASSERT_EQ(Tracked::alive_count, 0);
}

void test_unique_subtype()
{
    UniquePtr<Dog> dog(new Dog("Шарик"));
    UniquePtr<Animal> animal = std::move(dog);
    ASSERT_TRUE(!dog);
    ASSERT_EQ(animal->speak(), std::string("Голос"));
}

void test_shared_basic_and_copy()
{
    SharedPtr<int> a(new int(42));
    SharedPtr<int> b = a;
    ASSERT_EQ(a.use_count(), (size_t)2);
    ASSERT_EQ(b.use_count(), (size_t)2);
}

void test_shared_self_assign()
{
    SharedPtr<int> a(new int(5));
    a = a;
    ASSERT_EQ(a.use_count(), (size_t)1);
    ASSERT_EQ(*a, 5);
}

void test_shared_aliasing()
{
    SharedPtr<int> a(new int(5));
    SharedPtr<int> b = a; 

    SharedPtr<int> c(new int(10)); 
    c = a;                        

    ASSERT_EQ(a.use_count(), (size_t)3);
    ASSERT_EQ(c.use_count(), (size_t)3);
}

void test_shared_array()
{
    Tracked::alive_count = 0;
    {
        SharedPtr<Tracked[]> arr(new Tracked[4]);
        ASSERT_EQ(Tracked::alive_count, 4);
        SharedPtr<Tracked[]> arr2 = arr;
        ASSERT_EQ(arr.use_count(), (size_t)2);
    }
    ASSERT_EQ(Tracked::alive_count, 0);
}

void test_shared_from_unique()
{
    Tracked::alive_count = 0;
    {
        UniquePtr<Tracked> u(new Tracked(77));
        SharedPtr<Tracked> s(std::move(u));
        ASSERT_TRUE(!u);
        ASSERT_EQ(s.use_count(), (size_t)1);
        ASSERT_EQ(s->value, 77);
    }
    ASSERT_EQ(Tracked::alive_count, 0);
}

void test_exceptions()
{
    DynamicArray<int> arr;
    bool caught = false;
    try
    {
        arr.Get(0);
    }
    catch (const IndexOutOfRange &e)
    {
        caught = true;
        ASSERT_EQ(e.getIndex(), (size_t)0);
    }
    ASSERT_TRUE(caught);
}

inline void run_all_tests()
{
    std::cout << "\n=== ЗАПУСК ТЕСТОВ ===\n";
    tests_passed = 0;
    tests_failed = 0;

    std::cout << "\n[ UniquePtr ]\n";
    RUN_TEST(test_unique_basic);
    RUN_TEST(test_unique_move);
    RUN_TEST(test_unique_array);
    RUN_TEST(test_unique_subtype);

    std::cout << "\n[ SharedPtr ]\n";
    RUN_TEST(test_shared_basic_and_copy);
    RUN_TEST(test_shared_self_assign);
    RUN_TEST(test_shared_aliasing);
    RUN_TEST(test_shared_array);
    RUN_TEST(test_shared_from_unique);

    std::cout << "\n[ Исключения и Контейнеры ]\n";
    RUN_TEST(test_exceptions);

    std::cout << "\n=== ИТОГО: Пройдено: " << tests_passed
              << ", Провалено: " << tests_failed << " ===\n\n";
}