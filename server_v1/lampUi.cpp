#include "lampUi.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QSqlQuery>
#include <QFile>
#include <QApplication>

LampUI::LampUI(const QSqlDatabase& database, QWidget* parent)
    : QWidget(parent), db(database)
{
    layout = new QVBoxLayout(this);
    setFixedSize(300, 500);

    createLampLabels();
    updateLamps();
}

void LampUI::createLampLabels() {
    QSqlQuery query(db);
    if (query.exec("SELECT lamp_id, lamp_on FROM lamps")) {
        while (query.next()) {
            int lampId = query.value("lamp_id").toInt();
            bool lampOn = query.value("lamp_on").toBool();

            QLabel* lampLabel = new QLabel(this);
            lampLabel->setText(QString("Lamp %1 (Turn %2)").arg(lampId).arg(lampOn ? "On" : "Off"));

            lampLabels.append(lampLabel);
            layout->addWidget(lampLabel);
        }
    }

    setLayout(layout);
}

void LampUI::updateLamps()
{
    clearLampLabels();
    createLampLabels();

    QSqlQuery query(db);
    if (query.exec("SELECT lamp_id, lamp_on FROM lamps")) {
        int index = 0;
        while (query.next()) {
            int lampId = query.value("lamp_id").toInt();
            bool lampOn = query.value("lamp_on").toBool();

            QLabel* lampLabel = lampLabels.at(index);
            lampLabel->setText(QString("Lamp %1 (Turn %2)").arg(lampId).arg(lampOn ? "On" : "Off"));

            QString styleSheet;
            if (lampOn) {
                styleSheet = "background-color: green; border: 1px solid black";
            } else {
                styleSheet = "background-color: red; border: 1px solid black";
            }
            lampLabel->setStyleSheet(styleSheet);

            index++;
        }
    }
}

void LampUI::clearLampLabels() {
    if (!lampLabels.isEmpty()) {
        for (QLabel* lampLabel : lampLabels) {
            layout->removeWidget(lampLabel);
            delete lampLabel;
        }
        lampLabels.clear();
    }
}








