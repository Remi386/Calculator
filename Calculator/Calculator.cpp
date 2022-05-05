#include "calculator.h"
#include <QtWidgets>
#include "stdafx.h"
#include "MathExpression.h"
#include <string>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent)
{
    //ui.setupUi(this);

    display = new QLineEdit;
    display->setMinimumSize(150, 50);

    display->setAlignment(Qt::AlignRight);
    
    QString bValues[5][4] = {
        {"(", ")", "^", "CE"},
        {"7", "8", "9", "/"},
        {"4", "5", "6", "*"},
        {"1", "2", "3", "-"},
        {"0", ".", "=", "+"}
    };

    QGridLayout* gLayout = new QGridLayout;
    gLayout->addWidget(display, 0, 0, 1, 4);

    //Backspace symbol
    gLayout->addWidget(createButton(QChar(0x2190)), 1, 3);

    for (int row = 0; row < 5; ++row) {
        for (int column = 0; column < 4; ++column) {
            gLayout->addWidget(createButton(bValues[row][column]), row + 2, column);
        }
    }
    setLayout(gLayout);
    setMaximumSize(400, 300);
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
    
    std::string result;
    try {
        result = Calculate(Parse(expression)).str();

        displayContent = QString::fromStdString(result);

        //if long number not in positive power
        if (!displayContent.contains("e+")) {
            int pointIndex = displayContent.indexOf('.');
            //10 symbols precision
            if(pointIndex != -1)
                displayContent = displayContent.left(
                    std::min(displayContent.size(), pointIndex + 10));
        }
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
    else if (buttonContent == QChar(0x2190)) { //backspace
        displayContent.resize(displayContent.size() - 1);
        display->setText(displayContent);
    }
    else {
        displayContent += buttonContent;
        display->setText(displayContent);
    }
}

void Calculator::keyPressEvent(QKeyEvent* ke)
{
    if (ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return)
        calculate();
    
    QWidget::keyPressEvent(ke);
}