#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QCalendarWidget>
#include <QPushButton>
#include <QGroupBox>
#include <QTimer>
#include <QTableWidget>
#include <QLabel>

// Предварительные объявления классов окон
class ReaderWindow;
class BookWindow;
class IssueWindow;
class ReturnWindow;

// Главное окно приложения - центральный узел навигации
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Конструктор принимает имя пользователя и его роль для отображения в заголовке
    MainWindow(const QString& username, const QString& role, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Обработчики пунктов меню и кнопок
    void onBooksAction();        // Открыть окно управления книгами
    void onReadersAction();      // Открыть окно управления читателями
    void onIssueAction();        // Открыть окно выдачи книги
    void onReturnAction();       // Открыть окно приёма книги
    void checkChildWindows();    // Проверка открытых дочерних окон (блокировка кнопок)
    void onDateSelected(const QDate& date);  // Выбор даты в календаре
    void onRefreshDueBooks();    // Обновить список должников

private:
    void setupMenu();            // Создание главного меню
    void setupUI();              // Настройка интерфейса
    void closeEvent(QCloseEvent *event) override;  // Обработка закрытия окна
    void loadDueBooks(const QDate& date);   // Загрузка должников на конкретную дату
    void loadAllDueBooks();      // Загрузка всех должников и подсветка календаря

    // Указатели на дочерние окна (синглтон-подобное поведение)
    ReaderWindow *m_readerWindow;
    BookWindow *m_bookWindow;
    IssueWindow *m_issueWindow;
    ReturnWindow *m_returnWindow;

    // Виджеты главного окна
    QPushButton *m_issueButton;   // Кнопка "Выдать книгу"
    QPushButton *m_returnButton;  // Кнопка "Принять книгу"
    QCalendarWidget *m_calendar;  // Календарь для выбора даты
    QTimer *m_timer;              // Таймер для периодической проверки дочерних окон

    // Виджеты для отображения должников
    QGroupBox *m_dueBooksGroup;   // Группа с информацией о должниках
    QTableWidget *m_dueBooksTable; // Таблица должников
    QLabel *m_selectedDateLabel;  // Метка с выбранной датой
    QPushButton *m_refreshButton; // Кнопка обновления

    QString m_username;   // Имя текущего пользователя
    QString m_userRole;   // Роль текущего пользователя
};

#endif // MAINWINDOW_H