#include <QCoreApplication>

#include "fileserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    FileServer *server = new FileServer();
    return a.exec();
}
