#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QTableWidget>
#include <QComboBox>

class MainWindow;  // Предварительное объявление, чтобы избежать циклической зависимости

// Окно авторизации и управления пользователями
class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void onLoginClicked();           // Обработчик нажатия кнопки "Войти"
    void onShowRegister();           // Показать/скрыть панель управления пользователями
    void onRegisterUser();           // Добавление нового пользователя
    void onDeleteUser();             // Удаление выбранного пользователя
    void onCancelRegister();         // Отмена добавления и скрытие панели
    void onUserSelected();           // Обработка выбора пользователя в таблице

private:
    void setupUI();                  // Настройка пользовательского интерфейса
    void loadUsers();                // Загрузка списка пользователей из файла
    void saveUsers();                // Сохранение списка пользователей в файл
    void updateUsersTable();         // Обновление таблицы пользователей

    // Внутренняя структура для хранения данных пользователя
    struct User {
        QString login;      // Логин
        QString password;   // Пароль
        QString role;       // Роль (Администратор/Библиотекарь/Читатель)
    };

    // Виджеты левой панели (авторизация)
    QLineEdit *m_loginEdit;          // Поле ввода логина
    QLineEdit *m_passwordEdit;       // Поле ввода пароля
    QPushButton *m_loginButton;      // Кнопка "Войти"
    QPushButton *m_registerButton;   // Кнопка "Управление пользователями"
    QGroupBox *m_groupBox;
    MainWindow *m_mainWindow;        // Указатель на главное окно

    // Виджеты правой панели (управление пользователями)
    QWidget *m_registerWidget;       // Контейнер панели управления
    QLineEdit *m_newLoginEdit;       // Поле для нового логина
    QLineEdit *m_newPasswordEdit;    // Поле для нового пароля
    QComboBox *m_roleCombo;          // Выбор роли из списка
    QPushButton *m_saveUserButton;   // Кнопка "Добавить"
    QPushButton *m_cancelButton;     // Кнопка "Отмена"
    QPushButton *m_deleteUserButton; // Кнопка "Удалить"
    QTableWidget *m_usersTable;      // Таблица со списком пользователей

    QList<User> m_users;             // Список всех пользователей системы
};

#endif // LOGINWINDOW_H