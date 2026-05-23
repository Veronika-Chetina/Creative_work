#ifndef BOOKWINDOW_H
#define BOOKWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QList>
#include "book.h"

// Класс окна для управления книгами (добавление, удаление, поиск, просмотр)
class BookWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Конструктор и деструктор
    BookWindow(QWidget *parent = nullptr);
    ~BookWindow();

private slots:
    void onAddBook();                      // Обработчик добавления новой книги
    void onDeleteBook();                   // Обработчик удаления выбранной книги
    void onSaveBooks();                    // Обработчик сохранения всех книг в файл
    void onSearchTextChanged(const QString &text);  // Обработчик изменения текста поиска

private:
    void setupUI();                        // Настройка пользовательского интерфейса
    void loadBooks();                      // Загрузка книг из файла
    void updateBookTable();                // Обновление таблицы с книгами
    void saveBooks();                      // Сохранение книг в файл
    QString extractPassport(const QString& readerStr);  // Вспомогательный метод (не используется в этом окне)

    // Виджеты интерфейса
    QTableWidget *m_bookTable;             // Таблица для отображения списка книг
    QLineEdit *m_titleEdit;                // Поле ввода названия книги
    QLineEdit *m_authorEdit;               // Поле ввода автора
    QLineEdit *m_publisherEdit;            // Поле ввода издательства
    QLineEdit *m_yearEdit;                 // Поле ввода года выпуска
    QLineEdit *m_searchEdit;               // Поле поиска
    QPushButton *m_addButton;              // Кнопка "Добавить"
    QPushButton *m_deleteButton;           // Кнопка "Удалить"
    QPushButton *m_saveButton;             // Кнопка "Сохранить"

    // Списки книг
    QList<Book> m_books;                   // Полный список всех книг
    QList<Book> m_filteredBooks;           // Отфильтрованный список (для поиска)
};

#endif // BOOKWINDOW_H