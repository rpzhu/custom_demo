﻿#include "mytracer.h"
#pragma execution_character_set("utf-8")
myTracer::myTracer(QCustomPlot* _plot, QCPGraph* _graph, TracerType _type)
  : plot(_plot)
  , graph(_graph)
  , type(_type)
  , visible(false)

{
    if (plot) {
        tracer = new QCPItemTracer(plot);
        tracer->setStyle(QCPItemTracer::tsPlus); //可以选择设置追踪光标的样式，这个是小十字，还有大十字，圆点等样式
                                                 //        tracer->setPen(QPen(Qt::red));
        tracer->setPen(graph->pen());            //设置tracer的颜色
        tracer->setBrush(graph->pen().color());
        tracer->setSize(10);

        label = new QCPItemText(plot);
        label->setLayer("overlay");
        label->setClipToAxisRect(false);
        label->setPadding(QMargins(5, 5, 5, 5));

        label->position->setParentAnchor(tracer->position);
        label->setFont(QFont("宋体", 10));

        arrow = new QCPItemLine(plot);
        arrow->setLayer("overlay");
        arrow->setPen(graph->pen()); //设置箭头的颜色
        arrow->setClipToAxisRect(false);
        arrow->setHead(QCPLineEnding::esSpikeArrow);

        switch (type) {
        case XAxisTracer: {
            tracer->position->setTypeX(QCPItemPosition::ptPlotCoords);
            tracer->position->setTypeY(QCPItemPosition::ptAxisRectRatio);
            label->setBrush(QBrush(QColor(244, 244, 244, 100)));
            label->setPen(QPen(Qt::black));

            label->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);

            arrow->end->setParentAnchor(tracer->position);
            arrow->start->setParentAnchor(arrow->end);
            arrow->start->setCoords(20, 0); //偏移量
            break;
        }
        case YAxisTracer: {
            tracer->position->setTypeX(QCPItemPosition::ptAxisRectRatio);
            tracer->position->setTypeY(QCPItemPosition::ptPlotCoords);

            label->setBrush(QBrush(QColor(244, 244, 244, 100)));
            label->setPen(QPen(Qt::black));
            label->setPositionAlignment(Qt::AlignRight | Qt::AlignHCenter);

            arrow->end->setParentAnchor(tracer->position);
            arrow->start->setParentAnchor(label->position);
            arrow->start->setCoords(-20, 0); //偏移量
            break;
        }
        case DataTracer: {
            tracer->position->setTypeX(QCPItemPosition::ptPlotCoords);
            tracer->position->setTypeY(QCPItemPosition::ptPlotCoords);

            label->setBrush(QBrush(QColor(244, 244, 244, 150)));
            label->setPen(graph->pen());
            label->setPositionAlignment(Qt::AlignLeft | Qt::AlignVCenter);

            arrow->end->setParentAnchor(tracer->position);
            arrow->start->setParentAnchor(arrow->end);
            arrow->start->setCoords(25, 0);
            break;
        }

        default:
            break;
        }

        setVisible(false);
    }
}

myTracer::~myTracer()
{
    if (tracer)
        plot->removeItem(tracer);
    if (label)
        plot->removeItem(label);
}

void
myTracer::setPen(const QPen& pen)
{
    tracer->setPen(pen);
    arrow->setPen(pen);
}

void
myTracer::setBrush(const QBrush& brush)
{
    tracer->setBrush(brush);
}

void
myTracer::setLabelPen(const QPen& pen)
{
    label->setPen(pen);
}

void
myTracer::setText(const QString& text)
{
    label->setText(text);
}

void
myTracer::setVisible(bool visible)
{
    tracer->setVisible(visible);
    label->setVisible(visible);
    arrow->setVisible(visible);
}

void
myTracer::updatePosition(double xValue, double yValue)
{
    if (!visible) {
        setVisible(true);
        visible = true;
    }
    if (yValue > plot->yAxis->range().upper)
        yValue = plot->yAxis->range().upper;
    switch (type) {
    case XAxisTracer: {
        tracer->position->setCoords(xValue, 1);
        label->position->setCoords(0, 15);
        arrow->start->setCoords(0, 15);
        arrow->end->setCoords(0, 0);

        break;
    }
    case YAxisTracer: {
        tracer->position->setCoords(1, yValue);
        label->position->setCoords(-20, 0);
        break;
    }
    case DataTracer: {
        tracer->position->setCoords(xValue, yValue);
        label->position->setCoords(25, 0);
        break;
    }
    default:
        break;
    }
}
