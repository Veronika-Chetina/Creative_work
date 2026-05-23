#ifndef BOOK_H
#define BOOK_H

#include <QString>
#include <QDateTime>

// Класс, представляющий книгу в библиотечной системе
class Book
{
private:
    // Приватные поля класса - хранят данные о книге
    QString m_title;      // Название книги
    QString m_author;     // Автор книги
    QString m_publisher;  // Издательство
    int m_year;           // Год выпуска
public:
    // Конструктор по умолчанию - создаёт пустую книгу
    Book();

    // Конструктор с параметрами для создания полной информации о книге
    Book(const QString& title, const QString& author,
         const QString& publisher, int year);

    // Геттеры - методы для получения значений полей класса
    QString getTitle() const;      // Возвращает название книги
    QString getAuthor() const;     // Возвращает автора книги
    QString getPublisher() const;  // Возвращает издательство
    int getYear() const;           // Возвращает год выпуска

    // Метод для отображения информации о книге в пользовательском интерфейсе
    QString toString() const;

    // Метод для сохранения книги в файл (формат: поле1|поле2|поле3|поле4)
    QString toFileString() const;

    // Статический метод для создания книги из строки файла
    static Book fromString(const QString& str);

    // Операторы сравнения для поиска и удаления книг
    bool operator==(const Book& other) const;
    bool operator!=(const Book& other) const;


};

#endif // BOOK_H