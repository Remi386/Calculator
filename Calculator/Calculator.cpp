#include "calculator.h"
#include <QtWidgets>
#include "stdafx.h"
#include "MathExpression.h"
#include <string>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent)
{
    display = new QLineEdit;
    display->setMinimumSize(150, 50);

    //ui.setupUi(this);
    QChar bValues[4][4] = {
        {'7', '8', '9', '/'},
        {'4', '5', '6', '*'},
        {'1', '2', '3', '-'},
        {'0', '.', '=', '+'}
    };

    QGridLayout* gLayout = new QGridLayout;
    gLayout->addWidget(display, 0, 0, 1, 4);
    gLayout->addWidget(createButton("CE"), 1, 3);

    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            gLayout->addWidget(createButton(bValues[row][column]), row + 2, column);
        }
    }

    setLayout(gLayout);
    
}

QPushButton* Calculator::createButton(const QString& name)
{
    QPushButton* button = new QPushButton(name);
    button->setMinimumSize(40, 30);
    connect(button, SIGNAL(clicked()),
            this, SLOT(slotButtonClicked()));
    return button;
}

void Calculator::calculate()
{
    std::string expression = display->text().toStdString();
    
    double result = 0;
    try {
        result = Calculate(Parse(expression));
        displayContent = QString::number(result);
    }
    catch (std::exception& ex){
        displayContent = ex.what();
    }
    display->setText(displayContent);
}

void Calculator::slotButtonClicked()
{
    QString buttonContent = ((QPushButton*)sender())->text();

    if (buttonContent == "CE"){
        display->setText("0");
        displayContent = "";
    }
    else if (buttonContent == "=") {
        calculate();
    }
    else {
        displayContent += buttonContent;
        display->setText(displayContent);
    }
}