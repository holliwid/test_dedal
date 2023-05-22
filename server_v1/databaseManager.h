#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>

class DatabaseManager {
public:
    DatabaseManager(const QString& databasePath);
    bool openDatabase();
    QSqlDatabase& getDatabase();

private:
    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H
