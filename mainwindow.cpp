#include "mainwindow.h"
#include "readerwindow.h"
#include "bookwindow.h"
#include "issuewindow.h"
#include "returnwindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QTimer>
#include <QCloseEvent>
#include <QApplication>
#include <QMessageBox>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDate>

MainWindow::MainWindow(const QString& username, const QString& role, QWidget *parent)
    : QMainWindow(parent)
    , m_readerWindow(nullptr)
    , m_bookWindow(nullptr)
    , m_issueWindow(nullptr)
    , m_returnWindow(nullptr)
    , m_username(username)
    , m_userRole(role)
{
    setWindowTitle(QString("Библиотечная система - %1 (%2)").arg(username).arg(role));
    setMinimumSize(900, 700);

    setupMenu();
    setupUI();

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::checkChildWindows);
    m_timer->start(500);

    // Загружаем все должники
    loadAllDueBooks();
}

MainWindow::~MainWindow() {}

void MainWindow::setupMenu()
{
    QMenuBar *menuBar = new QMenuBar(this);

    QMenu *dataMenu = menuBar->addMenu("Данные");

    QAction *booksAction = new QAction("Книги", this);
    QAction *readersAction = new QAction("Читатели", this);

    connect(booksAction, &QAction::triggered, this, &MainWindow::onBooksAction);
    connect(readersAction, &QAction::triggered, this, &MainWindow::onReadersAction);

    dataMenu->addAction(booksAction);
    dataMenu->addAction(readersAction);

    // Меню "Справка"
    QMenu *helpMenu = menuBar->addMenu("Справка");
    QAction *aboutAction = new QAction("О программе", this);
    connect(aboutAction, &QAction::triggered, [this]() {
        QMessageBox::about(this, "О программе",
                           "Библиотечная система АРМ\n\n"
                           "Версия: 1.0\n\n"
                           "Функции:\n"
                           "- Управление книгами\n"
                           "- Управление читателями\n"
                           "- Выдача и приём книг\n"
                           "- Просмотр должников по календарю");
    });
    helpMenu->addAction(aboutAction);

    setMenuBar(menuBar);
}

void MainWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);

    // Левая панель - операции и календарь
    QWidget *leftPanel = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);

    QGroupBox *operationsGroup = new QGroupBox("Операции");
    QVBoxLayout *opLayout = new QVBoxLayout(operationsGroup);

    m_issueButton = new QPushButton("Выдать книгу");
    m_returnButton = new QPushButton("Принять книгу");

    connect(m_issueButton, &QPushButton::clicked, this, &MainWindow::onIssueAction);
    connect(m_returnButton, &QPushButton::clicked, this, &MainWindow::onReturnAction);

    opLayout->addWidget(m_issueButton);
    opLayout->addWidget(m_returnButton);

    m_calendar = new QCalendarWidget(this);
    connect(m_calendar, &QCalendarWidget::clicked, this, &MainWindow::onDateSelected);

    leftLayout->addWidget(operationsGroup);
    leftLayout->addWidget(m_calendar);
    leftLayout->addStretch();

    // Правая панель - список должников
    m_dueBooksGroup = new QGroupBox("Должники");
    QVBoxLayout *dueLayout = new QVBoxLayout(m_dueBooksGroup);

    // Информационная строка
    QHBoxLayout *infoLayout = new QHBoxLayout();
    m_selectedDateLabel = new QLabel("Выберите дату в календаре");
    m_refreshButton = new QPushButton("Обновить");
    connect(m_refreshButton, &QPushButton::clicked, this, &MainWindow::onRefreshDueBooks);
    infoLayout->addWidget(m_selectedDateLabel);
    infoLayout->addWidget(m_refreshButton);
    dueLayout->addLayout(infoLayout);

    // Таблица должников
    m_dueBooksTable = new QTableWidget();
    m_dueBooksTable->setColumnCount(5);
    m_dueBooksTable->setHorizontalHeaderLabels({"Читатель", "Книга", "Дата выдачи", "Срок сдачи", "Статус"});
    m_dueBooksTable->horizontalHeader()->setStretchLastSection(true);
    m_dueBooksTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    dueLayout->addWidget(m_dueBooksTable);

    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(m_dueBooksGroup);
    mainLayout->setStretch(0, 1);
    mainLayout->setStretch(1, 2);
}

void MainWindow::onBooksAction()
{
    if (!m_bookWindow || !m_bookWindow->isVisible()) {
        m_bookWindow = new BookWindow();
        m_bookWindow->show();
    } else {
        m_bookWindow->raise();
        m_bookWindow->activateWindow();
    }
}

void MainWindow::onReadersAction()
{
    if (!m_readerWindow || !m_readerWindow->isVisible()) {
        m_readerWindow = new ReaderWindow();
        m_readerWindow->show();
    } else {
        m_readerWindow->raise();
        m_readerWindow->activateWindow();
    }
}

void MainWindow::onIssueAction()
{
    if (!m_issueWindow || !m_issueWindow->isVisible()) {
        m_issueWindow = new IssueWindow();
        m_issueWindow->show();
    } else {
        m_issueWindow->raise();
        m_issueWindow->activateWindow();
    }
}

void MainWindow::onReturnAction()
{
    if (!m_returnWindow || !m_returnWindow->isVisible()) {
        m_returnWindow = new ReturnWindow();
        m_returnWindow->show();
    } else {
        m_returnWindow->raise();
        m_returnWindow->activateWindow();
    }
}

void MainWindow::checkChildWindows()
{
    bool hasVisibleChild = false;

    if (m_readerWindow && m_readerWindow->isVisible()) hasVisibleChild = true;
    if (m_bookWindow && m_bookWindow->isVisible()) hasVisibleChild = true;
    if (m_issueWindow && m_issueWindow->isVisible()) hasVisibleChild = true;
    if (m_returnWindow && m_returnWindow->isVisible()) hasVisibleChild = true;

    m_issueButton->setEnabled(!hasVisibleChild);
    m_returnButton->setEnabled(!hasVisibleChild);
    menuBar()->setEnabled(!hasVisibleChild);
    m_calendar->setEnabled(!hasVisibleChild);
    m_refreshButton->setEnabled(!hasVisibleChild);
}

void MainWindow::onDateSelected(const QDate& date)
{
    loadDueBooks(date);
}

void MainWindow::onRefreshDueBooks()
{
    loadAllDueBooks();
}

void MainWindow::loadDueBooks(const QDate& date)
{
    m_selectedDateLabel->setText(QString("Должники на %1").arg(date.toString("dd.MM.yyyy")));
    m_dueBooksTable->setRowCount(0);

    QDir usersDir("users");
    QStringList userFiles = usersDir.entryList(QStringList() << "*.txt", QDir::Files);

    int row = 0;
    QDate currentDate = QDate::currentDate();

    for (const QString& fileName : userFiles) {
        QString passport = fileName.left(fileName.indexOf(".txt"));

        // Получаем имя читателя
        QString readerName = "";
        QFile readersFile("database/Читатели.txt");
        if (readersFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&readersFile);
            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList parts = line.split('|');
                if (parts.size() >= 2 && parts[1] == passport) {
                    readerName = parts[0];
                    break;
                }
            }
            readersFile.close();
        }

        if (readerName.isEmpty()) continue;

        // Читаем книги читателя
        QFile file(QString("users/%1").arg(fileName));
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString line = in.readLine();
            file.close();

            if (!line.isEmpty()) {
                QStringList books = line.split(';', Qt::SkipEmptyParts);
                for (int i = 0; i < books.size(); i += 2) {
                    if (i + 1 < books.size()) {
                        QString bookTitle = books[i];
                        QString dueDateStr = books[i + 1];
                        QDate dueDate = QDate::fromString(dueDateStr, "dd.MM.yyyy");

                        // Проверяем, должна ли книга быть сдана в выбранную дату
                        if (dueDate == date) {
                            m_dueBooksTable->insertRow(row);
                            m_dueBooksTable->setItem(row, 0, new QTableWidgetItem(readerName));
                            m_dueBooksTable->setItem(row, 1, new QTableWidgetItem(bookTitle));

                            // Получаем дату выдачи (примерно - 14 дней назад)
                            QDate issueDate = dueDate.addDays(-14);
                            m_dueBooksTable->setItem(row, 2, new QTableWidgetItem(issueDate.toString("dd.MM.yyyy")));
                            m_dueBooksTable->setItem(row, 3, new QTableWidgetItem(dueDateStr));

                            // Определяем статус
                            QString status;
                            if (dueDate < currentDate) {
                                status = QString("Просрочена на %1 дн.").arg(dueDate.daysTo(currentDate));
                            } else if (dueDate == currentDate) {
                                status = "Должен сдать СЕГОДНЯ!";
                            } else {
                                status = QString("Осталось %1 дн.").arg(currentDate.daysTo(dueDate));
                            }
                            m_dueBooksTable->setItem(row, 4, new QTableWidgetItem(status));

                            // Подсветка просроченных
                            if (dueDate < currentDate) {
                                for (int col = 0; col < 5; col++) {
                                    m_dueBooksTable->item(row, col)->setBackground(QColor(255, 200, 200));
                                }
                            } else if (dueDate == currentDate) {
                                for (int col = 0; col < 5; col++) {
                                    m_dueBooksTable->item(row, col)->setBackground(QColor(255, 255, 200));
                                }
                            }

                            row++;
                        }
                    }
                }
            }
        }
    }

    m_dueBooksTable->resizeColumnsToContents();

    if (row == 0) {
        m_dueBooksTable->setRowCount(1);
        m_dueBooksTable->setSpan(0, 0, 1, 5);
        m_dueBooksTable->setItem(0, 0, new QTableWidgetItem("На эту дату нет должников"));
        m_dueBooksTable->item(0, 0)->setTextAlignment(Qt::AlignCenter);
    }
}

void MainWindow::loadAllDueBooks()
{
    // Подсвечиваем в календаре дни, когда есть должники
    QDate currentDate = QDate::currentDate();
    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(QColor(255, 100, 100));

    QTextCharFormat normalFormat;
    normalFormat.setBackground(QColor(255, 255, 255));

    // Сначала сбросим все дни
    for (int i = 1; i <= 31; i++) {
        // Это упрощенный подход, можно сделать через QSet
    }

    // Собираем все даты, когда есть должники
    QSet<QDate> dueDates;

    QDir usersDir("users");
    QStringList userFiles = usersDir.entryList(QStringList() << "*.txt", QDir::Files);

    for (const QString& fileName : userFiles) {
        QFile file(QString("users/%1").arg(fileName));
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString line = in.readLine();
            file.close();

            if (!line.isEmpty()) {
                QStringList books = line.split(';', Qt::SkipEmptyParts);
                for (int i = 0; i < books.size(); i += 2) {
                    if (i + 1 < books.size()) {
                        QDate dueDate = QDate::fromString(books[i + 1], "dd.MM.yyyy");
                        dueDates.insert(dueDate);
                    }
                }
            }
        }
    }

    // Подсвечиваем даты с должниками
    for (const QDate& date : dueDates) {
        highlightFormat.setBackground(QColor(255, 200, 200));
        m_calendar->setDateTextFormat(date, highlightFormat);
    }

    // Загружаем должников на сегодня
    loadDueBooks(currentDate);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QApplication::quit();
}