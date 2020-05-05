#ifndef AXISTAG_H
#define AXISTAG_H
#pragma execution_character_set("utf-8")
#include "qcustomplot.h"
#include <QObject>

//轴标签
class AxisTag : public QObject
{
    Q_OBJECT
public:
    explicit AxisTag(QCPAxis* parentAxis);
    virtual ~AxisTag();

    void setPen(const QPen& pen);
    void setBrush(const QBrush& brush);
    void setText(const QString& text);

    QPen pen() const
    {
        return mLabel->pen();
    }
    QBrush brush() const
    {
        return mLabel->brush();
    }
    QString text() const
    {
        return mLabel->text();
    }

    void updatePosition(double value);

protected:
    QCPAxis* mAxis;                       //坐标轴
    QPointer<QCPItemTracer> mDummyTracer; //小圆球
    QPointer<QCPItemLine> mArrow;         //线段
    QPointer<QCPItemText> mLabel;         //文本
};

#endif // AXISTAG_H
