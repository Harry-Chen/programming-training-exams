#include "cardmerger.h"
#include <QDebug>
#include <QFile>

CardMerger::CardMerger(QVector<QString>data, QString path, int id)
{
    this->data = data;
    this->path = path;
    this->id = id;
}

void CardMerger::run()
{
    qDebug() << "Thread" << id << "started";
    for (auto card : data){
        int mod = (card.at(card.length() - 1).toLatin1() - '0') % 5;
        queues[id][mod].enqueue(card);
    }
    classifyDone[id] = true;
    qDebug() << "Thread" << id << "merge done";
    while(true){
        bool skip = true;
        for(int i = 0;i<5;++i){
            skip = skip & classifyDone[i];
        }
        if(skip) break;
    }
    qDebug() << "Threads merge done.";
    int card_id = id + 1;
    for(int i = 0;i<5;++i){
        if(card_id == 5) card_id = 0;
        QFile out(path + "/" + QString::number(card_id) + ".txt");
        out.open(QFile::WriteOnly);
        QTextStream outputStream(&out);
        while(queues[i][card_id].size() > 0){
            QString card = queues[i][card_id].dequeue();
            myCard.push_back(card);
            outputStream << card << "\n";
        }
        out.close();
    }
    qDebug() << "Card id" << card_id << myCard.size();

}
