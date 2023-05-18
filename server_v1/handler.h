#ifndef HANDLER_H
#define HANDLER_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTcpSocket>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

class Handler : public QObject {
    Q_OBJECT
public:
    Handler(QSqlDatabase db, QObject *parent = nullptr);

public slots:
    void handleRequest(QTcpSocket *socket, const QString &method, const QString &path, const QByteArray &requestData);

private:
    QSqlDatabase m_db;

    void handleConnection(QTcpSocket *socket);
    void sendResponse(QTcpSocket *socket, int statusCode, const QJsonObject &body);
    void sendErrorResponse(QTcpSocket *socket, int statusCode, const QString &message);
    void handleGetLamps(QTcpSocket *socket);
    void handlePostLamp(QTcpSocket *socket, const QJsonObject &body);
    void handleDeleteLamp(QTcpSocket *socket, int lampId);
    void handlePutLamp(QTcpSocket *socket, int lampId, bool lampOn);
    void handleAddLamp(QTcpSocket *socket, QByteArray requestData);
    void handleTurnOnLamp(QTcpSocket *socket, int lampId);
    void handleTurnOffLamp(QTcpSocket *socket, int lampId);
};

#endif // HANDLER_H
