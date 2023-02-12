#include "TiffViewer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TiffViewer w;
    w.show();
    return a.exec();
}
