#include "fileserver.h"
#include  "filetransferer.h"

FileServer::FileServer(QObject *parent) : QObject(parent)
{
    server = new QTcpServer();
    server->listen(QHostAddress(QHostAddress::LocalHost), 911);
    connect(server, &QTcpServer::newConnection, this, &FileServer::handleConnection, Qt::DirectConnection);

}

void FileServer::handleConnection()
{
    if(socket1 == nullptr){
        qDebug() << "One connection";
        socket1 = server->nextPendingConnection();
    }
    else{
        qDebug() << "Two connections";
        socket2 = server->nextPendingConnection();
        QThread *thread = new FileTransferer(socket1, socket2);
        socket1->setParent(nullptr);
        socket2->setParent(nullptr);
        socket1->moveToThread(thread);
        socket2->moveToThread(thread);
        socket1 = nullptr;
        socket2 = nullptr;
        qDebug() << "Starting New Thread";
        thread->start();
    }

}
