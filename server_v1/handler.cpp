#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTcpSocket>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include "handler.h"

Handler::Handler(QSqlDatabase db, QObject *parent) : QObject(parent), m_db(db) {}


void Handler::handleRequest(QTcpSocket *socket, const QString &method, const QString &path, const QByteArray &requestData) {
    // Split the path into segments
    QStringList segments = path.split('/', Qt::SkipEmptyParts);

    // Ensure the path has at least one segment
    if (segments.isEmpty()) {
        if (method == "GET"){
            handleConnection(socket);
        }
        sendErrorResponse(socket, 400, "Bad Request");
        return;
    }

    // Retrieve the first segment (endpoint)
    QString endpoint = segments.first();

    // Handle the request based on the endpoint
    if (endpoint == "lamps") {
        // GET /lamps
        if (method == "GET") {
            handleGetLamps(socket);
        }
        // POST /lamps
        else if (method == "POST") {
            handleAddLamp(socket, requestData);
        }
        // DELETE /lamps/:lamp_id
        else if (method == "DELETE" && segments.size() == 2) {
            bool ok;
            int lampId = segments.at(1).toInt(&ok);
            if (ok) {
                handleDeleteLamp(socket, lampId);
            } else {
                sendErrorResponse(socket, 400, "Bad Request");
            }
        }
        // PUT /lamps/:lamp_id/on
        else if (method == "PUT" && segments.size() == 3 && segments.at(2) == "on") {
            bool ok;
            int lampId = segments.at(1).toInt(&ok);
            if (ok) {
                handleTurnOnLamp(socket, lampId);
            } else {
                sendErrorResponse(socket, 400, "Bad Request");
            }
        }
        // PUT /lamps/:lamp_id/off
        else if (method == "PUT" && segments.size() == 3 && segments.at(2) == "off") {
            bool ok;
            int lampId = segments.at(1).toInt(&ok);
            if (ok) {
                handleTurnOffLamp(socket, lampId);
            } else {
                sendErrorResponse(socket, 400, "Bad Request");
            }
        }
        // Invalid endpoint or method
        else {
            sendErrorResponse(socket, 404, "Not Found");
        }
    } else {
        sendErrorResponse(socket, 404, "Not Found");
    }
}

void Handler::handleConnection(QTcpSocket *socket){
    QJsonObject response;
    response["message"] = "Connection successful";

    // Create a QJsonDocument from the response object
    QJsonDocument responseDocument(response);

    // Convert the JSON document to QByteArray
    QByteArray responseData = responseDocument.toJson();

    // Send the HTTP response headers
    socket->write("HTTP/1.1 201 Created\r\n");
    socket->write("Content-Type: application/json\r\n");
    socket->write("Content-Length: " + QByteArray::number(responseData.size()) + "\r\n");
    socket->write("\r\n");

    // Send the response body
    socket->write(responseData);
    socket->flush();
}

void Handler::handleGetLamps(QTcpSocket *socket) {
    // Retrieve the list of lamps from the database
    QList<QJsonObject> lamps;
    QSqlQuery query(m_db);
    if (query.exec("SELECT * FROM lamps")) {
        while (query.next()) {
            QJsonObject lampObject;
            lampObject["lamp_id"] = query.value("lamp_id").toInt();
            lampObject["lamp_on"] = query.value("lamp_on").toBool();
            lamps.append(lampObject);
        }
    } else {
        // Database error
        sendErrorResponse(socket, 500, "Internal Server Error");
        return;
    }

    // Create a QVariantList from the list of lamps
    QVariantList lampsVariantList;
    for (const QJsonObject &lamp : lamps) {
        lampsVariantList.append(QVariant::fromValue(lamp));
    }

    // Create a QJsonArray from the QVariantList
    QJsonArray lampsArray = QJsonArray::fromVariantList(lampsVariantList);

    // Create a response object
    QJsonObject response;
    response["lamps"] = lampsArray;

    // Create a QJsonDocument from the response object
    QJsonDocument responseDocument(response);

    // Convert the JSON document to QByteArray
    QByteArray responseData = responseDocument.toJson();

    // Send the HTTP response headers
    socket->write("HTTP/1.1 200 OK\r\n");
    socket->write("Content-Type: application/json\r\n");
    socket->write("Content-Length: " + QByteArray::number(responseData.size()) + "\r\n");
    socket->write("\r\n");

    // Send the response body
    socket->write(responseData);
    socket->flush();
}

    void Handler::handleAddLamp(QTcpSocket* socket, QByteArray /* requestData */) {
        // Insert the new lamp into the database
        QSqlQuery query(m_db);
        query.prepare("INSERT INTO lamps (lamp_on) VALUES (0)");
        if (!query.exec()) {
            // Database error
            sendErrorResponse(socket, 500, "Internal Server Error");
            return;
        }

        // Create a response object
        QJsonObject response;
        response["message"] = "Lamp added successfully.";

        // Create a QJsonDocument from the response object
        QJsonDocument responseDocument(response);

        // Convert the JSON document to QByteArray
        QByteArray responseData = responseDocument.toJson();

        // Send the HTTP response headers
        socket->write("HTTP/1.1 201 Created\r\n");
        socket->write("Content-Type: application/json\r\n");
        socket->write("Content-Length: " + QByteArray::number(responseData.size()) + "\r\n");
        socket->write("\r\n");

        // Send the response body
        socket->write(responseData);
        socket->flush();
    }




    void Handler::handleDeleteLamp(QTcpSocket *socket, int lampId) {
        // Delete the specified lamp from the database
        QSqlQuery query(m_db);
        query.prepare("DELETE FROM lamps WHERE lamp_id = :lamp_id");
        query.bindValue(":lamp_id", lampId);
        if (!query.exec()) {
            // Database error
            sendErrorResponse(socket, 500, "Internal Server Error");
            return;
        }

        // Create a response object
        QJsonObject response;
        response["message"] = "Lamp deleted successfully.";

        // Create a QJsonDocument from the response object
        QJsonDocument responseDocument(response);

        // Convert the JSON document to QByteArray
        QByteArray responseData = responseDocument.toJson();

        // Send the HTTP response headers
        socket->write("HTTP/1.1 200 OK\r\n");
        socket->write("Content-Type: application/json\r\n");
        socket->write("Content-Length: " + QByteArray::number(responseData.size()) + "\r\n");
        socket->write("\r\n");

        // Send the response body
        socket->write(responseData);
        socket->flush();
    }

    void Handler::handleTurnOnLamp(QTcpSocket *socket, int lampId) {
        // Update the specified lamp's state to on in the database
        QSqlQuery query(m_db);
        query.prepare("UPDATE lamps SET lamp_on = true WHERE lamp_id = :lamp_id");
        query.bindValue(":lamp_id", lampId);
        if (!query.exec()) {
            // Database error
            sendErrorResponse(socket, 500, "Internal Server Error");
            return;
        }

        // Create a response object
        QJsonObject response;
        response["message"] = "Lamp turned on successfully.";

        // Create a QJsonDocument from the response object
        QJsonDocument responseDocument(response);

        // Convert the JSON document to QByteArray
        QByteArray responseData = responseDocument.toJson();

        // Send the HTTP response headers
        socket->write("HTTP/1.1 200 OK\r\n");
        socket->write("Content-Type: application/json\r\n");
        socket->write("Content-Length: " + QByteArray::number(responseData.size()) + "\r\n");
        socket->write("\r\n");

        // Send the response body
        socket->write(responseData);
        socket->flush();
    }

    void Handler::handleTurnOffLamp(QTcpSocket *socket, int lampId) {
        // Update the specified lamp's state to off in the database
        QSqlQuery query(m_db);
        query.prepare("UPDATE lamps SET lamp_on = false WHERE lamp_id = :lamp_id");
        query.bindValue(":lamp_id", lampId);
        if (!query.exec()) {
            // Database error
            sendErrorResponse(socket, 500, "Internal Server Error");
            return;
        }

        // Create a response object
        QJsonObject response;
        response["message"] = "Lamp turned off successfully.";

        // Create a QJsonDocument from the response object
        QJsonDocument responseDocument(response);

        // Convert the JSON document to QByteArray
        QByteArray responseData = responseDocument.toJson();

        // Send the HTTP response headers
        socket->write("HTTP/1.1 200 OK\r\n");
        socket->write("Content-Type: application/json\r\n");
        socket->write("Content-Length: " + QByteArray::number(responseData.size()) + "\r\n");
        socket->write("\r\n");

        // Send the response body
        socket->write(responseData);
        socket->flush();
    }
void Handler:: sendErrorResponse(QTcpSocket *socket, int statusCode, const QString &message) {
    // Create an error response object
    QJsonObject response;
    response["statusCode"] = statusCode;
    response["message"] = message;
    QJsonDocument errorDocument(response);

            // Convert the JSON document to QByteArray
            QByteArray errorData = errorDocument.toJson();

            // Send the HTTP response headers
            socket->write("HTTP/1.1 " + QByteArray::number(statusCode) + " " + message.toUtf8() + "\r\n");
            socket->write("Content-Type: application/json\r\n");
            socket->write("Content-Length: " + QByteArray::number(errorData.size()) + "\r\n");
            socket->write("\r\n");

            // Send the response body
            socket->write(errorData);
            socket->flush();
        }
