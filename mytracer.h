﻿#ifndef MYTRACER_H
#define MYTRACER_H
#pragma execution_character_set("utf-8")
#include "qcustomplot.h"
#include <QObject>
enum TracerType
{
    XAxisTracer,
    YAxisTracer,
    DataTracer
};

class myTracer : public QObject
{
    Q_OBJECT

public:
    explicit myTracer(QCustomPlot* _plot, QCPGraph* _graph, TracerType _type);
    ~myTracer();

    void setPen(const QPen& pen);
    void setBrush(const QBrush& brush);
    void setText(const QString& text);
    void setLabelPen(const QPen& pen);
    void updatePosition(double xValue, double yValue);

protected:
    void setVisible(bool visible);

protected:
    QCustomPlot* plot;     //传入实例化的QcustomPlot
    QCPGraph* graph;       //这里是存传入的绘图图层
    QCPItemTracer* tracer; // 跟踪的点
    QCPItemText* label;    // 显示的数值
    QCPItemLine* arrow;    // 箭头

    TracerType type;
    bool visible;
};

#endif // MYTRACER_H
