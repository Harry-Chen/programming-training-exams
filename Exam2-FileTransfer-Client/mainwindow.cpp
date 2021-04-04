#include <QFileDialog>
#include <QFileInfo>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connected = false;
    socket = new QTcpSocket();
    ui->buttonSend->setEnabled(false);
    connect(ui->buttonConnect, &QPushButton::clicked, [=](){
        socket->connectToHost(QHostAddress(QHostAddress::LocalHost), 911);
    });
    connect(socket, &QTcpSocket::connected, [=](){
        connected = true;
        ui->buttonConnect->setEnabled(false);
        ui->buttonSend->setEnabled(true);
    });
    connect(ui->buttonSend, &QPushButton::clicked, [=](){
        QString path = QFileDialog::getOpenFileName(nullptr, "Select file to send", ".", "All Files(*.*)");
        if(path.isEmpty()){
            return;
        }
        qDebug() << "fileToSend" << path;
        this->ui->buttonSend->setEnabled(false);
        this->sendFile(path);
    });
    connect(socket, &QTcpSocket::bytesWritten, this, &MainWindow::transferNextData);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::readData);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sendFile(QString path)
{
    file = new QFile(path);
    filename = QFileInfo(*file).fileName();
    file->open(QFile::ReadOnly);
    totalBytes = file->size();
    QDataStream send(&buffer, QIODevice::WriteOnly);
    send << qint64(0) << qint64(0) << filename;
    totalBytes += buffer.size();
    send.device()->seek(0);
    send << totalBytes << qint64(buffer.size() - sizeof(qint64) * 2);
    bytesToWrite = totalBytes - socket->write(buffer);
    buffer.resize(0);
}

void MainWindow::transferNextData(qint64 bytes)
{
    bytesWritten += bytes;
    int newPercent = bytesWritten * 100 / totalBytes;
    if(newPercent != percent){
        percent = newPercent;
        qDebug() << QString("Sent %1% of file %2").arg(percent).arg(filename);
    }
    if(bytesToWrite > 0){
        buffer = file->read(qMin(bytesToWrite, bufferSize));
        int size = socket->write(buffer);
        bytesToWrite -= size;
        buffer.resize(0);
    } else{
        file->close();
    }
    if(bytesWritten == totalBytes){
        qDebug() << "File sent";
        file->close();
        this->ui->buttonSend->setEnabled(true);
        fileNameSize = 0;
    }
}

void MainWindow::readData()
{
    ui->buttonSend->setEnabled(false);
    QDataStream in(socket);
    if(bytesReceived <= sizeof(qint64)*2){
        if(socket->bytesAvailable() >= sizeof(qint64)*2 && fileNameSize == 0){
            in >> totalBytes >> fileNameSize;
            bytesReceived += sizeof(qint64) * 2;
        }
        if(socket->bytesAvailable() >= fileNameSize && fileNameSize != 0){
            in >> filename;
            //emit startReceiving(filename, totalBytes);
            bytesReceived += fileNameSize;

            localFile = new QFile(filename);
            localFile->open(QFile::WriteOnly);
        }
        else return;
    }

    if(bytesReceived < totalBytes){
        bytesReceived += socket->bytesAvailable();
        buffer = socket->readAll();
        localFile->write(buffer);
        buffer.resize(0);
    }

    int newPercent = bytesReceived * 100 / totalBytes;
    if(newPercent != percent){
        percent = newPercent;
        //emit progressUpdate(percent);
    }

    if(bytesReceived == totalBytes){
        localFile->close();
        QString path;
        while(path.isEmpty()){
            path = QFileDialog::getSaveFileName(nullptr, "Select file to save", filename, "All Files(*)");
        }
        qDebug() << "file to save" << path;
        localFile->rename(path);
        ui->buttonSend->setEnabled(true);
    }
}
