#include "readerwindow.h"
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
#include <QRegularExpression>

// Конструктор окна управления читателями
ReaderWindow::ReaderWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Управление читателями");
    setMinimumSize(1000, 700);
    setupUI();
    loadReaders();  // Загрузка читателей из файла
}

ReaderWindow::~ReaderWindow() {}

// Настройка интерфейса
void ReaderWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);

    // ========== ЛЕВАЯ ПАНЕЛЬ - РЕГИСТРАЦИЯ ЧИТАТЕЛЯ ==========
    QGroupBox *addGroup = new QGroupBox("Регистрация читателя");
    QFormLayout *formLayout = new QFormLayout(addGroup);

    m_nameEdit = new QLineEdit();
    m_passportEdit = new QLineEdit();

    formLayout->addRow("ФИО:", m_nameEdit);
    formLayout->addRow("Паспорт:", m_passportEdit);

    m_addButton = new QPushButton("Зарегистрировать");
    formLayout->addRow(m_addButton);

    connect(m_addButton, &QPushButton::clicked, this, &ReaderWindow::onAddReader);

    // ========== ПРАВАЯ ПАНЕЛЬ ==========
    QWidget *rightPanel = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

    // Строка поиска
    QHBoxLayout *searchLayout = new QHBoxLayout();
    QLabel *searchLabel = new QLabel("Поиск:");
    searchLayout->addWidget(searchLabel);
    m_searchEdit = new QLineEdit();
    searchLayout->addWidget(m_searchEdit);
    rightLayout->addLayout(searchLayout);

    // Таблица читателей
    m_readerTable = new QTableWidget();
    m_readerTable->setColumnCount(2);
    m_readerTable->setHorizontalHeaderLabels({"ФИО", "Паспорт"});
    m_readerTable->horizontalHeader()->setStretchLastSection(true);
    m_readerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    rightLayout->addWidget(m_readerTable);

    // Кнопки управления
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_deleteButton = new QPushButton("Удалить");
    m_saveButton = new QPushButton("Сохранить");
    m_infoButton = new QPushButton("Информация о книгах");
    buttonLayout->addWidget(m_deleteButton);
    buttonLayout->addWidget(m_saveButton);
    buttonLayout->addWidget(m_infoButton);
    rightLayout->addLayout(buttonLayout);

    // Таблица книг выбранного читателя
    QGroupBox *booksGroup = new QGroupBox("Книги на руках");
    QVBoxLayout *booksLayout = new QVBoxLayout(booksGroup);
    m_booksTable = new QTableWidget();
    m_booksTable->setColumnCount(3);
    m_booksTable->setHorizontalHeaderLabels({"Книга", "Дата выдачи", "Срок сдачи"});
    booksLayout->addWidget(m_booksTable);
    rightLayout->addWidget(booksGroup);

    // Добавляем панели в главный layout
    mainLayout->addWidget(addGroup);
    mainLayout->addWidget(rightPanel);
    mainLayout->setStretch(0, 1);
    mainLayout->setStretch(1, 4);

    // Подключение сигналов
    connect(m_deleteButton, &QPushButton::clicked, this, &ReaderWindow::onDeleteReader);
    connect(m_saveButton, &QPushButton::clicked, this, &ReaderWindow::onSaveReaders);
    connect(m_infoButton, &QPushButton::clicked, this, &ReaderWindow::onShowInfo);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &ReaderWindow::onSearchTextChanged);

    // При выборе читателя в таблице автоматически загружаем его книги
    connect(m_readerTable, &QTableWidget::itemSelectionChanged, [this]() {
        int row = m_readerTable->currentRow();
        if (row >= 0) {
            const QList<Reader>& displayReaders = m_filteredReaders.isEmpty() ? m_readers : m_filteredReaders;
            if (row < displayReaders.size()) {
                loadReaderBooks(displayReaders[row].getPassport());
            }
        }
    });
}

// Загрузка читателей из файла
void ReaderWindow::loadReaders()
{
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

    updateReaderTable();
}

// Обновление таблицы читателей
void ReaderWindow::updateReaderTable()
{
    const QList<Reader>& displayReaders = m_filteredReaders.isEmpty() ? m_readers : m_filteredReaders;
    m_readerTable->setRowCount(displayReaders.size());

    for (int i = 0; i < displayReaders.size(); ++i) {
        m_readerTable->setItem(i, 0, new QTableWidgetItem(displayReaders[i].getName()));
        m_readerTable->setItem(i, 1, new QTableWidgetItem(displayReaders[i].getPassport()));
    }

    m_readerTable->resizeColumnsToContents();
}

// Регистрация нового читателя
void ReaderWindow::onAddReader()
{
    // Проверка заполнения полей
    if (m_nameEdit->text().isEmpty() || m_passportEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля!");
        return;
    }

    Reader reader(m_nameEdit->text(), m_passportEdit->text());
    m_readers.append(reader);

    // Создаём пустой файл для читателя (здесь будут храниться его книги)
    QDir().mkpath("users");
    QFile file(QString("users/%1.txt").arg(m_passportEdit->text()));
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.close();  // Просто создаём пустой файл
    }

    // Очищаем поля
    m_nameEdit->clear();
    m_passportEdit->clear();

    updateReaderTable();
    QMessageBox::information(this, "Успех", "Читатель зарегистрирован!");
}

// Удаление выбранного читателя
void ReaderWindow::onDeleteReader()
{
    int row = m_readerTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите читателя для удаления!");
        return;
    }

    const QList<Reader>& displayReaders = m_filteredReaders.isEmpty() ? m_readers : m_filteredReaders;
    if (row < displayReaders.size()) {
        Reader readerToRemove = displayReaders[row];

        // Удаляем из полного списка
        for (int i = 0; i < m_readers.size(); ++i) {
            if (m_readers[i].getPassport() == readerToRemove.getPassport()) {
                m_readers.removeAt(i);
                break;
            }
        }

        // Удаляем из отфильтрованного списка
        if (!m_filteredReaders.isEmpty()) {
            for (int i = 0; i < m_filteredReaders.size(); ++i) {
                if (m_filteredReaders[i].getPassport() == readerToRemove.getPassport()) {
                    m_filteredReaders.removeAt(i);
                    break;
                }
            }
        }

        // Удаляем файл читателя (список его книг)
        QFile::remove(QString("users/%1.txt").arg(readerToRemove.getPassport()));

        updateReaderTable();
        m_booksTable->setRowCount(0);  // Очищаем таблицу книг
    }
}

// Сохранение списка читателей в файл
void ReaderWindow::onSaveReaders()
{
    saveReaders();
    QMessageBox::information(this, "Успех", "Данные сохранены!");
}

void ReaderWindow::saveReaders()
{
    QDir().mkpath("database");
    QFile file("database/Читатели.txt");

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const Reader& reader : m_readers) {
            out << reader.toFileString() << "\n";
        }
        file.close();
    }
}

// Поиск читателей по ФИО или паспорту
void ReaderWindow::onSearchTextChanged(const QString &text)
{
    m_filteredReaders.clear();

    if (text.isEmpty()) {
        updateReaderTable();
        return;
    }

    for (const Reader& reader : m_readers) {
        if (reader.getName().contains(text, Qt::CaseInsensitive) ||
            reader.getPassport().contains(text)) {
            m_filteredReaders.append(reader);
        }
    }

    updateReaderTable();
}

// Показать книги выбранного читателя
void ReaderWindow::onShowInfo()
{
    int row = m_readerTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите читателя!");
        return;
    }

    const QList<Reader>& displayReaders = m_filteredReaders.isEmpty() ? m_readers : m_filteredReaders;
    if (row < displayReaders.size()) {
        loadReaderBooks(displayReaders[row].getPassport());
    }
}

// Загрузка и отображение книг, которые читатель взял на руки
void ReaderWindow::loadReaderBooks(const QString& passport)
{
    m_booksTable->setRowCount(0);
    QFile file(QString("users/%1.txt").arg(passport));

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString line = in.readLine();
        file.close();

        if (!line.isEmpty()) {
            // Формат: "Название;ДатаВозврата;Название2;ДатаВозврата2;..."
            QStringList books = line.split(';', Qt::SkipEmptyParts);
            m_booksTable->setRowCount(books.size() / 2);

            QDate today = QDate::currentDate();

            for (int i = 0; i < books.size(); i += 2) {
                m_booksTable->setItem(i/2, 0, new QTableWidgetItem(books[i]));
                if (i + 1 < books.size()) {
                    m_booksTable->setItem(i/2, 1, new QTableWidgetItem(books[i + 1]));

                    // Вычисляем статус: сколько дней осталось или просрочка
                    QDate date = QDate::fromString(books[i + 1], "dd.MM.yyyy");
                    int daysLeft = today.daysTo(date);
                    QString status;
                    if (daysLeft > 0) {
                        status = QString("%1 дн.").arg(daysLeft);
                    } else if (daysLeft == 0) {
                        status = "Сегодня";
                    } else {
                        status = QString("Просрочена на %1 дн.").arg(-daysLeft);
                    }
                    m_booksTable->setItem(i/2, 2, new QTableWidgetItem(status));
                }
            }
        }
    }

    m_booksTable->resizeColumnsToContents();
}

// Вспомогательный метод для извлечения номера паспорта (используется в другом месте)
QString ReaderWindow::extractPassport(const QString& readerStr)
{
    QRegularExpression re("(\\d+)");
    QRegularExpressionMatch match = re.match(readerStr);
    if (match.hasMatch()) {
        return match.captured(1);
    }
    return "";
}