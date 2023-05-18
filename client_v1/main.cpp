#include <QApplication>
#include "lampClient.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    LampClient client;
    client.show();

    return app.exec();
}
