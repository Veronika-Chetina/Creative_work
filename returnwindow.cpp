#include "returnwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDir>

// Конструктор окна возврата книги
ReturnWindow::ReturnWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_readerSelected(false)
    , m_bookSelected(false)
{
    setWindowTitle("Приём книги");
    setFixedSize(900, 500);
    setupUI();
    loadReaders();  // Загрузка читателей
}

ReturnWindow::~ReturnWindow() {}

// Настройка интерфейса
void ReturnWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);

    // ========== ВЫБОР ЧИТАТЕЛЯ ==========
    m_readerGroup = new QGroupBox("Выберите читателя");
    QVBoxLayout *readerLayout = new QVBoxLayout(m_readerGroup);

    m_readerList = new QListWidget();
    readerLayout->addWidget(m_readerList);

    m_selectReaderButton = new QPushButton("Выбрать");
    readerLayout->addWidget(m_selectReaderButton);

    // ========== СПИСОК КНИГ ЧИТАТЕЛЯ ==========
    m_bookGroup = new QGroupBox("Книги на руках");
    QVBoxLayout *bookLayout = new QVBoxLayout(m_bookGroup);

    m_readerInfoLabel = new QLabel("Выберите читателя");
    bookLayout->addWidget(m_readerInfoLabel);

    m_bookList = new QListWidget();
    bookLayout->addWidget(m_bookList);

    m_returnButton = new QPushButton("Принять книгу");
    bookLayout->addWidget(m_returnButton);

    // Добавляем панели в главный layout
    mainLayout->addWidget(m_readerGroup);
    mainLayout->addWidget(m_bookGroup);
    mainLayout->setStretch(0, 1);
    mainLayout->setStretch(1, 2);

    // Подключение сигналов
    connect(m_selectReaderButton, &QPushButton::clicked, this, &ReturnWindow::onSelectReader);
    connect(m_returnButton, &QPushButton::clicked, this, &ReturnWindow::onReturnBook);
    connect(m_bookList, &QListWidget::itemClicked, this, &ReturnWindow::onSelectBook);
}

// Загрузка списка читателей из файла
void ReturnWindow::loadReaders()
{
    m_readers.clear();
    QDir().mkpath("database");
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

    m_readerList->clear();
    for (const Reader& reader : m_readers) {
        m_readerList->addItem(reader.toString());
    }
}

// Загрузка книг, которые у выбранного читателя на руках
void ReturnWindow::loadReaderBooks(const QString& passport)
{
    m_bookList->clear();
    QString fileName = QString("users/%1.txt").arg(passport);

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString line = in.readLine();
        file.close();

        if (!line.isEmpty()) {
            // Формат: "Название;ДатаВозврата;Название2;ДатаВозврата2;..."
            QStringList books = line.split(';', Qt::SkipEmptyParts);
            for (int i = 0; i < books.size(); i += 2) {
                if (i + 1 < books.size()) {
                    QString displayText = QString("%1 (до %2)").arg(books[i]).arg(books[i + 1]);
                    m_bookList->addItem(displayText);
                }
            }
        }
    }

    // Если книг нет, выводим сообщение и блокируем кнопку возврата
    if (m_bookList->count() == 0) {
        m_bookList->addItem("Нет книг на руках");
        m_returnButton->setEnabled(false);
    } else {
        m_returnButton->setEnabled(true);
    }
}

// Обработчик выбора читателя
void ReturnWindow::onSelectReader()
{
    int row = m_readerList->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите читателя!");
        return;
    }

    m_selectedReader = m_readers[row];
    m_currentPassport = m_selectedReader.getPassport();
    m_readerSelected = true;

    m_readerInfoLabel->setText(QString("Читатель: %1").arg(m_selectedReader.getName()));
    loadReaderBooks(m_currentPassport);  // Загружаем его книги
}

// Обработчик выбора книги (просто запоминаем, какая книга выбрана)
void ReturnWindow::onSelectBook()
{
    int row = m_bookList->currentRow();
    if (row < 0) {
        return;
    }

    QString itemText = m_bookList->item(row)->text();
    if (itemText == "Нет книг на руках") {
        m_bookSelected = false;
        return;
    }

    m_bookSelected = true;
    // Извлекаем название книги из строки (всё, что до " (до")
    QString bookTitle = itemText.split(" (до").first();
    m_selectedBook = Book(bookTitle, "", "", 0);
}

// Основной метод - возврат книги
void ReturnWindow::onReturnBook()
{
    // Проверки
    if (!m_readerSelected) {
        QMessageBox::warning(this, "Ошибка", "Сначала выберите читателя!");
        return;
    }

    if (!m_bookSelected) {
        QMessageBox::warning(this, "Ошибка", "Выберите книгу для возврата!");
        return;
    }

    // ШАГ 1: Удаляем книгу из файла читателя
    QString fileName = QString("users/%1.txt").arg(m_currentPassport);
    QStringList remainingBooks;

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString line = in.readLine();
        file.close();

        if (!line.isEmpty()) {
            QStringList books = line.split(';', Qt::SkipEmptyParts);
            // Проходим по парам (книга, дата) и оставляем всё, кроме возвращаемой книги
            for (int i = 0; i < books.size(); i += 2) {
                if (books[i] != m_selectedBook.getTitle()) {
                    remainingBooks << books[i] + ";" + books[i + 1];
                }
            }
        }
    }

    // Сохраняем обновлённый список книг читателя
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << remainingBooks.join(";");
        file.close();
    }

    // ШАГ 2: Возвращаем книгу в библиотеку
    QFile libFile("database/библиотека.txt");
    QStringList allBooks;
    bool bookFound = false;

    // Читаем все книги из библиотеки
    if (libFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&libFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            Book book = Book::fromString(line);
            if (book.getTitle() == m_selectedBook.getTitle()) {
                bookFound = true;  // Книга уже есть в библиотеке
                allBooks << line;
            } else {
                allBooks << line;
            }
        }
        libFile.close();
    }

    // Если книги нет в библиотеке, добавляем её (с минимальной информацией)
    if (!bookFound) {
        if (libFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&libFile);
            for (const QString& bookLine : allBooks) {
                out << bookLine << "\n";
            }
            // Добавляем книгу с пустыми полями для автора и издательства
            out << m_selectedBook.getTitle() << "|||0\n";
            libFile.close();
        }
    }

    QMessageBox::information(this, "Успех",
                             QString("Книга \"%1\" возвращена в библиотеку!")
                                 .arg(m_selectedBook.getTitle()));

    // Сбрасываем выбор книги и обновляем список
    m_bookSelected = false;
    loadReaderBooks(m_currentPassport);
}

// Вспомогательный метод для добавления книги в библиотеку (не используется, но оставлен для полноты)
void ReturnWindow::saveBookToLibrary(const Book& book)
{
    QFile file("database/библиотека.txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << book.toFileString() << "\n";
        file.close();
    }
}