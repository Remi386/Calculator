#include "calculator.h"
//#include "stdafx.h"
#include <QtWidgets>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Calculator w;
    w.show();

    return a.exec();
}
