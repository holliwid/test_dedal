#include "databaseManager.h"
#include <QFileInfo>
#include <QDebug>

DatabaseManager::DatabaseManager(const QString& databasePath) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(databasePath);
}

bool DatabaseManager::openDatabase() {
    QFileInfo fileInfo(db.databaseName());
    if (!fileInfo.exists()) {
        qDebug() << "Database file does not exist.";
        return false;
    }

    if (!db.open()) {
        qDebug() << "Failed to open database.";
        return false;
    }

    return true;
}

QSqlDatabase& DatabaseManager::getDatabase() {
    return db;
}
