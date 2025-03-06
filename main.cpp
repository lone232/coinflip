#include "mainscene.h"
#include "crteffect.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainScene w;
    w.show();
    return a.exec();
}

// int main(int argc, char *argv[])
// {
//     QApplication a(argc, argv);

//     CRTEffect widget;
//     widget.resize(800, 600);
//     widget.show();

//     return a.exec();
// }
