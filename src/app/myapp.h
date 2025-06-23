#ifndef MYAPP_H
#define MYAPP_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QHotkey>
#include <QLineEdit>
#include <QListWidget>
#include <QProcess>
#include <QSettings>
#include <QString>
#include <QKeySequence>

class MyApp : public QMainWindow
{
Q_OBJECT

    public : explicit MyApp(QWidget *parent = nullptr);
    ~MyApp();

    int maxItems;
    int fontSize;

public slots:
    void LaunchApp(QListWidgetItem *item);
    void showSearch();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

protected slots:
    void onFocusChanged(QWidget *old, QWidget *now);

private slots:
    void changeHotkey();

private:
    void setupUI();
    void setupTrayIcon();
    void setupShortcut();
    void toggleVisibility();
    void SearchApplications(const QString &texto);
    void adjustWindowSizeForEmptyList();
    void resizeEvent(QResizeEvent *event);

    QSettings *settings;
    QProcess *process;
    QSystemTrayIcon *trayIcon;
    QHotkey *hotkey;
    QLineEdit *lineEdit;
    QListWidget *listWidget;
};

#endif
