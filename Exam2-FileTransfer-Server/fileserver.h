#ifndef FILESERVER_H
#define FILESERVER_H

#include <QObject>
#include <QtNetwork>

class FileServer : public QObject
{
    Q_OBJECT
public:
    explicit FileServer(QObject *parent = nullptr);
signals:

public slots:

private slots:
    void handleConnection();

private:
    QTcpServer *server;
    QTcpSocket *socket1 = nullptr, *socket2 = nullptr;
};

#endif // FILESERVER_H
