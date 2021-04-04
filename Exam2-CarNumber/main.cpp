#include <QApplication>
#include <QFileDialog>
#include <QDebug>
#include <QQueue>
#include <QMutex>

#include "application.h"
#include "cardmerger.h"

QQueue<QString> queues[5][5];

bool classifyDone[5];
QMutex boolMutex[5];
QThread *threads[5];


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Application *app = new Application();
    app->run();
    return a.exec();
}
