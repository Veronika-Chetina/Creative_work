#include "bookwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QLabel>

// Конструктор окна управления книгами
BookWindow::BookWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Управление книгами");
    setMinimumSize(900, 600);
    setupUI();      // Настройка интерфейса
    loadBooks();    // Загрузка книг из файла
}

BookWindow::~BookWindow() {}

// Настройка пользовательского интерфейса
void BookWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);  // Горизонтальное расположение

    // ========== ЛЕВАЯ ПАНЕЛЬ - ДОБАВЛЕНИЕ КНИГ ==========
    QGroupBox *addGroup = new QGroupBox("Добавление книги");
    QFormLayout *formLayout = new QFormLayout(addGroup);

    // Создание полей ввода
    m_titleEdit = new QLineEdit();
    m_authorEdit = new QLineEdit();
    m_publisherEdit = new QLineEdit();
    m_yearEdit = new QLineEdit();

    // Добавление полей в форму
    formLayout->addRow("Название:", m_titleEdit);
    formLayout->addRow("Автор:", m_authorEdit);
    formLayout->addRow("Издательство:", m_publisherEdit);
    formLayout->addRow("Год выпуска:", m_yearEdit);

    m_addButton = new QPushButton("Добавить");
    formLayout->addRow(m_addButton);

    connect(m_addButton, &QPushButton::clicked, this, &BookWindow::onAddBook);

    // ========== ПРАВАЯ ПАНЕЛЬ - СПИСОК КНИГ ==========
    QWidget *rightPanel = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

    // Строка поиска
    QHBoxLayout *searchLayout = new QHBoxLayout();
    QLabel *searchLabel = new QLabel("Поиск:");
    searchLayout->addWidget(searchLabel);
    m_searchEdit = new QLineEdit();
    searchLayout->addWidget(m_searchEdit);
    rightLayout->addLayout(searchLayout);

    // Таблица с книгами (4 колонки)
    m_bookTable = new QTableWidget();
    m_bookTable->setColumnCount(4);
    m_bookTable->setHorizontalHeaderLabels({"Название", "Автор", "Издательство", "Год"});
    m_bookTable->horizontalHeader()->setStretchLastSection(true);
    m_bookTable->setSelectionBehavior(QAbstractItemView::SelectRows);  // Выделение всей строки
    rightLayout->addWidget(m_bookTable);

    // Кнопки управления
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_deleteButton = new QPushButton("Удалить");
    m_saveButton = new QPushButton("Сохранить");
    buttonLayout->addWidget(m_deleteButton);
    buttonLayout->addWidget(m_saveButton);
    rightLayout->addLayout(buttonLayout);

    // Добавляем панели в главный layout
    mainLayout->addWidget(addGroup);
    mainLayout->addWidget(rightPanel);
    mainLayout->setStretch(0, 1);   // Левая панель - 1 часть
    mainLayout->setStretch(1, 3);   // Правая панель - 3 части

    // Подключение сигналов
    connect(m_deleteButton, &QPushButton::clicked, this, &BookWindow::onDeleteBook);
    connect(m_saveButton, &QPushButton::clicked, this, &BookWindow::onSaveBooks);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &BookWindow::onSearchTextChanged);
}

// Загрузка книг из файла database/библиотека.txt
void BookWindow::loadBooks()
{
    QDir().mkpath("database");  // Создаём папку, если её нет
    QFile file("database/библиотека.txt");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            Book book = Book::fromString(line);  // Преобразуем строку в объект Book
            if (!book.getTitle().isEmpty()) {
                m_books.append(book);
            }
        }
        file.close();
    }

    updateBookTable();  // Обновляем отображение
}

// Обновление таблицы с книгами
void BookWindow::updateBookTable()
{
    // Определяем, какой список отображать: полный или отфильтрованный
    const QList<Book>& displayBooks = m_filteredBooks.isEmpty() ? m_books : m_filteredBooks;
    m_bookTable->setRowCount(displayBooks.size());

    // Заполняем таблицу данными
    for (int i = 0; i < displayBooks.size(); ++i) {
        m_bookTable->setItem(i, 0, new QTableWidgetItem(displayBooks[i].getTitle()));
        m_bookTable->setItem(i, 1, new QTableWidgetItem(displayBooks[i].getAuthor()));
        m_bookTable->setItem(i, 2, new QTableWidgetItem(displayBooks[i].getPublisher()));
        m_bookTable->setItem(i, 3, new QTableWidgetItem(QString::number(displayBooks[i].getYear())));
    }

    m_bookTable->resizeColumnsToContents();  // Автоподбор ширины колонок
}

// Обработчик добавления новой книги
void BookWindow::onAddBook()
{
    // Проверка заполнения всех полей
    if (m_titleEdit->text().isEmpty() || m_authorEdit->text().isEmpty() ||
        m_publisherEdit->text().isEmpty() || m_yearEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля!");
        return;
    }

    // Валидация года выпуска
    bool ok;
    int year = m_yearEdit->text().toInt(&ok);
    if (!ok || year < 0 || year > 2025) {
        QMessageBox::warning(this, "Ошибка", "Некорректный год!");
        return;
    }

    // Создаём объект книги и добавляем в список
    Book book(m_titleEdit->text(), m_authorEdit->text(),
              m_publisherEdit->text(), year);
    m_books.append(book);

    // Очищаем поля ввода
    m_titleEdit->clear();
    m_authorEdit->clear();
    m_publisherEdit->clear();
    m_yearEdit->clear();

    updateBookTable();
    QMessageBox::information(this, "Успех", "Книга добавлена!");
}

// Обработчик удаления выбранной книги
void BookWindow::onDeleteBook()
{
    int row = m_bookTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите книгу для удаления!");
        return;
    }

    const QList<Book>& displayBooks = m_filteredBooks.isEmpty() ? m_books : m_filteredBooks;
    if (row < displayBooks.size()) {
        Book bookToRemove = displayBooks[row];

        // Удаляем из основного списка
        if (m_filteredBooks.isEmpty()) {
            m_books.removeAt(row);
        } else {
            // Если включён поиск, удаляем из полного списка по найденному объекту
            int originalIndex = m_books.indexOf(bookToRemove);
            if (originalIndex >= 0) {
                m_books.removeAt(originalIndex);
            }
            m_filteredBooks.removeAt(row);
        }
        updateBookTable();
    }
}

// Обработчик сохранения книг в файл
void BookWindow::onSaveBooks()
{
    saveBooks();
    QMessageBox::information(this, "Успех", "Данные сохранены!");
}

// Сохранение книг в файл
void BookWindow::saveBooks()
{
    QDir().mkpath("database");
    QFile file("database/библиотека.txt");

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const Book& book : m_books) {
            out << book.toFileString() << "\n";  // Каждая книга на новой строке
        }
        file.close();
    }
}

// Обработчик изменения текста в поле поиска (фильтрация в реальном времени)
void BookWindow::onSearchTextChanged(const QString &text)
{
    m_filteredBooks.clear();

    if (text.isEmpty()) {
        updateBookTable();  // Если поиск пуст, показываем все книги
        return;
    }

    // Поиск по названию или автору (без учёта регистра)
    for (const Book& book : m_books) {
        if (book.getTitle().contains(text, Qt::CaseInsensitive) ||
            book.getAuthor().contains(text, Qt::CaseInsensitive)) {
            m_filteredBooks.append(book);
        }
    }

    updateBookTable();
}

// Вспомогательный метод - извлечение номера паспорта из строки (не используется в этом окне)
QString BookWindow::extractPassport(const QString& readerStr)
{
    QRegularExpression re("(\\d+)");
    QRegularExpressionMatch match = re.match(readerStr);
    if (match.hasMatch()) {
        return match.captured(1);
    }
    return "";
}