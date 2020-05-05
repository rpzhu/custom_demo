#ifndef INTERACTIVEWIDGET_H
#define INTERACTIVEWIDGET_H
#pragma execution_character_set("utf-8")
#include "qcustomplot.h"
class QLabel;

class interactiveWidget : public QWidget
{
    Q_OBJECT

public:
    explicit interactiveWidget(QWidget* parent = nullptr, QCustomPlot* Plot = nullptr);
    ~interactiveWidget();

private slots:
    void titleDoubleClick(QMouseEvent* event);
    void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
    void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
    void selectionChanged();
    void mousePress();
    void mouseWheel();
    void addRandomGraph();
    void removeSelectedGraph();
    void removeAllGraphs();
    void contextMenuRequest(QPoint pos);
    void moveLegend();
    void graphClicked(QCPAbstractPlottable* plottable, int dataIndex);

private:
    QCustomPlot* customPlot;
    QLabel* label;
};

#endif // MAINWINDOW_H
