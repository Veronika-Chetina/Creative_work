#ifndef ISSUEWINDOW_H
#define ISSUEWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QGroupBox>
#include <QDateTimeEdit>
#include "book.h"
#include "reader.h"

// Класс окна для выдачи книги читателю
class IssueWindow : public QMainWindow
{
    Q_OBJECT

public:
    IssueWindow(QWidget *parent = nullptr);
    ~IssueWindow();

private slots:
    void onSelectBook();    // Обработчик выбора книги из списка
    void onSelectReader();  // Обработчик выбора читателя из списка
    void onIssueBook();     // Обработчик выдачи книги

private:
    void setupUI();         // Настройка интерфейса
    void loadBooks();       // Загрузка списка доступных книг
    void loadReaders();     // Загрузка списка читателей
    void updateBookList();  // Обновление списка книг
    void updateReaderList(); // Обновление списка читателей
    void saveBooks();       // Сохранение обновлённого списка книг

    // Виджеты интерфейса
    QListWidget *m_bookList;       // Список доступных книг
    QListWidget *m_readerList;     // Список читателей
    QPushButton *m_selectBookButton;   // Кнопка выбора книги
    QPushButton *m_selectReaderButton; // Кнопка выбора читателя
    QPushButton *m_issueButton;        // Кнопка "Выдать книгу"
    QGroupBox *m_bookGroup;            // Группа выбора книги
    QGroupBox *m_readerGroup;          // Группа выбора читателя
    QDateTimeEdit *m_dateEdit;         // Выбор даты возврата

    // Данные
    QList<Book> m_books;         // Список всех книг в библиотеке
    QList<Reader> m_readers;     // Список всех читателей
    Book m_selectedBook;         // Выбранная книга
    Reader m_selectedReader;     // Выбранный читатель
    bool m_bookSelected;         // Флаг: выбрана ли книга
    bool m_readerSelected;       // Флаг: выбран ли читатель
};

#endif // ISSUEWINDOW_H