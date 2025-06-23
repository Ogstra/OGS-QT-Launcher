#include <QApplication>
#include <QLocalServer>
#include <QLocalSocket>
#include "app/myapp.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QLocalSocket socket;
    socket.connectToServer("ogs_launcher_server");
    if (socket.waitForConnected(100))
    {
        socket.write("show");
        socket.flush();
        socket.disconnectFromServer();
        return 0;
    }

    QLocalServer server;
    QLocalServer::removeServer("ogs_launcher_server");
    server.listen("ogs_launcher_server");

    MyApp window;

    QObject::connect(&server, &QLocalServer::newConnection, [&]() {
        QLocalSocket *client = server.nextPendingConnection();
        if (client)
        {
            client->readAll();
            window.showSearch();
            client->disconnectFromServer();
        }
    });

    return app.exec();
}