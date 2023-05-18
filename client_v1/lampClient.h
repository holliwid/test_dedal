#ifndef LAMPCLIENT_H
#define LAMPCLIENT_H

#include "lamp.h"

#include <QWidget>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>  // Add this line
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QInputDialog>
#include <QComboBox>

class LampClient : public QWidget {
    Q_OBJECT
public:
    LampClient(QWidget *parent = nullptr);

private slots:
    void getAllLamps();
    void turnOnLamp();
    void turnOffLamp();
    void addLamp();
    void deleteLamp();
    void sendRequest(const QString& path, const QString& method);
    void handleResponse(QNetworkReply *reply);
    void lampSelected(int index);
    void checkConnection();
    void handleCheckConnection(const QString& method);

signals:
    void requestCompleted();


private:
    QLabel* connectionLabel;
    QLineEdit* ipLineEdit;
    QLineEdit* portLineEdit;
    QPushButton *getAllButton;
    QPushButton *chooseButton;
    QPushButton *turnOnButton;
    QPushButton *turnOffButton;
    QPushButton *deleteButton;
    QPushButton *addButton;
    QVector<Lamp> lamps;
    QComboBox *lampComboBox;
    int selectedLampId;
};

#endif // LAMPCLIENT_H
