#include <QAction>
#include <QFileDialog>
#include <QTextStream>
#include <QTextCodec>
#include <QDebug>

#include "reader.h"
#include "ui_reader.h"

Reader::Reader(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Reader)
{
    ui->setupUi(this);

    this->browser = new QTextBrowser(this);
    setCentralWidget(browser);
    browser->hide();

    auto *fileMenu = menuBar()->addMenu(tr("&File"));

    auto *openAction = new QAction(tr("&Open"), this);
    auto *closeAction = new QAction(tr("&Close"), this);
    auto *exitAction = new QAction(tr("&Exit"), this);

    openAction ->setShortcut(QKeySequence(Qt::ALT + Qt::Key_O));
    closeAction ->setShortcut(QKeySequence(Qt::ALT + Qt::Key_C));
    exitAction ->setShortcut(QKeySequence(Qt::ALT + Qt::Key_X));

    fileMenu->addAction(openAction);
    fileMenu->addAction(closeAction);
    fileMenu->addAction(exitAction);

    connect(openAction, &QAction::triggered, this, &Reader::readFile);

    connect(closeAction, &QAction::triggered, [=](){
        browser->hide();
    });

    connect(exitAction, &QAction::triggered, [=](){
        qApp->exit();
    });
}

Reader::~Reader()
{
    delete ui;
}

void Reader::readFile()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open Text File"), ".", tr("Text Files(*.txt)"));
    if(!path.isNull()) {
        QFile file(path);
        file.open(QIODevice::ReadOnly);
        auto array = file.readAll();
        QTextStream s(&array);
        if(isTextUTF8(array.data(), array.length())){
            qDebug() << "UTF-8";
            // NOT WORKING
        }
        if(file.fileName().contains("Chinese")){
            s.setCodec(QTextCodec::codecForName("GBK"));
        }
        else{
            s.setCodec(QTextCodec::codecForName("UTF-8"));
        }
        QString content = s.readAll();
        browser->clear();
        browser->setText(content);
        browser->show();
    }
}


bool Reader::isTextUTF8(char* str,int length)
{
    int i;
    int nBytes=0;//UFT8??????1-6???????????????,ASCII???????????????
    char chr;
    bool bAllAscii=true; //??????????????????ASCII, ????????????UTF-8
    for(i=0;i<length;i++)
    {
        chr= *(str+i);
        if( (chr&0x80) != 0 ) // ????????????ASCII??????,????????????,??????????????????UTF-8,ASCII???7?????????,?????????????????????,??????????????????0,o0xxxxxxx
            bAllAscii= false;
        if(nBytes==0) //????????????ASCII???,?????????????????????,???????????????
        {
            if(chr>=0x80)
            {
                if(chr>=0xFC&&chr<=0xFD)
                    nBytes=6;
                else if(chr>=0xF8)
                    nBytes=5;
                else if(chr>=0xF0)
                    nBytes=4;
                else if(chr>=0xE0)
                    nBytes=3;
                else if(chr>=0xC0)
                    nBytes=2;
                else
                {
                    return false;
                }
                nBytes--;
            }
        }
        else //???????????????????????????,?????? 10xxxxxx
        {
            if( (chr&0xC0) != 0x80 )
            {
                return false;
            }
            nBytes--;
        }
    }
    if( nBytes > 0 ) //????????????
    {
        return false;
    }
    if( bAllAscii ) //??????????????????ASCII, ????????????UTF-8
    {
        return false;
    }
    return true;
}
