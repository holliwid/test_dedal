#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include "handler.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Create and configure the TCP server
    QTcpServer server;
    if (!server.listen(QHostAddress::Any, 8080)) {
        qDebug() << "Failed to start server.";
        return -1;
    }
    qDebug() << "Server started on port" << server.serverPort();

    // Connect to the SQLite database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("../lamps.db");
    if (!db.open()) {
        qDebug() << "Failed to open database.";
        return -1;
    }

    // Create the handler instance
    Handler handler(db);





    // Create the main application window
    QWidget mainWindow;
    mainWindow.setFixedSize(300, 500);
    QVBoxLayout layout(&mainWindow);

    QList<QLabel*> lampLabels;

    // Create a label for each lamp
    QSqlQuery query(db);
    if (query.exec("SELECT lamp_id, lamp_on FROM lamps")) {
        while (query.next()) {
            int lampId = query.value("lamp_id").toInt();
            bool lampOn = query.value("lamp_on").toBool();

            QLabel* lampLabel = new QLabel(&mainWindow);
            lampLabel->setText(QString("Lamp %1 (Turn %2)").arg(lampId).arg(lampOn ? "On" : "Off"));

            // Set the background color and border
            lampLabel->setStyleSheet(QString("background-color: %1; border: 1px solid black").arg(lampOn ? "green" : "red"));

            layout.addWidget(lampLabel);
            lampLabels.append(lampLabel);
        }
    }

    mainWindow.setLayout(&layout);
    mainWindow.show();

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
                QSqlQuery query(db);

                QLayoutItem* item;
                    while ((item = layout.takeAt(0)) != nullptr) {
                        delete item->widget();
                        delete item;
                    }

                if (query.exec("SELECT lamp_id, lamp_on FROM lamps")) {
                    while (query.next()) {
                        int lampId = query.value("lamp_id").toInt();
                        bool lampOn = query.value("lamp_on").toBool();

                        QLabel* lampLabel = new QLabel(&mainWindow);
                        lampLabel->setText(QString("Lamp %1 (Turn %2)").arg(lampId).arg(lampOn ? "On" : "Off"));

                        // Set the background color and border
                        lampLabel->setStyleSheet(QString("background-color: %1; border: 1px solid black").arg(lampOn ? "green" : "red"));

                        layout.addWidget(lampLabel);
                        lampLabels.append(lampLabel);
                    }
                }
                mainWindow.setLayout(&layout);

            });
        }
    });

    return app.exec();
}
