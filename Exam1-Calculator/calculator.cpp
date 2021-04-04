#include <QtWidgets>

#include <cmath>

#include "calculator.h"

Calculator::Calculator(QWidget *parent)
    : QDialog(parent)
{
    sumSoFar = 0.0;
    waitingForOperand = true;

    display = new QLineEdit("0");

    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setMaxLength(15);

    QFont font = display->font();
    font.setPointSize(font.pointSize() + 8);
    display->setFont(font);

    mapper = new QSignalMapper();

    for(int i = 0; i < NumDigitButtons; ++i) {
        digitButtons[i] = new Button(QString::number(i));
        connect(digitButtons[i], &QPushButton::clicked, mapper, QOverload<>::of(&QSignalMapper::map));
        mapper->setMapping(digitButtons[i], i);
    }

    connect(mapper, QOverload<int>::of(&QSignalMapper::mapped),
            this, &Calculator::digitClicked);

    Button *minusButton = createButton(tr("-"), SLOT(additiveOperatorClicked()));
    Button *plusButton = createButton(tr("+"), SLOT(additiveOperatorClicked()));
    Button *equalButton = createButton(tr("="), SLOT(equalClicked()));

    auto *quitButton = new Button("Quit");
    connect(quitButton, &QPushButton::clicked, [](){
        qApp->quit();
    });

    QGridLayout *mainLayout = new QGridLayout;

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    mainLayout->addWidget(display, 0, 0, 1, 6);

    for (int i = 1; i < NumDigitButtons; ++i) {
        int row = ((9 - i) / 3) + 2;
        int column = ((i - 1) % 3) + 1;
        mainLayout->addWidget(digitButtons[i], row, column);
    }

    mainLayout->addWidget(digitButtons[0], 5, 2);


    mainLayout->addWidget(quitButton, 2, 4);
    mainLayout->addWidget(minusButton, 4, 4);
    mainLayout->addWidget(plusButton, 3, 4);
    mainLayout->addWidget(equalButton, 5, 4);
    setLayout(mainLayout);

    setWindowTitle(tr("Calculator"));
}

void Calculator::digitClicked(int digitValue)
{
    if (display->text() == "0" && digitValue == 0.0)
        return;

    if (waitingForOperand) {
        display->clear();
        waitingForOperand = false;
    }
    display->setText(display->text() + QString::number(digitValue));
}

void Calculator::additiveOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();

    if (!pendingAdditiveOperator.isEmpty() && !waitingForOperand) {
        calculate(operand, pendingAdditiveOperator);
        display->setText(QString::number(sumSoFar));
    } else {
        sumSoFar = operand;
    }

    pendingAdditiveOperator = clickedOperator;
    waitingForOperand = true;
}


void Calculator::equalClicked()
{
    double operand = display->text().toDouble();
    if (!pendingAdditiveOperator.isEmpty()) {
        if (!calculate(operand, pendingAdditiveOperator)) {
            abortOperation();
            return;
        }
        pendingAdditiveOperator.clear();
    } else {
        sumSoFar = operand;
    }

    display->setText(QString::number(sumSoFar));
    sumSoFar = 0.0;
    waitingForOperand = true;
}

void Calculator::clearAll()
{
    sumSoFar = 0.0;
    pendingAdditiveOperator.clear();
    display->setText("0");
    waitingForOperand = true;
}

Button *Calculator::createButton(const QString &text, const char *member)
{
    Button *button = new Button(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}


void Calculator::abortOperation()
{
    clearAll();
    display->setText(tr("####"));
}


bool Calculator::calculate(double rightOperand, const QString &pendingOperator)
{
    if (pendingOperator == tr("+")) {
        sumSoFar += rightOperand;
    } else if (pendingOperator == tr("-")) {
        sumSoFar -= rightOperand;
    }
    return true;
}
