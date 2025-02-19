#include <QApplication>
#include "app/myapp.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MyApp window;
    return app.exec();
}