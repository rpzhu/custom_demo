#include "interactiveWidget.h"
#include <QDebug>
#include <QInputDialog>
#include <QLabel>
#include <QVBoxLayout>
#pragma execution_character_set("utf-8")
interactiveWidget::interactiveWidget(QWidget* parent, QCustomPlot* Plot)
  : QWidget(parent)
  , customPlot(Plot)
{
    setWindowFlags(Qt::WindowStaysOnTopHint);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(255, 255, 255));
    setAutoFillBackground(true);
    setPalette(palette);
    srand(QDateTime::currentDateTime().toTime_t());

    label = new QLabel;
    label->setStyleSheet("QLabel{font-size:28px;}");
    QLabel* label1 = new QLabel;
    label1->setWordWrap(true);
    label1->setStyleSheet("QLabel{font-size:28px;}");
    label1->setText("选择要分别拖动和缩放的轴\n双击标签或图例项以设置用户指定的字符串\n左键单击图形或图例以选择图形\n右键单击弹出菜单以添加/删除图形并移动图例");

    //交互设置                   拖拽                缩放                 选中轴            选中图例                选中图表标签
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
    customPlot->xAxis->setRange(-8, 8); //坐标轴范围
    customPlot->yAxis->setRange(-5, 5);
    customPlot->axisRect()->setupFullAxesBox();
    customPlot->plotLayout()->insertRow(0);

    //标题元素
    QCPTextElement* title = new QCPTextElement(customPlot, "交互例子", QFont("sans", 17, QFont::Bold));
    customPlot->plotLayout()->addElement(0, 0, title);

    customPlot->xAxis->setLabel("x轴");
    customPlot->yAxis->setLabel("y轴");
    customPlot->legend->setVisible(true);
    QFont legendFont;
    legendFont.setPointSize(10);
    customPlot->legend->setFont(legendFont);
    customPlot->legend->setSelectedFont(legendFont);
    customPlot->legend->setSelectableParts(QCPLegend::spItems /*spLegendBox*/);

    addRandomGraph();
    addRandomGraph();
    addRandomGraph();
    addRandomGraph();
    customPlot->rescaleAxes();

    //选中图表
    connect(customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    //鼠标按下
    connect(customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    //鼠标滚轮
    connect(customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    //坐标范围改变
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

    //双击坐标轴
    connect(customPlot, SIGNAL(axisDoubleClick(QCPAxis*, QCPAxis::SelectablePart, QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*, QCPAxis::SelectablePart)));
    //双击图例
    connect(customPlot, SIGNAL(legendDoubleClick(QCPLegend*, QCPAbstractLegendItem*, QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*, QCPAbstractLegendItem*)));
    //双击标题
    connect(title, SIGNAL(doubleClicked(QMouseEvent*)), this, SLOT(titleDoubleClick(QMouseEvent*)));

    //按下图表
    connect(customPlot, SIGNAL(plottableClick(QCPAbstractPlottable*, int, QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*, int)));

    //自定义上下文菜单
    customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(customPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));

    QVBoxLayout* V = new QVBoxLayout;
    V->setContentsMargins(0, 0, 0, 0);
    V->addWidget(customPlot, 12);
    V->addWidget(label1, 2);
    V->addWidget(label, 1);
    setLayout(V);
}

interactiveWidget::~interactiveWidget()
{
}

//双击标题
void
interactiveWidget::titleDoubleClick(QMouseEvent* event)
{
    Q_UNUSED(event)

    if (QCPTextElement* title = qobject_cast<QCPTextElement*>(sender())) {
        bool ok;
        QString newTitle = QInputDialog::getText(this, "交互例子", "请输入新标题：", QLineEdit::Normal, title->text(), &ok);
        if (ok) {
            title->setText(newTitle);
            customPlot->replot(); //刷新图表
        }
    }
}

//双击坐标轴
void
interactiveWidget::axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part)
{
    if (part == QCPAxis::spAxisLabel) //双击的是轴标签
    {
        bool ok;
        QString newLabel = QInputDialog::getText(this, "交互例子", "新轴标签：", QLineEdit::Normal, axis->label(), &ok);
        if (ok) {
            axis->setLabel(newLabel);
            customPlot->replot();
        }
    }
    if (part == QCPAxis::spTickLabels) {
        qDebug() << "双击了刻度标签";
    }
}

//双击图例
void
interactiveWidget::legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item)
{
    Q_UNUSED(legend)
    if (item) {
        QCPPlottableLegendItem* plItem = qobject_cast<QCPPlottableLegendItem*>(item);
        bool ok;
        QString newName = QInputDialog::getText(this, "交互例子", "图表更名为：", QLineEdit::Normal, plItem->plottable()->name(), &ok);
        if (ok) {
            plItem->plottable()->setName(newName);
            customPlot->replot();
        }
    }
}

//选中图表
void
interactiveWidget::selectionChanged()
{
    //当选中一个x/y轴时确保另一条x/y轴也被选中
    if (customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || customPlot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
        customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels)) {
        customPlot->xAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
        customPlot->xAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
    }

    if (customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || customPlot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
        customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels)) {
        customPlot->yAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
        customPlot->yAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
    }

    for (int i = 0; i < customPlot->graphCount(); ++i) {
        QCPGraph* graph = customPlot->graph(i);
        QCPPlottableLegendItem* item = customPlot->legend->itemWithPlottable(graph);
        if (item->selected() || graph->selected()) //图例或图表有一个被选中时设置两个同时被选中
        {
            item->setSelected(true);
            graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
        }
    }
}

//鼠标按下
void
interactiveWidget::mousePress()
{
    //鼠标按下后当x轴被选中时设置图表的交互范围是单一的x或y方向
    //orientation()自动确定方向
    if (customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
        customPlot->axisRect()->setRangeDrag(customPlot->xAxis->orientation());
    } else if (customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
        customPlot->axisRect()->setRangeDrag(customPlot->yAxis->orientation());
    } else {
        customPlot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    }
}

//鼠标滚动
void
interactiveWidget::mouseWheel()
{
    //当坐标轴被按下时 范围缩放方向为单一方向
    if (customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
        customPlot->axisRect()->setRangeZoom(customPlot->xAxis->orientation());
    } else if (customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
        customPlot->axisRect()->setRangeZoom(customPlot->yAxis->orientation());
    } else {
        customPlot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    }
}

//添加随机图表
void
interactiveWidget::addRandomGraph()
{
    int n = 50; //图表上点的个数
    double xScale = (rand() / (double)RAND_MAX + 0.5) * 2;
    double yScale = (rand() / (double)RAND_MAX + 0.5) * 2;
    double xOffset = (rand() / (double)RAND_MAX - 0.5) * 4;
    double yOffset = (rand() / (double)RAND_MAX - 0.5) * 10;
    double r1 = (rand() / (double)RAND_MAX - 0.5) * 2;
    double r2 = (rand() / (double)RAND_MAX - 0.5) * 2;
    double r3 = (rand() / (double)RAND_MAX - 0.5) * 2;
    double r4 = (rand() / (double)RAND_MAX - 0.5) * 2;
    QVector<double> x(n), y(n);
    for (int i = 0; i < n; i++) {
        x[i] = (i / (double)n - 0.5) * 10.0 * xScale + xOffset;
        y[i] = (qSin(x[i] * r1 * 5) * qSin(qCos(x[i] * r2) * r4 * 3) + r3 * qCos(qSin(x[i]) * r4 * 2)) * yScale + yOffset;
    }

    customPlot->addGraph();
    customPlot->graph()->setName(QString("新图表 %1").arg(customPlot->graphCount() - 1));
    customPlot->graph()->setData(x, y); //图表数据
    customPlot->graph()->setLineStyle((QCPGraph::LineStyle)(rand() % 5 + 1));
    if (rand() % 100 > 50) {
        customPlot->graph()->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)(rand() % 14 + 1)));
    }
    QPen graphPen;
    graphPen.setColor(QColor(rand() % 245 + 10, rand() % 245 + 10, rand() % 245 + 10));
    graphPen.setWidthF(rand() / (double)RAND_MAX * 2 + 1);
    customPlot->graph()->setPen(graphPen);
    customPlot->replot();
}

//移除选中的图表
void
interactiveWidget::removeSelectedGraph()
{
    if (customPlot->selectedGraphs().size() > 0) {
        customPlot->removeGraph(customPlot->selectedGraphs().first());
        customPlot->replot();
    }
}

//移除所有图表
void
interactiveWidget::removeAllGraphs()
{
    customPlot->clearGraphs();
    customPlot->replot();
}

void
interactiveWidget::contextMenuRequest(QPoint pos)
{
    QMenu* menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    if (customPlot->legend->selectTest(pos, false) >= 0) //图例菜单
    {
        menu->addAction("移动到左上角", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop | Qt::AlignLeft));
        menu->addAction("移动到顶部中央", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop | Qt::AlignHCenter));
        menu->addAction("移动到右上角", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop | Qt::AlignRight));
        menu->addAction("移动到右下角", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom | Qt::AlignRight));
        menu->addAction("移动到左下角", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom | Qt::AlignLeft));
    } else {
        menu->addAction("增加随机图表", this, SLOT(addRandomGraph()));
        if (customPlot->selectedGraphs().size() > 0) {
            menu->addAction("移除选中图表", this, SLOT(removeSelectedGraph()));
        }
        if (customPlot->graphCount() > 0) {
            menu->addAction("移除所有图表", this, SLOT(removeAllGraphs()));
        }
    }
    menu->popup(customPlot->mapToGlobal(pos));
}

//移动图例
void
interactiveWidget::moveLegend()
{
    if (QAction* contextAction = qobject_cast<QAction*>(sender())) {
        bool ok;
        int dataInt = contextAction->data().toInt(&ok);
        if (ok) {
            customPlot->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
            customPlot->replot();
        }
    }
}

void
interactiveWidget::graphClicked(QCPAbstractPlottable* plottable, int dataIndex)
{
    label->setText(QString("选中图表：%1，第%2个数据点，值为：%3").arg(plottable->name()).arg(dataIndex).arg(plottable->interface1D()->dataMainValue(dataIndex)));
}
