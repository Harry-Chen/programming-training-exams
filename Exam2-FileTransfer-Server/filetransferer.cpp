#include "filetransferer.h"
#include <QDebug>

FileTransferer::FileTransferer(QTcpSocket *socket1, QTcpSocket *socket2)
{
    socket[0] = socket1;
    socket[1] = socket2;
    clientStates[0] = clientStates[1] = State::IDLE;
    buffer[0] = new QByteArray();
    buffer[1] = new QByteArray();
}

void FileTransferer::run()
{
    connect(socket[0], &QTcpSocket::readyRead, this, [=](){
        this->readData(0);
    }, Qt::DirectConnection);
    connect(socket[1], &QTcpSocket::readyRead, this, [=](){
        this->readData(1);
    }, Qt::DirectConnection);
    connect(this, &FileTransferer::fileReceived, [=](int id){
        int opposite = 1 - id;
        timers[opposite] = new QTimer();
        timers[opposite]->start(10);
        connect(timers[opposite], &QTimer::timeout, [=](){
            this->checkCanTransferTo(opposite);
        });
    });
    connect(socket[0], &QTcpSocket::bytesWritten, [=](qint64 bytes){
        this->transferNextData(0, bytes);
    });
    connect(socket[1], &QTcpSocket::bytesWritten, [=](qint64 bytes){
        this->transferNextData(1, bytes);
    });
    connect(this, &FileTransferer::transferTo, &FileTransferer::sendData);
    this->exec();
}

void FileTransferer::readData(int id)
{
    if(buffer[id] == nullptr){
        buffer[id] = new QByteArray();
    }
    clientStates[id] = State::SENDING;
    QTcpSocket *connection = socket[id];
    QDataStream in(connection);
    auto &bytesReceived = this->bytesReceived[id];
    auto &fileNameSize = this->fileNameSize[id];
    auto &fileName = this->fileName[id];
    auto &totalBytes = this->totalBytes[id];
    auto &buffer = this->buffer[id];
    auto &percent = this->percent[id];

    if(bytesReceived <= sizeof(qint64)*2){
        if(connection->bytesAvailable() >= sizeof(qint64)*2 && fileNameSize == 0){
            in >> totalBytes >> fileNameSize;
            bytesReceived += sizeof(qint64) * 2;
        }
        if(connection->bytesAvailable() >= fileNameSize && fileNameSize != 0){
            in >> fileName;
            qDebug() << "File name" << fileName << "size" << totalBytes;
            bytesReceived += fileNameSize;
        }
        else return;
    }

    if(bytesReceived < totalBytes){
        bytesReceived += connection->bytesAvailable();
        *buffer += connection->readAll();
    }

    int newPercent = bytesReceived * 100 / totalBytes;
    if(newPercent != percent){
        qDebug() << "Receiving" << fileName << QString::number(newPercent) << "%";
        percent = newPercent;
    }

    if(bytesReceived == totalBytes){
        clientStates[id] = State::IDLE;
        fileToSend[1 - id] = this->buffer[id];
        this->buffer[id] = nullptr;
        emit fileReceived(id);
    }
}

void FileTransferer::checkCanTransferTo(int client)
{
    if(clientStates[client] == State::IDLE){
        filenameToSend[client] = fileName[1 - client];
        timers[client]->stop();
        delete timers[client];
        timers[client] = nullptr;
        emit transferTo(client);
    }
    else{
        return;
    }
}

void FileTransferer::sendData(int client)
{
    qDebug() << "Start sending data to another client";
    clientStates[client] = State::RECEIVING;
    totalBytes[client] = fileToSend[client]->size();
    if(buffer[client] == nullptr){
        buffer[client] = new QByteArray();
    }
    buffer[client]->resize(0);
    QDataStream send(buffer[client], QIODevice::WriteOnly);
    send << qint64(0) << qint64(0) << filenameToSend[client];
    totalBytes[client] += buffer[client]->size();
    send.device()->seek(0);
    send << totalBytes[client] << qint64(buffer[client]->size() - sizeof(qint64) * 2);
    bytesToWrite[client] = totalBytes[client] - socket[client]->write(*buffer[client]);
    fileOffset[client] = buffer[client]->size();
    buffer[client]->resize(0);
    bytesWritten[client] = 0;
}

void FileTransferer::transferNextData(int client, qint64 bytes)
{
    bytesWritten[client] += bytes;
    if(bytesToWrite > 0){
        qint64 length = qMin(bytesToWrite[client], bufferSize);
        //buffer[client] = fileToSend[client]->re
        buffer[client]->setRawData(fileToSend[client]->data() + bytesWritten[client] - fileOffset[client], length);
        int size = socket[client]->write(*buffer[client]);
        bytesToWrite[client] -= size;
        buffer[client]->resize(0);
    } else{
        //file->close();
    }
    if(bytesWritten[client] == totalBytes[client]){
        qDebug() << "File sent";
        clientStates[client] = State::IDLE;
        delete fileToSend[client];
        fileToSend[client] = nullptr;
        //file->close();
    }
}
