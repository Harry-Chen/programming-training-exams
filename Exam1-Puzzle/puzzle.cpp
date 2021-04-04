#include <QtWidgets>
#include <QHeaderView>
#include <QString>
#include <QVector>
#include <algorithm>
#include <QDebug>
#include <ctime>
#include <QKeyEvent>

#include "puzzle.h"
#include "ui_puzzle.h"

Puzzle::Puzzle(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Puzzle)
{
    ui->setupUi(this);

    srand(time(NULL));
    QGridLayout *mainLayout = new QGridLayout;

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    auto *buttonNew = new QPushButton("新游戏");
    auto *buttonReplay = new QPushButton("重玩");
    auto *buttonManual = new QPushButton("手动输入");

    mainLayout->addWidget(buttonNew, 0, 0);
    mainLayout->addWidget(buttonReplay, 0, 1);
    mainLayout->addWidget(buttonManual, 0, 2);

    connect(buttonNew, &QPushButton::clicked, this, &Puzzle::newRandomGame);
    connect(buttonManual, &QPushButton::clicked, [=](){
        QString text = QInputDialog::getText(nullptr, "手动设置游戏", "请按先行后列的顺序输入游戏，空格则为0，如123456780。");
        if(this->setGame(text)){
            paintPuzzle(game);
            user = game;
        }
        else{
            QMessageBox::warning(nullptr, "错误", "输入的格式不正确！游戏将被禁用");
            paintPuzzle("000000000");
            table->setSelectionMode( QAbstractItemView::NoSelection );
        }
    });
    connect(buttonReplay, &QPushButton::clicked, [=](){
        paintPuzzle(game);
        user = game;
    });

    table = new QTableWidget();
    table->setRowCount(3);
    table->setColumnCount(3);
    table->verticalHeader()->hide();
    table->horizontalHeader()->hide();
    table->horizontalHeader()->setDefaultSectionSize(50);
    table->verticalHeader()->setDefaultSectionSize(50);
    table->setSelectionBehavior( QAbstractItemView::SelectItems );
    table->setSelectionMode( QAbstractItemView::SingleSelection );
    mainLayout->addWidget(table, 1, 0, 3, 3);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    setLayout(mainLayout);

    newRandomGame();
    table->installEventFilter(this);
    installEventFilter(this);
}

void Puzzle::keyPressEvent(QKeyEvent *e)
{
    if(table->currentItem()->text().isEmpty())
        return;
    QChar digit = table->currentItem()->text().at(0);
    int index = user.indexOf(digit);

    qDebug() << index;

    switch(e->key()){
    case Qt::Key::Key_Up:
        if(index >= 3 && user.at(index - 3) == '0'){
            user.replace(index, 1, '0');
            user.replace(index - 3, 1, digit);
        }
        break;
    case Qt::Key::Key_Down:
        if(index <=5 && user.at(index + 3) == '0'){
            user.replace(index, 1, '0');
            user.replace(index + 3, 1, digit);
        }
        break;
    case Qt::Key::Key_Left:
        if(index % 3 >= 1 && user.at(index - 1) == '0'){
            user.replace(index, 1, '0');
            user.replace(index - 1, 1, digit);
        }
        break;
    case Qt::Key::Key_Right:
        if(index % 3 <= 1 && user.at(index + 1) == '0'){
            user.replace(index, 1, '0');
            user.replace(index + 1, 1, digit);
        }
        break;
    }
    qDebug() << user;
    paintPuzzle(user);
    if(user == "123456780"){
        QMessageBox::warning(nullptr, "success", "You have successfully completed the game!");
    }
}

bool Puzzle::eventFilter(QObject *target, QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        keyPressEvent(keyEvent);
        return true;
    }
    return QWidget::eventFilter(target, event);
}

Puzzle::~Puzzle()
{
    delete ui;
}

bool Puzzle::setGame(QString given)
{
    if(given.isNull()){
        QString result;
        QVector<int> numbers;
        for(int i = 0; i < 9;++i){
            numbers.push_back(i);
        }
        std::random_shuffle(numbers.begin(), numbers.end());
        for(int i : numbers){
            result += QString::number(i);
        }
        qDebug() << result;
        game = result;
        return true;
    }
    else{
        if(given.length() != 9){
            return false;
        }
        else{
            for(int i = 0;i < 9;++i){
                if(!given.contains('0' + i)){
                    return false;
                }
            }
            game = given;
            return true;
        }
    }
}

void Puzzle::newRandomGame()
{
    setGame();
    paintPuzzle(game);
    user = game;
}

void Puzzle::paintPuzzle(QString game)
{
    table->setSelectionMode( QAbstractItemView::SingleSelection );
    for(int i = 0;i<3;++i){
        for(int j = 0;j < 3;++j){
            auto *item = new QTableWidgetItem();
            int number = game.at(i * 3 + j).digitValue();
            if(number != 0){
                item->setText(QString::number(number));
            }
            else{
                item->setText(QString());
            }
            table->setItem(i, j, item);
        }
    }
}
