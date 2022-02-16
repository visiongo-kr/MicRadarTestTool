#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    qApp->installEventFilter(&w);
    w.show();
    return a.exec();
}
