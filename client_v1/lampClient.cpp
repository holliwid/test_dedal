#include "lampClient.h"
#include "lamp.h"

#include <QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QComboBox>
#include <QEventLoop>
#include <QLabel>

LampClient::LampClient(QWidget *parent) : QWidget(parent) {
    // Set up the UI
    setFixedSize(300, 500);

    ipLineEdit = new QLineEdit(this);
    ipLineEdit->setGeometry(50, 15, 200, 30);
    ipLineEdit->setText("localhost"); // Set the default IP address

    portLineEdit = new QLineEdit(this);
    portLineEdit->setGeometry(50, 50, 200, 30);
    portLineEdit->setText("8080"); // Set the default port

    connectionLabel = new QLabel(this);
    connectionLabel->setGeometry(50, 90, 200, 30);
    connectionLabel->setStyleSheet("color: green");


    QPushButton *getAllButton = new QPushButton("Connect", this);
    getAllButton->setGeometry(50, 130, 200, 30);
    connect(getAllButton, &QPushButton::clicked, this, &LampClient::checkConnection);

    lampComboBox = new QComboBox(this);
    lampComboBox->setGeometry(50, 170, 200, 30);
    connect(lampComboBox, QOverload<int>::of(&QComboBox::activated), this, &LampClient::lampSelected);

    QPushButton *turnOnButton = new QPushButton("Turn On", this);
    turnOnButton->setGeometry(50, 210, 200, 30);
    connect(turnOnButton, &QPushButton::clicked, this, &LampClient::turnOnLamp);

    QPushButton *turnOffButton = new QPushButton("Turn Off", this);
    turnOffButton->setGeometry(50, 250, 200, 30);
    connect(turnOffButton, &QPushButton::clicked, this, &LampClient::turnOffLamp);

    QPushButton *deleteButton = new QPushButton("Delete Lamp", this);
    deleteButton->setGeometry(50, 290, 200, 30);
    connect(deleteButton, &QPushButton::clicked, this, &LampClient::deleteLamp);

    QPushButton *addButton = new QPushButton("Add Lamp", this);
    addButton->setGeometry(50, 330, 200, 30);
    connect(addButton, &QPushButton::clicked, this, &LampClient::addLamp);


}

void LampClient::checkConnection() {
    handleCheckConnection("GET");
    getAllLamps();
}

void LampClient::getAllLamps() {
    // Send a GET request to retrieve all lamps
    sendRequest("/lamps", "GET");

}


void LampClient::turnOnLamp() {
    if (selectedLampId != 0) {
        // Send a PUT request to turn on the chosen lamp
        sendRequest("/lamps/" + QString::number(selectedLampId) + "/on", "PUT");

        QEventLoop loop;
        connect(this, &LampClient::requestCompleted, &loop, &QEventLoop::quit);
        loop.exec();

    } else {
        QMessageBox::warning(this, "No Lamp Selected", "Please select a lamp before turning it on.");
    }
    getAllLamps();
}

void LampClient::turnOffLamp() {
    if (selectedLampId != 0) {
        // Send a PUT request to turn off the chosen lamp
        sendRequest("/lamps/" + QString::number(selectedLampId) + "/off", "PUT");

        QEventLoop loop;
        connect(this, &LampClient::requestCompleted, &loop, &QEventLoop::quit);
        loop.exec();

    } else {
        QMessageBox::warning(this, "No Lamp Selected", "Please select a lamp before turning it off.");
    }
    getAllLamps();
}


void LampClient::addLamp() {
    // Send a POST request to add a new lamp
    sendRequest("/lamps", "POST");
    QEventLoop loop;
    connect(this, &LampClient::requestCompleted, &loop, &QEventLoop::quit);
    loop.exec();
    getAllLamps();
}

void LampClient::deleteLamp() {
    if (selectedLampId != 0) {
        // Send a DELETE request to delete the chosen lamp
        sendRequest("/lamps/" + QString::number(selectedLampId), "DELETE");

        // Wait for the request to complete
        QEventLoop loop;
        connect(this, &LampClient::requestCompleted, &loop, &QEventLoop::quit);
        loop.exec();

        // Perform the subsequent operations after the request is completed
        getAllLamps();
    } else {
        QMessageBox::warning(this, "No Lamp Selected", "Please select a lamp before deleting it.");
    }
}

void LampClient::handleCheckConnection(const QString& method) {
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QString ipAddress = ipLineEdit->text();
    QString port = portLineEdit->text();
    QUrl url("http://" + ipAddress + ":" + port);
    QNetworkRequest request(url);

    QNetworkReply *reply = nullptr;

    if (method == "GET") {
        reply = manager->get(request);

        connect(reply, &QNetworkReply::finished, [=]() {
            if (reply->error() == QNetworkReply::NoError) {
                connectionLabel->setText("Connection succeeded");
            } else{
                connectionLabel->setText("");
            }

            reply->deleteLater();
        });
    }
}

void LampClient::sendRequest(const QString& path, const QString& method) {
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QString ipAddress = ipLineEdit->text();
    QString port = portLineEdit->text();
    QUrl url("http://" + ipAddress + ":" + port + path);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = nullptr;

    if (method == "GET") {
        reply = manager->get(request);
    } else if (method == "PUT") {
        reply = manager->put(request, "");
    } else if (method == "DELETE") {
        reply = manager->deleteResource(request);
    } else if (method == "POST") {
        const QByteArray data;
        reply = manager->post(request, data);
    }

    if (reply) {
        connect(reply, &QNetworkReply::finished, [=]() {
            handleResponse(reply);
            reply->deleteLater();
        });
    } else {
        QMessageBox::critical(this, "Error", "Failed to send request.");
    }
}

void LampClient::handleResponse(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

        if (jsonDoc.isObject()) {
            QJsonObject jsonObject = jsonDoc.object();

            if (jsonObject.contains("lamps") && jsonObject["lamps"].isArray()) {
                QJsonArray jsonArray = jsonObject["lamps"].toArray();

                // Create an array to store Lamp objects
                QVector<Lamp> lamps;

                // Iterate over the JSON array
                for (const QJsonValue &value : jsonArray) {
                    if (value.isObject()) {
                        QJsonObject lampObj = value.toObject();

                        // Extract lamp data from the JSON object
                        int lampId = lampObj["lamp_id"].toInt();
                        bool lampOn = lampObj["lamp_on"].toBool();

                        // Create a Lamp object and add it to the array
                        Lamp lamp(lampId, lampOn);
                        lamps.append(lamp);
                    }
                }
                if (lamps.length() != 0){
                    selectedLampId = lamps[0].getId();
                }


                // Update the QComboBox with the lamp names
                QStringList lampNames;
                for (const Lamp &lamp : lamps) {
                    lampNames.append("Lamp №" + QString::number(lamp.getId()) + (lamp.isOn() ? " (On)" : " (Off)"));
                }

                lampComboBox->clear();
                lampComboBox->addItems(lampNames);
            }
        }
    } else {
        QMessageBox::critical(this, "Error", "Request failed: " + reply->errorString());
        emit requestCompleted();

    }
    emit requestCompleted();
}


void LampClient::lampSelected(int index) {
    QComboBox* comboBox = qobject_cast<QComboBox*>(sender());
    if (comboBox) {
        QString selectedItem = comboBox->itemText(index);
        QRegularExpression regex("Lamp №(\\d+)");
        QRegularExpressionMatch match = regex.match(selectedItem);
        if (match.hasMatch()) {
            QString lampIdStr = match.captured(1);
            selectedLampId = lampIdStr.toInt();

            // Display the selected lamp ID and name
            //            QMessageBox::information(this, "Lamp Selected", "Lamp ID: " + QString::number(selectedLampId) + "\nLamp Name: " + selectedItem);
        }
    }
}
