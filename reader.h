#ifndef READER_H
#define READER_H

#include <QString>
#include <QList>

// Класс, представляющий читателя библиотеки
class Reader
{
public:
    // Конструктор по умолчанию
    Reader();

    // Конструктор с параметрами: ФИО и номер паспорта
    Reader(const QString& name, const QString& passport);

    // Геттеры для доступа к приватным полям
    QString getName() const;      // Возвращает ФИО читателя
    QString getPassport() const;  // Возвращает номер паспорта

    // Метод для отображения информации о читателе в интерфейсе
    QString toString() const;

    // Метод для сохранения в файл (формат: имя|паспорт)
    QString toFileString() const;

    // Статический метод для восстановления читателя из строки файла
    static Reader fromString(const QString& str);

    // Операторы сравнения для поиска и удаления
    bool operator==(const Reader& other) const;
    bool operator!=(const Reader& other) const;

private:
    QString m_name;      // ФИО читателя
    QString m_passport;  // Номер паспорта (уникальный идентификатор)
};

#endif // READER_H