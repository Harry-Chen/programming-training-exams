#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QDialog>
#include <QSignalMapper>
#include <QLineEdit>

#include "button.h"

class Calculator : public QDialog
{
    Q_OBJECT

public:
    Calculator(QWidget *parent = 0);

private slots:
    void digitClicked(int digitValue);
    void additiveOperatorClicked();
    void equalClicked();
    void clearAll();

private:

    Button *createButton(const QString &text, const char *member);
    void abortOperation();
    bool calculate(double rightOperand, const QString &pendingOperator);

    double sumSoFar;
    QString pendingAdditiveOperator;
    bool waitingForOperand;

    QLineEdit *display;

    enum { NumDigitButtons = 10 };
    Button *digitButtons[NumDigitButtons];

    QSignalMapper *mapper;

};
#endif
