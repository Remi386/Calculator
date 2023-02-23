#pragma once

#include <QtWidgets/QWidget>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include "MathExpression.h"

class QLineEdit;
class QPushButton;

class Calculator : public QWidget {
    Q_OBJECT

private:
    QLineEdit* display;
    QString displayContent;
    float100 memory;
    bool needCleanInput = false;

protected:
    virtual void keyPressEvent(QKeyEvent* ke);

public:
    Calculator(QWidget *parent = Q_NULLPTR);

public slots:
    void slotButtonClicked();

private:
    QPushButton* createButton(const QString& name);
    void calculate();

    //Ui::calculatorClass ui;
};
