#include "calculator.h"
#include <QtWidgets>
#include <chrono>
#include <string>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent)
{
    //ui.setupUi(this);

    display = new QLineEdit;
    display->setMinimumSize(150, 50);

    display->setAlignment(Qt::AlignRight);
    
    QString bValues[5][6] = {
        {"log", "ln", "(", ")", "^", "CE"},
        {"tan", "7", "8", "9", "/", "M+"},
        {"cos", "4", "5", "6", "*", "M-"},
        {"sin", "1", "2", "3", "-", "M"},
        {"sqrt", "0", ".", "=", "+", "!"}
    };

    QGridLayout* gLayout = new QGridLayout;
    gLayout->addWidget(display, 0, 0, 1, 6);

    //Backspace symbol
    gLayout->addWidget(createButton(QChar(0x2190)), 1, 5);

    for (int row = 0; row < 5; ++row) {
        for (int column = 0; column < 6; ++column) {
            gLayout->addWidget(createButton(bValues[row][column]), row + 2, column);
        }
    }
    setLayout(gLayout);
    setMaximumSize(500, 350);
}

QPushButton* Calculator::createButton(const QString& name)
{
    QPushButton* button = new QPushButton(name);
    button->setMinimumSize(40, 30);
    connect(button, &QPushButton::clicked,
            this,   &Calculator::slotButtonClicked);
    return button;
}

void Calculator::calculate()
{
    std::string expression = display->text().toStdString();
    
    std::string result;
    try {
        
        result = MathExpression::Calculate(expression).str();

        displayContent = QString::fromStdString(result);

        //if long number not in positive power
        if (!displayContent.contains("e+")) {
            int64_t pointIndex = displayContent.indexOf('.');
            //10 symbols precision
            if(pointIndex != -1)
                displayContent = displayContent.left(
                    std::min<int64_t>(displayContent.size(), pointIndex + 10));
        }
    }
    catch (const std::exception& ex) {
        needCleanInput = true;
        displayContent = ex.what();
    }

    display->setText(displayContent);
}

void Calculator::slotButtonClicked()
{
    QString buttonContent = ((QPushButton*)sender())->text();

    if (needCleanInput) { //exception was thrown
        display->setText("0");
        displayContent = "";
        needCleanInput = false;
    }

    if (buttonContent == "CE") {
        display->setText("0");
        displayContent = "";
        needCleanInput = false;
    }
    else if (buttonContent == "M+") {
        displayContent = display->text();
        memory.assign(displayContent.toStdString());
    }
    else if (buttonContent == "M-") {
        memory = 0;
    }
    else if (buttonContent == "M") {
        displayContent += QString::fromStdString(memory.str());
        display->setText(displayContent);
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