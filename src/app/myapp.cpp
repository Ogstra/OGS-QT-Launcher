#include "myapp.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QScreen>
#include <QGuiApplication>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QMenu>
#include <QDir>
#include <QLabel>
#include <QFile>
#include <QTextStream>

MyApp::MyApp(QWidget *parent) : QMainWindow(parent)
{
    // Settings
    maxItems = 5;
    fontSize = 20;

    setupUI();
    setupTrayIcon();
    setupShortcut();

    this->hide();
    this->setFocusPolicy(Qt::StrongFocus);
    this->setStyleSheet("border: 0px");

    connect(qApp, &QApplication::focusChanged, this, &MyApp::onFocusChanged);
}

MyApp::~MyApp()
{
    delete trayIcon;
    delete hotkey;
}

void MyApp::setupUI()
{
    setWindowFlags(Qt::FramelessWindowHint);

    QFont font;
    QApplication::setFont(font);
    font.setPointSize(fontSize);

    lineEdit = new QLineEdit(this);
    lineEdit->setFont(font);
    lineEdit->setFrame(false);
    lineEdit->setStyleSheet("QLineEdit { background: transparent; padding: 4px; padding-left: 10px; padding-right: 10px; }");
    lineEdit->setPlaceholderText("Buscar aplicaciones...");
    lineEdit->installEventFilter(this);

    listWidget = new QListWidget(this);
    listWidget->setFont(font);
    listWidget->setStyleSheet("QListWidget { background: transparent; border: none; }");

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 5, 0, 5);
    layout->setSpacing(5);
    layout->addWidget(lineEdit);
    layout->addWidget(listWidget);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    centralWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setCentralWidget(centralWidget);

    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QRect screenRect = QGuiApplication::primaryScreen()->availableGeometry();
    int x = (screenRect.width() - this->width()) / 2;
    int y = (screenRect.height() - this->height()) / 2;
    this->setGeometry(x, y, this->width(), this->height());

    adjustWindowSizeForEmptyList();
    listWidget->hide();

    connect(listWidget, &QListWidget::itemDoubleClicked, this, &MyApp::LaunchApp);
    connect(lineEdit, &QLineEdit::textChanged, this, &MyApp::SearchApplications);
}

void MyApp::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    QPainterPath path;

    path.addRoundedRect(rect(), 10, 10);

    setMask(QRegion(path.toFillPolygon().toPolygon()));
}

void MyApp::setupTrayIcon()
{
    trayIcon = new QSystemTrayIcon(QIcon::fromTheme("system-search"), this);
    QMenu *trayMenu = new QMenu(this);

    QAction *quitAction = new QAction("Salir", this);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
    trayMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();
}

void MyApp::setupShortcut()
{
    hotkey = new QHotkey(QKeySequence("Ctrl+Alt+Q"), true, this);
    connect(hotkey, &QHotkey::activated, this, &MyApp::toggleVisibility);
}

void MyApp::toggleVisibility()
{
    if (this->isVisible())
    {
        this->hide();
        qDebug() << "Toggle Visibility (hide)";
    }
    else
    {
        this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
        this->show();
        lineEdit->setFocus();
        this->activateWindow();
        qDebug() << "Toggle Visibility (show)";
    }
}

void MyApp::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        QListWidgetItem *item = listWidget->currentItem();
        if (item)
        {
            LaunchApp(item);
            this->hide();
        }
    }
    if (event->key() == Qt::Key_Up)
    {
        int currentRow = listWidget->currentRow();
        if (currentRow > 0)
        {
            listWidget->setCurrentRow(currentRow - 1);
        }
    }
    else if (event->key() == Qt::Key_Down)
    {
        int currentRow = listWidget->currentRow();
        if (currentRow < listWidget->count() - 1)
        {
            listWidget->setCurrentRow(currentRow + 1);
        }
    }
    if (event->key() == Qt::Key_Escape)
    {
        this->hide();
    }
    QMainWindow::keyPressEvent(event);
}

// Text auto focus
bool MyApp::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == lineEdit && event->type() == QEvent::FocusIn)
    {
        lineEdit->selectAll();
    }
    return QMainWindow::eventFilter(obj, event);
}

// Hide on focus loss
void MyApp::onFocusChanged(QWidget *old, QWidget *now)
{
    Q_UNUSED(old);
    if (!this->isAncestorOf(now))
    {
        this->hide();
    }
}
void MyApp::SearchApplications(const QString &text)
{
    if (text.isEmpty())
    {
        listWidget->clear();
        listWidget->hide();
        adjustWindowSizeForEmptyList();
        return;
    }

    listWidget->show();
    listWidget->clear();

    QDir directory("/usr/share/applications");
    QStringList files = directory.entryList(QStringList() << "*.desktop", QDir::Files);

    int iconSize = fontSize * 2;
    listWidget->setIconSize(QSize(iconSize, iconSize));
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    int count = 0;
    foreach (const QString &ffile, files)
    {
        if (count >= maxItems)
            break;

        QFile file(directory.filePath(ffile));
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            QString nameApp;
            QString icono;
            bool tieneExec = false;
            bool NoDisplay = false;

            while (!in.atEnd())
            {
                QString line = in.readLine();
                if (line.startsWith("Name=") && line.contains(text, Qt::CaseInsensitive))
                {
                    nameApp = line.mid(5).trimmed();
                }
                if (line.startsWith("Icon="))
                {
                    icono = line.mid(5).trimmed();
                }
                if (line.startsWith("Exec="))
                {
                    tieneExec = true;
                }
                if (line.startsWith("NoDisplay=true"))
                {
                    NoDisplay = true;
                }
            }

            file.close();

            if (!nameApp.isEmpty() && tieneExec && !NoDisplay)
            {

                QWidget *rowWidget = new QWidget();
                QHBoxLayout *rowLayout = new QHBoxLayout(rowWidget);

                QIcon appIcon = QIcon::fromTheme(icono);
                if (appIcon.isNull())
                {
                    appIcon = QIcon::fromTheme("application-x-executable");
                }

                QLabel *iconLabel = new QLabel();
                iconLabel->setAlignment(Qt::AlignCenter);
                iconLabel->setFixedSize(iconSize, iconSize);
                QPixmap appIconPixmap = appIcon.pixmap(QSize(iconSize, iconSize));
                appIconPixmap = appIconPixmap.scaled(iconSize, iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                iconLabel->setPixmap(appIconPixmap);

                QLabel *label = new QLabel(nameApp);
                label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

                rowLayout->addWidget(iconLabel);
                rowLayout->addWidget(label);
                rowLayout->setContentsMargins(10, 0, 10, 0);
                rowLayout->setStretch(1, 1);

                QListWidgetItem *item = new QListWidgetItem();
                item->setSizeHint(QSize(label->width(), iconSize + 10));
                item->setData(Qt::UserRole, nameApp);
                listWidget->addItem(item);
                listWidget->setItemWidget(item, rowWidget);
                count++;
            }
        }
    }

    // Preselect first item
    if (listWidget->count() > 0)
    {
        listWidget->setCurrentRow(0);
    }

    if (listWidget->count() == 0)
    {
        listWidget->clear();
        listWidget->hide();
        adjustWindowSizeForEmptyList();
        return;
    }

    // Adjust QListWidget height based on items
    int totalHeight = 0;
    int itemSpacing = listWidget->spacing();

    for (int i = 0; i < listWidget->count(); ++i)
    {
        QListWidgetItem *item = listWidget->item(i);
        totalHeight += item->sizeHint().height();
    }

    if (listWidget->count() > 0)
    {
        totalHeight += (listWidget->count() - 1) * itemSpacing;
        int listWidgetMargins = listWidget->contentsMargins().top() + listWidget->contentsMargins().bottom();
        totalHeight += listWidgetMargins;
        this->resize(this->width(), totalHeight);

        listWidget->setFixedHeight(totalHeight);
        this->setFixedHeight(lineEdit->sizeHint().height() + totalHeight + layout()->contentsMargins().top() + layout()->contentsMargins().bottom());
    }
}

void MyApp::adjustWindowSizeForEmptyList()
{
    int margins = listWidget->contentsMargins().top() + listWidget->contentsMargins().bottom() + lineEdit->contentsMargins().top() + lineEdit->contentsMargins().bottom();
    int spacing = 12;
    int minHeight = lineEdit->sizeHint().height() + margins + spacing;

    this->setFixedHeight(minHeight);
}

void MyApp::LaunchApp(QListWidgetItem *item)
{
    QString nameApp = item->data(Qt::UserRole).toString();

    if (nameApp.isEmpty())
    {
        qDebug() << "Application name empty";
        return;
    }

    QDir directorio("/usr/share/applications");
    QStringList files = directorio.entryList(QStringList() << "*.desktop", QDir::Files);
    if (files.isEmpty())
    {
        return;
    }

    QString commandTried;
    foreach (const QString &ffile, files)
    {
        QFile file(directorio.filePath(ffile));
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            QString execCommand;
            QString nameffileApp;
            while (!in.atEnd())
            {
                QString line = in.readLine();
                if (line.startsWith("Name="))
                {
                    nameffileApp = line.mid(5).trimmed();
                }
                if (line.startsWith("Exec="))
                {
                    execCommand = line.mid(5).trimmed();
                    commandTried = execCommand;
                    break;
                }
            }
            file.close();

            if (!execCommand.isEmpty() && nameffileApp.compare(nameApp, Qt::CaseInsensitive) == 0)
            {
                qDebug() << "Exec command not found:" << execCommand;

                execCommand.replace("%U", "");
                execCommand.replace("%u", "");
                execCommand.replace("%F", "");
                execCommand.replace("%f", "");

                if (!execCommand.startsWith("/"))
                {
                    execCommand = "/usr/bin/" + execCommand;
                }

                QStringList execParts = execCommand.split(" ", Qt::SkipEmptyParts);
                if (!execParts.isEmpty())
                {
                    QString program = execParts.takeFirst();

                    if (!QFile::exists(program))
                    {
                        qDebug() << "Application not found:" << program;
                        return;
                    }

                    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
                    env.insert("DISPLAY", ":0");

                    QProcess *proc = new QProcess();
                    proc->setProcessEnvironment(env);
                    proc->setWorkingDirectory("/");
                    if (proc->startDetached(program, execParts))
                    {
                        return;
                    }
                }
            }
        }
    }
}