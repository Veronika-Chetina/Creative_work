#ifndef READERWINDOW_H
#define READERWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QList>
#include "reader.h"

// Класс окна для управления читателями (регистрация, удаление, просмотр книг)
class ReaderWindow : public QMainWindow
{
    Q_OBJECT

public:
    ReaderWindow(QWidget *parent = nullptr);
    ~ReaderWindow();

private slots:
    void onAddReader();                      // Регистрация нового читателя
    void onDeleteReader();                   // Удаление читателя
    void onSaveReaders();                    // Сохранение списка читателей
    void onSearchTextChanged(const QString &text);  // Поиск читателей
    void onShowInfo();                       // Показать книги выбранного читателя

private:
    void setupUI();                          // Настройка интерфейса
    void loadReaders();                      // Загрузка читателей из файла
    void updateReaderTable();                // Обновление таблицы читателей
    void saveReaders();                      // Сохранение читателей в файл
    QString extractPassport(const QString& readerStr);  // Извлечение паспорта из строки
    void loadReaderBooks(const QString& passport);      // Загрузка книг выбранного читателя

    // Виджеты интерфейса
    QTableWidget *m_readerTable;             // Таблица читателей
    QLineEdit *m_nameEdit;                   // Поле ввода ФИО
    QLineEdit *m_passportEdit;               // Поле ввода паспорта
    QLineEdit *m_searchEdit;                 // Поле поиска
    QPushButton *m_addButton;                // Кнопка "Зарегистрировать"
    QPushButton *m_deleteButton;             // Кнопка "Удалить"
    QPushButton *m_saveButton;               // Кнопка "Сохранить"
    QPushButton *m_infoButton;               // Кнопка "Информация о книгах"
    QTableWidget *m_booksTable;              // Таблица книг выбранного читателя

    QList<Reader> m_readers;                 // Полный список читателей
    QList<Reader> m_filteredReaders;         // Отфильтрованный список (для поиска)
};

#endif // READERWINDOW_H