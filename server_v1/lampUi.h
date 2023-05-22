#ifndef LAMPUI_H
#define LAMPUI_H

#include <QWidget>
#include <QSqlDatabase>

class QLabel;
class QVBoxLayout;

class LampUI : public QWidget {
    Q_OBJECT

public:
    explicit LampUI(const QSqlDatabase& database, QWidget* parent = nullptr);
    void updateLamps();
    void clearLampLabels();

private:
    void createLampLabels();

    QVBoxLayout* layout;
    QList<QLabel*> lampLabels;
    QSqlDatabase db;
};

#endif // LAMPUI_H
