#include "application.h"

Application::Application(){}

void Application::run(){
    QString path = QFileDialog::getOpenFileName(nullptr, "Source text", ".", "Text Files(*.txt)");
    if(!path.isEmpty()){
        qApp->exit();
    }
    qDebug() << path;
    QFile file(path);
    file.open(QFile::ReadOnly);
    QTextStream in(&file);

    QVector<QString> cards;
    while (!in.atEnd())
    {
        cards.push_back(in.readLine());
    }
    int size[6] = {
        0,
        cards.size()/5,
        cards.size()/5 * 2,
        cards.size()/5 * 3,
        cards.size()/5 * 4,
        cards.size()
    };
    for(int i=0;i<5;++i){
        auto begin = cards.cbegin() + size[i];
        auto end = cards.cbegin() + size[i + 1];
        QVector<QString> data;
        while(begin != end){
            data.push_back(*begin);
            begin++;
        }
        threads[i] = new CardMerger(data, QFileInfo(file).dir().absolutePath(), i);
        threads[i]->start();
    }
    file.close();
}
