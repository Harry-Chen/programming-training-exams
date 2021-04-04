#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    bool connected;
    void sendFile(QString path);
    void transferNextData(qint64 bytes);

    QString filename;
    QFile *file;
    qint64 totalBytes = 0;
    qint64 bytesWritten = 0;
    qint64 bytesToWrite = 0;
    qint64 bufferSize = 4 * 1024;
    int percent = 0;
    QByteArray buffer;

    qint64 bytesReceived = 0;
    qint64 fileNameSize = 0;
    QFile *localFile;

private slots:
    void readData();
};

#endif // MAINWINDOW_H
