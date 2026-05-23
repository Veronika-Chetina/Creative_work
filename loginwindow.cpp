#include "loginwindow.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QHeaderView>
#include <QDebug>

// Конструктор окна авторизации
LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_mainWindow(nullptr)
{
    setWindowTitle("Библиотечная система - Вход");
    setMinimumSize(800, 500);
    setupUI();      // Настройка интерфейса
    loadUsers();    // Загрузка пользователей из файла
}

LoginWindow::~LoginWindow() {}

// Настройка пользовательского интерфейса
void LoginWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);  // Горизонтальное расположение

    // ========== ЛЕВАЯ ПАНЕЛЬ - ВХОД В СИСТЕМУ ==========
    QGroupBox *loginGroup = new QGroupBox("Вход в систему");
    QVBoxLayout *loginLayout = new QVBoxLayout(loginGroup);

    QLabel *loginLabel = new QLabel("Логин:");
    m_loginEdit = new QLineEdit();
    QLabel *passwordLabel = new QLabel("Пароль:");
    m_passwordEdit = new QLineEdit();
    m_passwordEdit->setEchoMode(QLineEdit::Password);  // Скрытие вводимых символов

    m_loginButton = new QPushButton("Войти");
    m_registerButton = new QPushButton("Управление пользователями");

    loginLayout->addWidget(loginLabel);
    loginLayout->addWidget(m_loginEdit);
    loginLayout->addWidget(passwordLabel);
    loginLayout->addWidget(m_passwordEdit);
    loginLayout->addWidget(m_loginButton);
    loginLayout->addWidget(m_registerButton);
    loginLayout->addStretch();  // Растягивающийся элемент для выравнивания

    // ========== ПРАВАЯ ПАНЕЛЬ - УПРАВЛЕНИЕ ПОЛЬЗОВАТЕЛЯМИ (скрыта по умолчанию) ==========
    m_registerWidget = new QWidget();
    QVBoxLayout *registerLayout = new QVBoxLayout(m_registerWidget);

    // Группа добавления нового пользователя
    QGroupBox *addUserGroup = new QGroupBox("Добавление пользователя");
    QFormLayout *formLayout = new QFormLayout(addUserGroup);

    m_newLoginEdit = new QLineEdit();
    m_newPasswordEdit = new QLineEdit();
    m_newPasswordEdit->setEchoMode(QLineEdit::Password);
    m_roleCombo = new QComboBox();
    m_roleCombo->addItems({"Администратор", "Библиотекарь", "Читатель"});

    formLayout->addRow("Логин:", m_newLoginEdit);
    formLayout->addRow("Пароль:", m_newPasswordEdit);
    formLayout->addRow("Роль:", m_roleCombo);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_saveUserButton = new QPushButton("Добавить");
    m_cancelButton = new QPushButton("Отмена");
    buttonLayout->addWidget(m_saveUserButton);
    buttonLayout->addWidget(m_cancelButton);
    formLayout->addRow(buttonLayout);

    // Группа со списком существующих пользователей
    QGroupBox *usersListGroup = new QGroupBox("Список пользователей");
    QVBoxLayout *usersLayout = new QVBoxLayout(usersListGroup);

    m_usersTable = new QTableWidget();
    m_usersTable->setColumnCount(3);
    m_usersTable->setHorizontalHeaderLabels({"Логин", "Роль", ""});  // Третий столбец для кнопки удаления
    m_usersTable->horizontalHeader()->setStretchLastSection(true);
    m_usersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    usersLayout->addWidget(m_usersTable);

    m_deleteUserButton = new QPushButton("Удалить выбранного пользователя");
    usersLayout->addWidget(m_deleteUserButton);

    registerLayout->addWidget(addUserGroup);
    registerLayout->addWidget(usersListGroup);

    // Добавляем обе панели в главный layout
    mainLayout->addWidget(loginGroup);
    mainLayout->addWidget(m_registerWidget);
    mainLayout->setStretch(0, 1);   // Левая панель - 1 часть
    mainLayout->setStretch(1, 2);   // Правая панель - 2 части

    m_registerWidget->hide();       // Скрываем панель управления по умолчанию

    // ========== ПОДКЛЮЧЕНИЕ СИГНАЛОВ ==========
    connect(m_loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(m_registerButton, &QPushButton::clicked, this, &LoginWindow::onShowRegister);
    connect(m_saveUserButton, &QPushButton::clicked, this, &LoginWindow::onRegisterUser);
    connect(m_cancelButton, &QPushButton::clicked, this, &LoginWindow::onCancelRegister);
    connect(m_deleteUserButton, &QPushButton::clicked, this, &LoginWindow::onDeleteUser);
    connect(m_usersTable, &QTableWidget::itemSelectionChanged, this, &LoginWindow::onUserSelected);
}

// Загрузка пользователей из файла database/Пользователи.txt
void LoginWindow::loadUsers()
{
    m_users.clear();
    QFile file("database/Пользователи.txt");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split('|');
            if (parts.size() == 3) {
                User user;
                user.login = parts[0];
                user.password = parts[1];
                user.role = parts[2];
                m_users.append(user);
            }
        }
        file.close();
    }

    updateUsersTable();
}

// Сохранение пользователей в файл
void LoginWindow::saveUsers()
{
    QFile file("database/Пользователи.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const User& user : m_users) {
            out << user.login << "|" << user.password << "|" << user.role << "\n";
        }
        file.close();
    }
}

// Обновление таблицы пользователей
void LoginWindow::updateUsersTable()
{
    m_usersTable->setRowCount(m_users.size());
    for (int i = 0; i < m_users.size(); ++i) {
        m_usersTable->setItem(i, 0, new QTableWidgetItem(m_users[i].login));
        m_usersTable->setItem(i, 1, new QTableWidgetItem(m_users[i].role));

        QPushButton *deleteBtn = new QPushButton("Удалить");
        // ИСПРАВЛЕНО: захватываем login для поиска, а не индекс
        QString loginToDelete = m_users[i].login;
        connect(deleteBtn, &QPushButton::clicked, [this, loginToDelete]() {
            // Ищем пользователя по логину
            for (int j = 0; j < m_users.size(); ++j) {
                if (m_users[j].login == loginToDelete && m_users[j].login != "admin") {
                    m_users.removeAt(j);
                    saveUsers();
                    loadUsers();
                    QMessageBox::information(this, "Успех", "Пользователь удален!");
                    return;
                }
            }
            if (loginToDelete == "admin") {
                QMessageBox::warning(this, "Ошибка", "Нельзя удалить администратора!");
            }
        });
        m_usersTable->setCellWidget(i, 2, deleteBtn);
    }
    m_usersTable->resizeColumnsToContents();
}

// Обработчик нажатия кнопки "Войти"
void LoginWindow::onLoginClicked()
{
    QString login = m_loginEdit->text();
    QString password = m_passwordEdit->text();

    // Поиск пользователя с введёнными логином и паролем
    for (const User& user : m_users) {
        if (user.login == login && user.password == password) {
            // Создаём главное окно, передавая логин и роль
            m_mainWindow = new MainWindow(login, user.role);
            m_mainWindow->show();
            this->hide();  // Скрываем окно авторизации
            return;
        }
    }

    QMessageBox::warning(this, "Ошибка", "Неверный логин или пароль!");
}

// Показать/скрыть панель управления пользователями
void LoginWindow::onShowRegister()
{
    if (m_registerWidget->isVisible()) {
        m_registerWidget->hide();
        m_registerButton->setText("Управление пользователями");
        setFixedSize(400, 300);  // Уменьшаем окно
    } else {
        m_registerWidget->show();
        m_registerButton->setText("Скрыть");
        setFixedSize(800, 500);  // Увеличиваем окно
        loadUsers();             // Обновляем список пользователей
    }
}

// Добавление нового пользователя
void LoginWindow::onRegisterUser()
{
    QString login = m_newLoginEdit->text().trimmed();
    QString password = m_newPasswordEdit->text().trimmed();
    QString role = m_roleCombo->currentText();

    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля!");
        return;
    }

    // Проверка на уникальность логина
    for (const User& user : m_users) {
        if (user.login == login) {
            QMessageBox::warning(this, "Ошибка", "Пользователь с таким логином уже существует!");
            return;
        }
    }

    User newUser;
    newUser.login = login;
    newUser.password = password;
    newUser.role = role;
    m_users.append(newUser);
    saveUsers();

    m_newLoginEdit->clear();
    m_newPasswordEdit->clear();

    loadUsers();  // Обновляем таблицу
    QMessageBox::information(this, "Успех", "Пользователь добавлен!");
}

// Удаление выбранного пользователя
void LoginWindow::onDeleteUser()
{
    int row = m_usersTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите пользователя для удаления!");
        return;
    }

    if (m_users[row].login == "admin") {
        QMessageBox::warning(this, "Ошибка", "Нельзя удалить администратора!");
        return;
    }

    if (QMessageBox::question(this, "Подтверждение",
                              QString("Удалить пользователя '%1'?").arg(m_users[row].login)) == QMessageBox::Yes) {
        m_users.removeAt(row);
        saveUsers();
        loadUsers();
        QMessageBox::information(this, "Успех", "Пользователь удален!");
    }
}

// Отмена добавления пользователя и скрытие панели
void LoginWindow::onCancelRegister()
{
    m_newLoginEdit->clear();
    m_newPasswordEdit->clear();
    onShowRegister();  // Также скрываем панель управления
}

void LoginWindow::onUserSelected()
{
    // Пустая реализация - можно добавить логику при выборе пользователя
}