#pragma once

#include <QtWidgets>
//#include "ui_calculator.h"

class Calculator : public QWidget {
    Q_OBJECT

private:
    QLineEdit* display;
    QString displayContent;

public:
    Calculator(QWidget *parent = Q_NULLPTR);

public slots:
    void slotButtonClicked();

private:
    QPushButton* createButton(const QString& name);
    void calculate();

    //Ui::calculatorClass ui;
};
