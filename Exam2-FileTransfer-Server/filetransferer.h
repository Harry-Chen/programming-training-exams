#ifndef FILETRANSFERER_H
#define FILETRANSFERER_H

#include <QThread>
#include <QtNetwork>
#include <QTimer>
#include <QByteArray>

class FileTransferer : public QThread
{
    Q_OBJECT
public:
    FileTransferer(QTcpSocket *socket1, QTcpSocket *socket2);
protected:
    void run() override;
private:
    QTcpSocket *socket[2];
    void readData(int id);
    qint64 totalBytes[2];
    qint64 bytesReceived[2] = {0};
    qint64 bytesWritten[2] = {0};
    qint64 bytesToWrite[2] = {0};
    qint64 fileNameSize[2] = {0};
    qint64 fileOffset[2] = {0};
    int percent[2] = {0};
    QString fileName[2];
    QString filenameToSend[2];
    QFile *localFile;
    QByteArray *buffer[2];
    QByteArray *fileToSend[2];
    qint64 bufferSize = 4 * 1024;
    enum class State {
        IDLE,
        SENDING,
        RECEIVING
    } clientStates[2];
    QTimer *timers[2];
private slots:
    void checkCanTransferTo(int client);
    void sendData(int client);
    void transferNextData(int client, qint64 bytes);
signals:
    void fileReceived(int client);
    void transferTo(int client);
};

#endif // FILETRANSFERER_H
