#include <QPainter>
#include <QMouseEvent>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    udp = new QUdpSocket();
    this->setWindowTitle("Exam2-Draw-Client");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    QVector<QPoint> line;
    lines.append(line);

    QVector<QPoint> &lastLine = lines.last();
    lastLine.append(e->pos());
    sendInfo(e->pos(), true);
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    if(lines.size() == 0)
    {
        QVector<QPoint> line;
        lines.append(line);
    }

    QVector<QPoint> &lastLine = lines.last();
    //记录该条线当前的位置
    lastLine.append(e->pos());
    //强制重绘
    sendInfo(e->pos(), false);
    update();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    QVector<QPoint> &lastLine = lines.last();
    lastLine.append(e->pos());
    sendInfo(e->pos(), false);
}

void MainWindow::paintEvent(QPaintEvent *e){
    QPainter p(this);
    for(const auto &line : lines){
        for(int j=0; j<line.size()-1; j++){
            p.drawLine(line.at(j), line.at(j+1));
        }
    }
    p.end();
}

void MainWindow::sendInfo(QPoint point, bool start)
{
    QByteArray buffer;
    QDataStream out(&buffer, QIODevice::WriteOnly);
    out << point.x() << point.y() << start;
    udp->writeDatagram(buffer, QHostAddress(QHostAddress::LocalHost), 8000);
}
