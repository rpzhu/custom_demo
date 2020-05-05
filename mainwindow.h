#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "axistag.h"
#include "flowlayout.h"
#include <QMainWindow>
#include <QTimer>
#pragma execution_character_set("utf-8")
class QCustomPlot;
class QDesktopWidget;
class myTracer;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    void init_add_btn();

private:
    QString get_rand_color();
    void init_CustomPlot();
    Ui::MainWindow* ui;
    FlowLayout* flowlayout;
    QSize size;
    QDesktopWidget* desktop;
    QCustomPlot* customPlot;

    //动态图
    QTimer timer;
    int mIndex;
    QVector<QString> mLabels, mIndexLabels;
    QVector<double> mPositions;
    QPointer<QCPGraph> mGraph1; //折线图
    QPointer<QCPGraph> mGraph2;
    AxisTag* mTag1;
    AxisTag* mTag2;
    double lastPointKey;
    QTime time;
    QCPItemTracer* itemDemoPhaseTracer = nullptr;
    myTracer* m_TraserX = nullptr;
    myTracer* m_TraserY = nullptr;
    myTracer* m_TraserD = nullptr;

    void showTracer(QMouseEvent* event);
};
#endif // MAINWINDOW_H
