#include <QApplication>
#include <QSettings>
#include <QLocalSocket>
#include <QLocalServer>

#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QString serverName = "qwapp_403498328432";

    QLocalSocket socket;
    socket.connectToServer(serverName);

    if (socket.waitForConnected(500)) {
        socket.write("RAISE_WINDOW");
        socket.waitForBytesWritten();
        return 0;
    }

    QLocalServer::removeServer(serverName);
    QLocalServer server;

    if (!server.listen(serverName)) {
        return 1;
    }

    QSettings settings("com.404coding", "QWApp");
    app.setApplicationName("QWApp");

    auto *mainWindow = new MainWindow();

    if (settings.value("startMinimized").toBool()) {
        mainWindow->hide();
    } else {
        mainWindow->show();
    }

    QObject::connect(&server, &QLocalServer::newConnection, [&]() {
        QLocalSocket *clientSocket = server.nextPendingConnection();
        if (clientSocket->waitForReadyRead(500)) {
            QByteArray data = clientSocket->readAll();
            if (data == "RAISE_WINDOW") {
                mainWindow->show();
                mainWindow->raise();
                mainWindow->activateWindow();
            }
        }
        clientSocket->deleteLater();
    });

    return app.exec();
}
