#include <QtGui/QApplication>
#include "widget.hpp"
#include <QSharedMemory>
#include "singleprocess.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!GV_share.create(512, QSharedMemory::ReadWrite)){
        exit(0);
    }

    Widget w;
    w.show();

    return a.exec();
}
