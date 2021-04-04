#ifndef CARDMERGER_H
#define CARDMERGER_H

#include <QThread>
#include <QVector>
#include <QQueue>
#include <QMutex>
#include <QMutexLocker>

extern QQueue<QString> queues[5][5];
extern bool classifyDone[5];
extern QMutex boolMutex[5];

class CardMerger : public QThread
{
public:
    CardMerger(QVector<QString>data, QString path, int id);

signals:


protected:
    void run() override;
    QVector<QString> data;
    QString path;
    int id;
    QVector<QString> myCard;
};

#endif // CARDMERGER_H
