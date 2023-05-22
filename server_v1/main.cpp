#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QApplication>
#include <QFileInfo>
#include <QVBoxLayout>
#include "handler.h"
#include "databaseManager.h"
#include "lampUi.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Create and configure the TCP server
    QTcpServer server;
    if (!server.listen(QHostAddress::Any, 8080)) {
        qDebug() << "Failed to start server.";
        return -1;
    }
    qDebug() << "Server started on port" << server.serverPort();

    // Create the database manager
    DatabaseManager dbManager("/home/san/qt/dedal/server_v1/lamps.db");

    // Open the database
    if (!dbManager.openDatabase()) {
        qDebug() << "Failed to open the database.";
        return -1;
    }

    // Get a reference to the database
    QSqlDatabase& db = dbManager.getDatabase();

    // Create the handler instance
    Handler handler(db);

    // Create the LampUI instance
    LampUI lampUI(db);

    // Create the main application window
    QWidget mainWindow;
    QVBoxLayout layout(&mainWindow);
    layout.addWidget(&lampUI);
    mainWindow.setLayout(&layout);
    mainWindow.show();

    QObject::connect(&server, &QTcpServer::newConnection, [&]() {
        while (server.hasPendingConnections()) {
            QTcpSocket* socket = server.nextPendingConnection();
            QObject::connect(socket, &QTcpSocket::readyRead, [&, socket]() {
                // Read the incoming request
                QByteArray requestData = socket->readAll();

                // Extract the method and path from the request
                QList<QByteArray> lines = requestData.split('\n');
                QList<QByteArray> parts = lines.first().split(' ');
                if (parts.size() != 3) {
                    // Invalid request
                    socket->write("HTTP/1.1 400 Bad Request\r\n\r\n");
                    socket->flush();
                    socket->close();
                    return;
                }
                QString method = parts[0];
                QString path = parts[1];

                // Handle the request
                handler.handleRequest(socket, method, path, requestData);

                // Update the lamp labels
                lampUI.updateLamps();
            });
        }
    });

    return app.exec();
}
