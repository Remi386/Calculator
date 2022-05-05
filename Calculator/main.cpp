#include "calculator.h"
#include "stdafx.h"
#include <QtWidgets>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Calculator w;
    w.show();
    //QWidget widget;
    //widget.setFixedSize(400, 400);
    //widget.show();

    return a.exec();
}
