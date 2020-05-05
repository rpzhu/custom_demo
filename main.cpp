#pragma execution_character_set("utf-8")
#include "mainwindow.h"

#include <QApplication>

int
main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("QCustomPlot各种Demo收集整理，来源于网络和QCustomPlot例程");
    w.show();
    return a.exec();
}
