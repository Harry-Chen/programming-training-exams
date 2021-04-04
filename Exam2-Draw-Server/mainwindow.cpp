#include <QtNetwork>
#include <QPaintEvent>
#include <QPainter>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    udp = new QUdpSocket(this);
    udp->bind(QHostAddress::Any, 8000);
    connect(udp, &QUdpSocket::readyRead, this, &MainWindow::receiveData);
    this->setWindowTitle("Exam2-Draw-Server");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    QPainter p;
    p.setPen(QColor(0, 0, 0));
    p.begin(this);
    for(const auto& line : lines){
        for(auto i = 0;i < line.size() - 1;++i){
            p.drawLine(line[i], line[i+1]);
        }
    }
    p.end();
}

void MainWindow::receiveData()
{
    while(udp->hasPendingDatagrams()){
        QByteArray data;
        data.resize(udp->pendingDatagramSize());
        udp->readDatagram(data.data(), data.size());
        int x, y;
        bool start;
        QDataStream in(&data, QIODevice::ReadOnly);
        in >> x >> y >> start;
        QPoint point(x, y);
        if(start){
            QVector<QPoint> line;
            line.push_back(point);
            lines.push_back(line);
        }
        else{
            auto &line = lines.last();
            line.push_back(point);
        }
        update();
    }
}
