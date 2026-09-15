#pragma once
#include <string>
#include <sstream>

class IException
{
public:
    virtual ~IException() = default;
    virtual const char *what() const noexcept = 0;
};

class DataStructureException : public IException
{
    std::string message;

public:
    DataStructureException(const std::string &msg) : message(msg) {}
    const char *what() const noexcept override { return message.c_str(); }
};

class IndexOutOfRange : public DataStructureException
{
public:
    IndexOutOfRange(std::size_t index, std::size_t size, const std::string &context)
        : DataStructureException(buildMessage(index, size, context)),
          m_index(index), m_size(size) {}

    std::size_t getIndex() const noexcept { return m_index; }
    std::size_t getSize() const noexcept { return m_size; }

private:
    static std::string buildMessage(std::size_t index, std::size_t size, const std::string &context)
    {
        std::ostringstream oss;
        oss << "Ошибка в " << context << ": индекс " << index
            << " вне допустимого диапазона [0, " << (size == 0 ? 0 : size - 1) << "]";
        return oss.str();
    }
    std::size_t m_index;
    std::size_t m_size;
};

class EmptyStructureException : public DataStructureException
{
public:
    EmptyStructureException(const std::string &context)
        : DataStructureException("Ошибка в " + context + ": последовательность пуста") {}
};

class InvalidArgument : public DataStructureException
{
public:
    InvalidArgument(const std::string &context)
        : DataStructureException("Неверный аргумент в " + context) {}

    InvalidArgument(const std::string &context, const std::string &detail)
        : DataStructureException("Неверный аргумент в " + context + ": " + detail) {}
};

class DivisionByZeroException : public DataStructureException
{
public:
    DivisionByZeroException(const std::string &context)
        : DataStructureException("Деление на ноль в " + context) {}

    DivisionByZeroException(const std::string &context, const std::string &detail)
        : DataStructureException("Деление на ноль в " + context + " (" + detail + ")") {}
};

class MathDomainException : public DataStructureException
{
public:
    MathDomainException(const std::string &context)
        : DataStructureException("Математическая ошибка в " + context) {}

    MathDomainException(const std::string &context, const std::string &detail)
        : DataStructureException("Математическая ошибка в " + context + ": " + detail) {}
};

class NullPtrException : public DataStructureException
{
public:
    NullPtrException(const std::string &context)
        : DataStructureException("Ошибка в " + context + ": попытка разыменования нулевого указателя") {}

    NullPtrException(const std::string &context, const std::string &detail)
        : DataStructureException("Ошибка в " + context + ": " + detail) {}
};

class MemoryAllocationException : public DataStructureException
{
public:
    MemoryAllocationException(const std::string &context)
        : DataStructureException("Ошибка выделения памяти в " + context) {}

    MemoryAllocationException(const std::string &context, const std::string &detail)
        : DataStructureException("Ошибка выделения памяти в " + context + ": " + detail) {}
};