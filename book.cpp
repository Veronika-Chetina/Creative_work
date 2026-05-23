#include "book.h"

// Конструктор по умолчанию: инициализирует год нулём
Book::Book() : m_year(0) {}

// Конструктор с параметрами: инициализирует все поля переданными значениями
Book::Book(const QString& title, const QString& author,
           const QString& publisher, int year)
    : m_title(title), m_author(author), m_publisher(publisher), m_year(year) {}

// Реализация геттеров - возвращают значения приватных полей
QString Book::getTitle() const { return m_title; }
QString Book::getAuthor() const { return m_author; }
QString Book::getPublisher() const { return m_publisher; }
int Book::getYear() const { return m_year; }

// Оператор равенства - сравнивает все поля книги
bool Book::operator==(const Book& other) const
{
    return m_title == other.m_title &&
           m_author == other.m_author &&
           m_publisher == other.m_publisher &&
           m_year == other.m_year;
}

// Оператор неравенства - использует оператор равенства и инвертирует результат
bool Book::operator!=(const Book& other) const
{
    return !(*this == other);
}

// Преобразование книги в строку для отображения пользователю
// Формат: "Название by Автор (Издательство, Год)"
QString Book::toString() const
{
    return QString("%1 by %2 (%3, %4)")
    .arg(m_title).arg(m_author).arg(m_publisher).arg(m_year);
}

// Преобразование книги в строку для сохранения в файл
// Используется разделитель "|" для простоты парсинга
QString Book::toFileString() const
{
    return QString("%1|%2|%3|%4")
    .arg(m_title).arg(m_author).arg(m_publisher).arg(m_year);
}

// Статический метод: создаёт книгу из строки файла
// Ожидает формат: "название|автор|издательство|год"
Book Book::fromString(const QString& str)
{
    QStringList parts = str.split('|');
    if (parts.size() == 4) {
        return Book(parts[0], parts[1], parts[2], parts[3].toInt());
    }
    return Book(); // Возвращаем пустую книгу при ошибке формата
}