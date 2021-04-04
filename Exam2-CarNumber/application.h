#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QFileDialog>
#include <QDebug>
#include <QQueue>
#include <QMutex>

#include  "cardmerger.h"


extern bool classifyDone[5];
extern QMutex boolMutex[5];
extern QThread *threads[5];

class Application : public QObject{
    Q_OBJECT
public:
    Application();
    void run();

};

#endif // APPLICATION_H
