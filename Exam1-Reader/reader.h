#ifndef READER_H
#define READER_H

#include <QMainWindow>
#include <QTextBrowser>

namespace Ui {
class Reader;
}

class Reader : public QMainWindow
{
    Q_OBJECT

public:
    explicit Reader(QWidget *parent = 0);
    ~Reader();

public slots:
    void readFile();

private:
    Ui::Reader *ui;
    QTextBrowser *browser = nullptr;
    bool isTextUTF8(char* str,int length);
};

#endif // READER_H
