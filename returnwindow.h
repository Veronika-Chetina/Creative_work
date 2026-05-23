#ifndef RETURNWINDOW_H
#define RETURNWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include "book.h"
#include "reader.h"

// Класс окна для возврата книги от читателя
class ReturnWindow : public QMainWindow
{
    Q_OBJECT

public:
    ReturnWindow(QWidget *parent = nullptr);
    ~ReturnWindow();

private slots:
    void onSelectReader();   // Обработчик выбора читателя
    void onSelectBook();     // Обработчик выбора книги для возврата
    void onReturnBook();     // Обработчик возврата книги

private:
    void setupUI();                              // Настройка интерфейса
    void loadReaders();                          // Загрузка списка читателей
    void loadReaderBooks(const QString& passport);  // Загрузка книг выбранного читателя
    void saveBookToLibrary(const Book& book);    // Добавление книги обратно в библиотеку

    // Виджеты интерфейса
    QListWidget *m_readerList;       // Список читателей
    QListWidget *m_bookList;         // Список книг выбранного читателя
    QPushButton *m_selectReaderButton;  // Кнопка выбора читателя
    QPushButton *m_returnButton;     // Кнопка "Принять книгу"
    QGroupBox *m_readerGroup;        // Группа выбора читателя
    QGroupBox *m_bookGroup;          // Группа списка книг
    QLabel *m_readerInfoLabel;       // Метка с информацией о выбранном читателе

    // Данные
    QList<Reader> m_readers;         // Список всех читателей
    Reader m_selectedReader;         // Выбранный читатель
    Book m_selectedBook;             // Выбранная книга для возврата
    QString m_currentPassport;       // Паспорт выбранного читателя
    bool m_readerSelected;           // Флаг: выбран ли читатель
    bool m_bookSelected;             // Флаг: выбрана ли книга
};

#endif // RETURNWINDOW_H