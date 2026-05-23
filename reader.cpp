#include "reader.h"

// Конструктор по умолчанию: создаёт пустого читателя
Reader::Reader() {}

// Конструктор с параметрами: инициализирует имя и паспорт
Reader::Reader(const QString& name, const QString& passport)
    : m_name(name), m_passport(passport) {}

// Реализация геттеров
QString Reader::getName() const { return m_name; }
QString Reader::getPassport() const { return m_passport; }

// Оператор равенства: сравнивает оба поля читателя
bool Reader::operator==(const Reader& other) const
{
    return m_name == other.m_name && m_passport == other.m_passport;
}

// Оператор неравенства
bool Reader::operator!=(const Reader& other) const
{
    return !(*this == other);
}

// Форматированное отображение для интерфейса
// Пример: "Иванов И.И. (Паспорт: 1234567890)"
QString Reader::toString() const
{
    return QString("%1 (Паспорт: %2)").arg(m_name).arg(m_passport);
}

// Формат для сохранения в файл: "имя|паспорт"
QString Reader::toFileString() const
{
    return QString("%1|%2").arg(m_name).arg(m_passport);
}

// Статический метод: создаёт читателя из строки файла
Reader Reader::fromString(const QString& str)
{
    QStringList parts = str.split('|');
    if (parts.size() == 2) {
        return Reader(parts[0], parts[1]);
    }
    return Reader(); // Возвращаем пустого читателя при ошибке
}