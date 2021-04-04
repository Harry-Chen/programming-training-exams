#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtNetwork>
#include <QMainWindow>
#include <QPoint>
#include <QVector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *e) override;

private:
    Ui::MainWindow *ui;
    QUdpSocket *udp;
    QVector<QVector<QPoint>> lines;
    QPoint first;

private slots:
    void receiveData();
};

#endif // MAINWINDOW_H
