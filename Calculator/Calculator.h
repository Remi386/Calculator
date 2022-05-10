#pragma once

#include <QtWidgets/QWidget>
#include <boost/multiprecision/cpp_dec_float.hpp>
using float100 = boost::multiprecision::cpp_dec_float_100;

class QLineEdit;
class QPushButton;

class Calculator : public QWidget {
    Q_OBJECT

private:
    QLineEdit* display;
    QString displayContent;
    float100 memory;
    QPushButton* backSpace;

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
