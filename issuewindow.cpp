#include "issuewindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime>

// Конструктор окна выдачи книги
IssueWindow::IssueWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_bookSelected(false)
    , m_readerSelected(false)
{
    setWindowTitle("Выдача книги");
    setFixedSize(900, 500);  // Фиксированный размер окна
    setupUI();
    loadBooks();    // Загрузка доступных книг
    loadReaders();  // Загрузка читателей
}

IssueWindow::~IssueWindow() {}

// Настройка интерфейса
void IssueWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);

    // ========== ВЫБОР КНИГИ ==========
    m_bookGroup = new QGroupBox("Выберите книгу");
    QVBoxLayout *bookLayout = new QVBoxLayout(m_bookGroup);

    m_bookList = new QListWidget();  // Список для отображения книг
    bookLayout->addWidget(m_bookList);

    m_selectBookButton = new QPushButton("Выбрать");
    bookLayout->addWidget(m_selectBookButton);

    // ========== ВЫБОР ЧИТАТЕЛЯ ==========
    m_readerGroup = new QGroupBox("Выберите читателя");
    QVBoxLayout *readerLayout = new QVBoxLayout(m_readerGroup);

    m_readerList = new QListWidget();
    readerLayout->addWidget(m_readerList);

    m_selectReaderButton = new QPushButton("Выбрать");
    readerLayout->addWidget(m_selectReaderButton);

    // ========== ВЫБОР ДАТЫ ВОЗВРАТА ==========
    QGroupBox *dateGroup = new QGroupBox("Срок сдачи");
    QVBoxLayout *dateLayout = new QVBoxLayout(dateGroup);

    m_dateEdit = new QDateTimeEdit();
    m_dateEdit->setDateTime(QDateTime::currentDateTime().addDays(14));  // По умолчанию через 14 дней
    m_dateEdit->setCalendarPopup(true);  // Всплывающий календарь
    dateLayout->addWidget(m_dateEdit);

    m_issueButton = new QPushButton("Выдать книгу");
    dateLayout->addWidget(m_issueButton);

    // Добавляем все три панели в главный layout
    mainLayout->addWidget(m_bookGroup);
    mainLayout->addWidget(m_readerGroup);
    mainLayout->addWidget(dateGroup);

    // Подключение сигналов
    connect(m_selectBookButton, &QPushButton::clicked, this, &IssueWindow::onSelectBook);
    connect(m_selectReaderButton, &QPushButton::clicked, this, &IssueWindow::onSelectReader);
    connect(m_issueButton, &QPushButton::clicked, this, &IssueWindow::onIssueBook);
}

// Загрузка доступных книг из файла библиотеки
void IssueWindow::loadBooks()
{
    m_books.clear();
    QDir().mkpath("database");
    QFile file("database/библиотека.txt");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            Book book = Book::fromString(line);
            if (!book.getTitle().isEmpty()) {
                m_books.append(book);
            }
        }
        file.close();
    }
    updateBookList();
}

// Загрузка списка читателей из файла
void IssueWindow::loadReaders()
{
    m_readers.clear();
    QFile file("database/Читатели.txt");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            Reader reader = Reader::fromString(line);
            if (!reader.getName().isEmpty()) {
                m_readers.append(reader);
            }
        }
        file.close();
    }
    updateReaderList();
}

// Обновление списка книг в интерфейсе
void IssueWindow::updateBookList()
{
    m_bookList->clear();
    for (const Book& book : m_books) {
        m_bookList->addItem(book.toString());  // Отображаем книгу в удобном формате
    }
}

// Обновление списка читателей в интерфейсе
void IssueWindow::updateReaderList()
{
    m_readerList->clear();
    for (const Reader& reader : m_readers) {
        m_readerList->addItem(reader.toString());
    }
}

// Обработчик выбора книги
void IssueWindow::onSelectBook()
{
    int row = m_bookList->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите книгу!");
        return;
    }

    m_selectedBook = m_books[row];
    m_bookSelected = true;

    // Меняем заголовок группы, чтобы показать выбранную книгу
    m_bookGroup->setTitle(QString("Выбрана книга: %1").arg(m_selectedBook.getTitle()));
    QMessageBox::information(this, "Успех", QString("Книга \"%1\" выбрана!").arg(m_selectedBook.getTitle()));
}

// Обработчик выбора читателя
void IssueWindow::onSelectReader()
{
    int row = m_readerList->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите читателя!");
        return;
    }

    m_selectedReader = m_readers[row];
    m_readerSelected = true;

    m_readerGroup->setTitle(QString("Выбран читатель: %1").arg(m_selectedReader.getName()));
    QMessageBox::information(this, "Успех", QString("Читатель \"%1\" выбран!").arg(m_selectedReader.getName()));
}

// Основной метод - выдача книги
void IssueWindow::onIssueBook()
{
    // Проверка, что выбрана книга
    if (!m_bookSelected) {
        QMessageBox::warning(this, "Ошибка", "Сначала выберите книгу!");
        return;
    }

    // Проверка, что выбран читатель
    if (!m_readerSelected) {
        QMessageBox::warning(this, "Ошибка", "Сначала выберите читателя!");
        return;
    }

    // ШАГ 1: Удаляем книгу из библиотеки
    for (int i = 0; i < m_books.size(); ++i) {
        if (m_books[i].getTitle() == m_selectedBook.getTitle() &&
            m_books[i].getAuthor() == m_selectedBook.getAuthor()) {
            m_books.removeAt(i);
            break;
        }
    }
    saveBooks();  // Сохраняем обновлённый список библиотеки

    // ШАГ 2: Добавляем книгу читателю в его персональный файл
    QString passport = m_selectedReader.getPassport();
    QString fileName = QString("users/%1.txt").arg(passport);

    QDir().mkpath("users");

    // Сначала читаем существующие данные
    QString existingData;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        existingData = in.readAll();
        file.close();
    }

    // Записываем обновлённые данные (старые + новая книга)
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << existingData;
        out << m_selectedBook.getTitle() << ";"
            << m_dateEdit->dateTime().toString("dd.MM.yyyy") << ";";
        file.close();
    }

    // Вычисляем количество дней до возврата для информационного сообщения
    QString dueDateStr = m_dateEdit->dateTime().toString("dd.MM.yyyy");
    QDate dueDate = QDate::fromString(dueDateStr, "dd.MM.yyyy");
    QDate today = QDate::currentDate();
    int daysToReturn = today.daysTo(dueDate);

    QMessageBox::information(this, "Успех",
                             QString("Книга \"%1\" выдана читателю \"%2\"\n"
                                     "Срок сдачи: %3\n"
                                     "До возврата осталось: %4 дней")
                                 .arg(m_selectedBook.getTitle())
                                 .arg(m_selectedReader.getName())
                                 .arg(dueDateStr)
                                 .arg(daysToReturn));

    // Сброс состояния выбора
    m_bookSelected = false;
    m_readerSelected = false;
    m_bookGroup->setTitle("Выберите книгу");
    m_readerGroup->setTitle("Выберите читателя");

    // Перезагружаем списки для отображения актуальных данных
    loadBooks();
    loadReaders();
}

// Сохранение обновлённого списка книг в файл
void IssueWindow::saveBooks()
{
    QDir().mkpath("database");
    QFile file("database/библиотека.txt");

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const Book& book : m_books) {
            out << book.toFileString() << "\n";
        }
        file.close();
    }
}