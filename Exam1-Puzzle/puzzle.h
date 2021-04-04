#ifndef PUZZLE_H
#define PUZZLE_H

#pragma execution_character_set("utf-8")

#include <QDialog>
#include <QString>
#include <QtWidgets>

namespace Ui {
class Puzzle;
}

class Puzzle : public QDialog
{
    Q_OBJECT

public:
    explicit Puzzle(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent *e) override;
    bool eventFilter(QObject *target,QEvent *event);
    ~Puzzle();

private:
    Ui::Puzzle *ui;
    bool setGame(QString given = QString());
    void newRandomGame();
    void paintPuzzle(QString game);
    QString game;
    QString user;
    QTableWidget *table;
};

#endif // PUZZLE_H
