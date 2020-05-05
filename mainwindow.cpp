#include "mainwindow.h"
#include "QCustomPlot.h"
#include "interactiveWidget.h"
#include "mytracer.h"
#include "savePDFwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QPushButton>
#include <QTime>
#include <QTimer>
#pragma execution_character_set("utf-8")
MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(255, 255, 255));
    setAutoFillBackground(true);
    setPalette(palette);

    ui->scrollArea->viewport()->setStyleSheet("background-color:transparent;");
    flowlayout = new FlowLayout(10, 10, 10, 10, 10, 10);
    ui->scrollAreaWidgetContents->setLayout(flowlayout);

    desktop = QApplication::desktop();
    resize(desktop->width() * 0.8, desktop->height() * 0.8);

    size = QSize((desktop->width() * 0.8 - 100) / 6, 40);

    customPlot = nullptr;

    init_add_btn();
}

MainWindow::~MainWindow()
{
    delete ui;
    if (m_TraserX != nullptr) {
        delete m_TraserX;
    }
    if (m_TraserY != nullptr) {
        delete m_TraserY;
    }
    if (m_TraserD != nullptr) {
        delete m_TraserD;
    }
    if (customPlot)
        delete customPlot;
}

//添加按钮
void
MainWindow::init_add_btn()
{
    QString style = "QPushButton{border-style: none;border: 0px;color: #FFFFFF;border-radius:6px;background: #00beac;font-size:22px;font-weight:bold;}"
                    "QPushButton:hover{background: #20c9b3;}"
                    "QPushButton:pressed{background: #01968c;}"
                    "QToolTip{border:1px solid rgb(118, 118, 118);border-radius:6px;background-color: #fffffe; color:#484848; font-size:20px;}";

    QPushButton* btn = new QPushButton("一根抛物线", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        customPlot->plotLayout()->insertRow(0); // 插入一行
        // 在第一行第一列添加标题
        customPlot->plotLayout()->addElement(0, 0, new QCPTextElement(customPlot, "标题：抛物线", QFont("sans", 12, QFont::Bold)));

        QCPTextElement* title = new QCPTextElement(customPlot, "这是图例标题", QFont("sans", 10, QFont::Bold));
        title->setMargins(QMargins(0, 6, 0, 10)); // 为了效果更好，添加一些边距
        title->setLayer("legend");                // 一定要把标题的层设置为legend层

        customPlot->legend->insertRow(0);            // 插入一行
        customPlot->legend->addElement(0, 0, title); // 在第一行第一列添加标题

        //横向图例
        customPlot->legend->setFillOrder(QCPLayoutGrid::foColumnsFirst);

        QVector<double> x(101), y(101);
        for (int i = 0; i < 101; ++i) {
            x[i] = i / 50.0 - 1; // -1 到 1
            y[i] = x[i] * x[i];
        }
        customPlot->addGraph();                                             // 添加一个曲线图QGraph
        customPlot->graph(customPlot->graphCount() - 1)->setData(x, y);     // 为曲线图添加数据
        customPlot->graph(customPlot->graphCount() - 1)->setName("抛物线"); //  设置曲线图的名字
        customPlot->graph(customPlot->graphCount() - 1)->setPen(QColor(get_rand_color()));
        customPlot->xAxis->setLabel("x"); // 设置x轴的标签
        customPlot->yAxis->setLabel("y");
        customPlot->xAxis->setRange(-1, 1); // 设置x轴的范围为(-1,1)
        customPlot->yAxis->setRange(0, 1);
        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("线条样式", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();
        QPen pen;
        QStringList lineNames;
        lineNames << "lsNone"
                  << "lsLine"
                  << "lsStepLeft"
                  << "lsStepRight"
                  << "lsStepCenter"
                  << "lsImpulse";

        for (int i = QCPGraph::lsNone; i <= QCPGraph::lsImpulse; ++i) {
            customPlot->addGraph();
            pen.setColor(QColor(qSin(i * 1 + 1.2) * 80 + 80, qSin(i * 0.3 + 0) * 80 + 80, qSin(i * 0.3 + 1.5) * 80 + 80));
            customPlot->graph()->setPen(pen); // 设置图表的画笔
            customPlot->graph()->setName(lineNames.at(i - QCPGraph::lsNone));
            customPlot->graph()->setLineStyle((QCPGraph::LineStyle)i);                           // 设置图表线段的风格
            customPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5)); // 设置图表散点图的样式，散点图的样式有很多种，可以自己试试

            QVector<double> x(15), y(15);
            for (int j = 0; j < 15; ++j) {
                x[j] = j / 15.0 * 5 * 3.14 + 0.01;
                y[j] = 7 * qSin(x[j]) / x[j] - (i - QCPGraph::lsNone) * 5 + (QCPGraph::lsImpulse)*5 + 2;
            }
            customPlot->graph()->setData(x, y);
            customPlot->graph()->rescaleAxes(true);
        }
        customPlot->xAxis->setRange(0, 15); // 设置x轴的范围为(-1,1)
        customPlot->yAxis->setRange(0, 40);
        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("两根曲线围成区域", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();
        customPlot->addGraph();
        customPlot->graph(0)->setPen(QPen(Qt::blue));
        customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); //填充

        customPlot->addGraph();
        customPlot->graph(1)->setPen(QPen(Qt::red)); //设置第二条曲线

        QVector<double> x(251), y0(251), y1(251);
        for (int i = 0; i < 251; ++i) {
            x[i] = i;
            y0[i] = qExp(-i / 150.0) * qCos(i / 10.0);
            y1[i] = qExp(-i / 150.0);
        }
        customPlot->graph(0)->setData(x, y0);
        customPlot->graph(1)->setData(x, y1);

        //让范围自行缩放，以便图完全适合可见区域:
        customPlot->graph(0)->rescaleAxes();
        //对于图1也是一样，但只扩大范围（如果图1小于图0）:
        customPlot->graph(1)->rescaleAxes(true);
        customPlot->graph(0)->setChannelFillGraph(customPlot->graph(1)); // 将图0与图1围成区域 使用的是图0的画刷

        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("文本刻度轴", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        //纵向
        QCPAxis* keyAxis = customPlot->xAxis;
        QCPAxis* valueAxis = customPlot->yAxis;

        //横向
        //        QCPAxis *keyAxis = customPlot->yAxis;
        //        QCPAxis *valueAxis = customPlot->xAxis;

        //柱状图
        QCPBars* fossil = new QCPBars(keyAxis, valueAxis); // 使用xAxis作为柱状图的key轴，yAxis作为value轴

        fossil->setAntialiased(false);                        // 为了更好的边框效果，关闭抗齿锯
        fossil->setName("文本刻度轴");                        // 设置柱状图的名字，可在图例中显示
        fossil->setPen(QPen(QColor(255, 0, 0).lighter(130))); // 设置柱状图的边框颜色
        fossil->setBrush(QColor(0, 168, 140));                // 设置柱状图的画刷颜色

        //为柱状图设置一个文字类型的key轴，ticks决定了轴的范围，而labels决定了轴的刻度文字的显示
        QVector<double> ticks;
        ticks << 1 << 2 << 3 << 4 << 5 << 6 << 7;

        QVector<QString> labels;
        labels << "美国"
               << "日本"
               << "德国"
               << "法国"
               << "英国"
               << "意大利"
               << "加拿大";

        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        textTicker->addTicks(ticks, labels);

        keyAxis->setTicker(textTicker);                       // 设置为文字轴
        keyAxis->setTickLabelRotation(60);                    // 轴刻度文字旋转60度
        keyAxis->setSubTicks(false);                          // 不显示子刻度
        keyAxis->setTickLength(0, 4);                         // 轴内外刻度的长度分别是0,4,也就是轴内的刻度线不显示
        keyAxis->setRange(0, 8);                              // 设置范围
        keyAxis->setUpperEnding(QCPLineEnding::esSpikeArrow); //显示末尾箭头

        valueAxis->setRange(0, 12.1);
        valueAxis->setPadding(35); // 轴的内边距 轴距离窗口边缘的距离

        valueAxis->setLabel("人均用电量（千瓦）(2007)");
        valueAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

        QVector<double> fossilData;
        fossilData << 0.86 * 10.5 << 0.83 * 5.5 << 0.84 * 5.5 << 0.52 * 5.8 << 0.89 * 5.2 << 0.90 * 4.2 << 0.67 * 11.2;
        fossil->setData(ticks, fossilData); //柱状图数据

        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("日期时间坐标轴1", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        customPlot->setInteraction(QCP::iRangeDrag, true); //开启拖拽和缩放
        customPlot->setInteraction(QCP::iRangeZoom, true);
        //QCPAxisTickerDateTime坐标轴默认是从时间戳起点即1970-1-1 0:0:0开始，以秒为单位进行刻度划分
        double now = QDateTime::currentDateTime().toTime_t(); //当前时间转化为秒
        //生成时间刻度对象
        QSharedPointer<QCPAxisTickerDateTime> dateTimeTicker(new QCPAxisTickerDateTime);
        customPlot->xAxis->setTicker(dateTimeTicker);
        //dateTimeTicker->setDateTimeSpec(Qt::UTC);//设置世界时间，即不加上时区的时间
        dateTimeTicker->setTickCount(12);
        dateTimeTicker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
        customPlot->xAxis->setSubTicks(false);
        customPlot->xAxis->setRange(now, now + 3600 * 24); //x轴范围，从当前时间起往后推24小时
        QVector<double> yData, xData;                      //生成数据
        for (int i = 0; i <= 24; ++i) {
            xData.push_back(now + i * 3600.0);
            yData.push_back(pow(double(i), 2) + 550.0 * sin(double(i) / 4.0));
        }
        dateTimeTicker->setDateTimeFormat("yyyy-M-d h:m"); //设置x轴刻度显示格式
        customPlot->xAxis->setTickLabelRotation(30);       //设置刻度标签顺时针旋转30度
        customPlot->yAxis->setRange(-250, 700);
        customPlot->addGraph();
        customPlot->graph(0)->setData(xData, yData); //显示数据
        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("日期时间坐标轴2", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();
        customPlot->setLocale(QLocale(QLocale::Chinese));

        double now = QDateTime::currentDateTime().toTime_t();
        srand(8);

        for (int gi = 0; gi < 5; ++gi) {
            customPlot->addGraph();
            QColor color(20 + 200 / 4.0 * gi, 70 * (1.6 - gi / 4.0), 150, 150);
            customPlot->graph()->setLineStyle(QCPGraph::lsLine);
            customPlot->graph()->setPen(QPen(color.lighter(200)));
            customPlot->graph()->setBrush(QBrush(color));
            customPlot->graph()->setName(QString("图表%1").arg(gi + 1));

            QVector<QCPGraphData> timeData(250);
            for (int i = 0; i < 250; ++i) {
                timeData[i].key = now + 24 * 3600 * i;
                if (i == 0) {
                    timeData[i].value = (i / 50.0 + 1) * (rand() / (double)RAND_MAX - 0.5);
                } else {
                    timeData[i].value = qFabs(timeData[i - 1].value) * (1 + 0.02 / 4.0 * (4 - gi)) + (i / 50.0 + 1) * (rand() / (double)RAND_MAX - 0.5);
                }
            }
            customPlot->graph()->data()->set(timeData);
        }

        QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
        //    dateTicker->setDateTimeFormat("d. MMMM\nyyyy");
        dateTicker->setDateTimeFormat("yyyy-MM");
        customPlot->xAxis->setTicker(dateTicker);

        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        textTicker->addTick(10, "很低"); //增加两个刻度
        textTicker->addTick(50, "十分高");
        customPlot->yAxis->setTicker(textTicker);

        customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 18));
        customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 18));

        customPlot->xAxis->setLabel("日期");
        customPlot->yAxis->setLabel("随机摆动线值");

        customPlot->xAxis2->setVisible(true);
        customPlot->yAxis2->setVisible(true);
        customPlot->xAxis2->setTicks(false);
        customPlot->yAxis2->setTicks(false);
        customPlot->xAxis2->setTickLabels(false);
        customPlot->yAxis2->setTickLabels(false);

        customPlot->xAxis->setRange(now, now + 24 * 3600 * 249);
        customPlot->yAxis->setRange(0, 60);

        customPlot->legend->setVisible(true);
        customPlot->legend->setBrush(QColor(255, 255, 255, 150));

        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("时间坐标轴", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        customPlot->setInteraction(QCP::iRangeDrag, true); //开启拖拽和缩放
        customPlot->setInteraction(QCP::iRangeZoom, true);
        customPlot->rescaleAxes();

        int hours = QDateTime::currentDateTime().time().hour(); //获取当前的小时数

        QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
        customPlot->xAxis->setTicker(timeTicker);
        timeTicker->setTickCount(6);
        //设置刻度表示策略
        timeTicker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
        customPlot->xAxis->setSubTicks(false);
        timeTicker->setTickOrigin(3600 * 3); //x轴起点坐标加3小时，即从当前hours+3开始
        customPlot->xAxis->setRange(hours * 3600, hours * 3600 + 3600 * 12);
        customPlot->yAxis->setRange(-300, 700);
        //时间可以以毫秒、秒、分钟、小时和天显示。取决于通过setTimeFormat函数定义的最大可用单位，超过最大单位的
        timeTicker->setTimeFormat("%d天-%h小时");

        QVector<double> yData, xData;
        for (int i = 0; i <= 24; ++i) {
            xData.push_back(hours * 3600 + i * 3600);
            yData.push_back(pow(double(i), 2) + 550.0 * sin(double(i) / 4.0));
        }
        customPlot->addGraph();
        customPlot->graph(0)->setData(xData, yData);

        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("π刻度轴", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        QSharedPointer<QCPAxisTickerPi> pITicker(new QCPAxisTickerPi());
        customPlot->xAxis->setTicker(pITicker);
        //pITicker->setPeriodicity(2);
        //pITicker->setPiSymbol("--");
        pITicker->setFractionStyle(QCPAxisTickerPi::fsUnicodeFractions);
        pITicker->setPiValue(3.1415 * 2); //设置pi表示的值
        customPlot->xAxis->setRange(0, 3.0 * M_PI);
        customPlot->xAxis->setVisible(true);
        QVector<double> x2(250), y2(250);
        for (int i = 0; i < 250; ++i) {
            x2[i] = i / 250.0 * 3 * M_PI;
            y2[i] = qSin(x2[i] + M_PI) + M_PI;
        }
        customPlot->addGraph(customPlot->xAxis, customPlot->yAxis);
        customPlot->graph(0)->setData(x2, y2);

        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("对数刻度轴1", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
        customPlot->xAxis->setTicker(logTicker);
        customPlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
        customPlot->xAxis->setRange(0, 250);
        logTicker->setLogBase(10);
        QVector<double> x2(250), y2(250);
        for (int i = 0; i < 250; ++i) {
            x2[i] = i;
            y2[i] = log(i);
        }
        customPlot->addGraph(customPlot->xAxis, customPlot->yAxis);
        customPlot->graph(0)->setData(x2, y2);

        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("对数刻度轴2", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        customPlot->setNoAntialiasingOnDrag(true); //设置当用户拖动轴范围时是否对此QCustomPlot禁用抗锯齿
        customPlot->addGraph();
        QPen pen;
        pen.setColor(QColor(255, 170, 100));
        pen.setWidth(2);
        pen.setStyle(Qt::DotLine);
        customPlot->graph(0)->setPen(pen);
        customPlot->graph(0)->setName("x");

        customPlot->addGraph();
        customPlot->graph(1)->setPen(QPen(Qt::red));
        customPlot->graph(1)->setBrush(QBrush(QColor(255, 0, 0, 20)));
        customPlot->graph(1)->setName("-sin(x)exp(x)");

        customPlot->addGraph();
        customPlot->graph(2)->setPen(QPen(Qt::blue));
        customPlot->graph(2)->setBrush(QBrush(QColor(0, 0, 255, 20)));
        customPlot->graph(2)->setName("sin(x)exp(x)");

        customPlot->addGraph();
        pen.setColor(QColor(0, 0, 0));
        pen.setWidth(1);
        pen.setStyle(Qt::DashLine);
        customPlot->graph(3)->setPen(pen);
        customPlot->graph(3)->setBrush(QBrush(QColor(0, 0, 0, 15)));
        customPlot->graph(3)->setLineStyle(QCPGraph::lsStepCenter);
        customPlot->graph(3)->setName("x!");

        const int dataCount = 200;
        const int dataFactorialCount = 21;
        QVector<QCPGraphData> dataLinear(dataCount), dataMinusSinExp(dataCount), dataPlusSinExp(dataCount), dataFactorial(dataFactorialCount);
        for (int i = 0; i < dataCount; ++i) {
            dataLinear[i].key = i / 10.0;
            dataLinear[i].value = dataLinear[i].key;
            dataMinusSinExp[i].key = i / 10.0;
            dataMinusSinExp[i].value = -qSin(dataMinusSinExp[i].key) * qExp(dataMinusSinExp[i].key);
            dataPlusSinExp[i].key = i / 10.0;
            dataPlusSinExp[i].value = qSin(dataPlusSinExp[i].key) * qExp(dataPlusSinExp[i].key);
        }
        for (int i = 0; i < dataFactorialCount; ++i) {
            dataFactorial[i].key = i;
            dataFactorial[i].value = 1.0;
            for (int k = 1; k <= i; ++k) {
                dataFactorial[i].value *= k; // 阶乘
            }
        }
        customPlot->graph(0)->data()->set(dataLinear);
        customPlot->graph(1)->data()->set(dataMinusSinExp);
        customPlot->graph(2)->data()->set(dataPlusSinExp);
        customPlot->graph(3)->data()->set(dataFactorial);

        customPlot->yAxis->grid()->setSubGridVisible(true);
        customPlot->xAxis->grid()->setSubGridVisible(true);
        customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
        customPlot->yAxis2->setScaleType(QCPAxis::stLogarithmic);
        QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
        customPlot->yAxis->setTicker(logTicker);
        customPlot->yAxis2->setTicker(logTicker);
        customPlot->yAxis->setNumberFormat("eb"); // e = 指数, b = beautiful decimal powers
        customPlot->yAxis->setNumberPrecision(0); // 确保 "1*10^4" 显示为 "10^4"
        customPlot->xAxis->setRange(0, 19.9);
        customPlot->yAxis->setRange(1e-2, 1e10);
        customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); //开启拖拽和缩放

        customPlot->axisRect()->setupFullAxesBox();
        connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
        connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

        customPlot->legend->setVisible(true);
        customPlot->legend->setBrush(QBrush(QColor(255, 255, 255, 150)));
        customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft | Qt::AlignTop); //图例在左上角

        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("多坐标轴1", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        customPlot->plotLayout()->clear(); // 清空默认的轴矩形 可以清空轴的所有内容
        QCPAxisRect* wideAxisRect = new QCPAxisRect(customPlot, true);
        wideAxisRect->setupFullAxesBox(true);                                         // 让四个轴关联，并全部显示出来
        wideAxisRect->addAxis(QCPAxis::atLeft)->setTickLabelColor(QColor("#6050F8")); // 在左边多添加一个轴并且设置它的颜色

        QCPLayoutGrid* subLayout = new QCPLayoutGrid;
        customPlot->plotLayout()->addElement(0, 0, wideAxisRect); // 在第一行添加轴矩形
        customPlot->plotLayout()->addElement(1, 0, subLayout);    // 在第二行添加一个子布局，后面会添加两个轴矩形在里面

        QCPAxisRect* subRectLeft = new QCPAxisRect(customPlot, false); // 不配置轴
        QCPAxisRect* subRectRight = new QCPAxisRect(customPlot, false);

        // 让右边的轴矩形固定大小
        subRectRight->setMaximumSize(100, 100);
        subRectRight->setMinimumSize(100, 100);

        subLayout->addElement(0, 0, subRectLeft);  // 在第一列添加轴矩形
        subLayout->addElement(0, 1, subRectRight); // 在第二列添加轴矩形

        subRectLeft->addAxes(QCPAxis::atBottom | QCPAxis::atLeft);     // 添加下轴和左轴
        subRectRight->addAxes(QCPAxis::atBottom | QCPAxis::atRight);   // 添加下轴和右轴
        subRectLeft->axis(QCPAxis::atLeft)->ticker()->setTickCount(2); // 设置轴的刻度为一个固定的步进值
        subRectRight->axis(QCPAxis::atRight)->ticker()->setTickCount(2);
        subRectLeft->axis(QCPAxis::atBottom)->grid()->setVisible(true);

        //放置在相应的层
        foreach (auto* rect, customPlot->axisRects()) {
            foreach (auto* axis, rect->axes()) {
                axis->setLayer("axes");
                axis->grid()->setLayer("grid");
            }
        }

        // 准备数据
        QVector<QCPGraphData> dataCos(21), dataGauss(50), dataRandom(100);
        QVector<double> x3, y3;
        qsrand(3);
        for (int i = 0; i < dataCos.size(); ++i) {
            dataCos[i].key = i / (double)(dataCos.size() - 1) * 10 - 5.0;
            dataCos[i].value = qCos(dataCos[i].key);
        }
        for (int i = 0; i < dataGauss.size(); ++i) {
            dataGauss[i].key = i / (double)dataGauss.size() * 10 - 5.0;
            dataGauss[i].value = qExp(-dataGauss[i].key * dataGauss[i].key * 0.2) * 1000;
        }
        for (int i = 0; i < dataRandom.size(); ++i) {
            dataRandom[i].key = i / (double)dataRandom.size() * 10;
            dataRandom[i].value = qrand() / (double)RAND_MAX - 0.5 + dataRandom[qMax(0, i - 1)].value;
        }
        x3 << 1 << 2 << 3 << 4;
        y3 << 2 << 2.5 << 4 << 1.5;

        // mainGraphCos和 mainGraphGauss 共享下轴，但是它们的左轴不同
        QCPGraph* mainGraphCos = customPlot->addGraph(wideAxisRect->axis(QCPAxis::atBottom), wideAxisRect->axis(QCPAxis::atLeft));
        mainGraphCos->data()->set(dataCos);
        mainGraphCos->valueAxis()->setRange(-1, 1);
        mainGraphCos->rescaleKeyAxis();
        mainGraphCos->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black), QBrush(Qt::white), 6));
        mainGraphCos->setPen(QPen(QColor(120, 120, 120), 2));

        QCPGraph* mainGraphGauss = customPlot->addGraph(wideAxisRect->axis(QCPAxis::atBottom), wideAxisRect->axis(QCPAxis::atLeft, 1));
        mainGraphGauss->data()->set(dataGauss);
        mainGraphGauss->setPen(QPen(QColor("#8070B8"), 2));
        mainGraphGauss->setBrush(QColor(110, 170, 110, 30));
        mainGraphCos->setChannelFillGraph(mainGraphGauss);
        mainGraphCos->setBrush(QColor(255, 161, 0, 50));
        mainGraphGauss->valueAxis()->setRange(0, 1000);
        mainGraphGauss->rescaleKeyAxis();

        QCPGraph* subGraphRandom = customPlot->addGraph(subRectLeft->axis(QCPAxis::atBottom), subRectLeft->axis(QCPAxis::atLeft));
        subGraphRandom->data()->set(dataRandom);
        subGraphRandom->setLineStyle(QCPGraph::lsImpulse);
        subGraphRandom->setPen(QPen(QColor("#FFA100"), 1.5));
        subGraphRandom->rescaleAxes();

        QCPBars* subBars = new QCPBars(subRectRight->axis(QCPAxis::atBottom), subRectRight->axis(QCPAxis::atRight));
        subBars->setWidth(3 / (double)x3.size());
        subBars->setData(x3, y3);
        subBars->setPen(QPen(Qt::black));
        subBars->setAntialiased(false);
        subBars->setAntialiasedFill(false);
        subBars->setBrush(QColor("#705BE8"));
        subBars->keyAxis()->setSubTicks(false);
        subBars->rescaleAxes();

        // 给柱状图一个固定步长轴
        QSharedPointer<QCPAxisTickerFixed> intTicker(new QCPAxisTickerFixed);
        intTicker->setTickStep(1.0);
        intTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);
        subBars->keyAxis()->setTicker(intTicker);

        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("多坐标轴2", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        customPlot->setLocale(QLocale(QLocale::Chinese));
        customPlot->legend->setVisible(true);
        QFont legendFont = font();
        legendFont.setPointSize(9);
        customPlot->legend->setFont(legendFont);
        customPlot->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));

        customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignRight);

        customPlot->addGraph(customPlot->yAxis, customPlot->xAxis);
        customPlot->graph(0)->setPen(QPen(QColor(255, 100, 0)));
        customPlot->graph(0)->setBrush(QBrush(QPixmap(":/image/balboa.jpg")));
        customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
        customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
        customPlot->graph(0)->setName("左：曲线1");

        customPlot->addGraph();
        customPlot->graph(1)->setPen(QPen(Qt::red));
        //    customPlot->graph(1)->setBrush(QBrush(QPixmap(":/image/balboa.jpg")));
        customPlot->graph(1)->setLineStyle(QCPGraph::lsStepCenter);
        customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::red, Qt::white, 7));
        customPlot->graph(1)->setName("下：曲线2");
        QCPErrorBars* errorBars = new QCPErrorBars(customPlot->xAxis, customPlot->yAxis);
        errorBars->removeFromLegend();
        errorBars->setDataPlottable(customPlot->graph(1));

        customPlot->addGraph(customPlot->xAxis2, customPlot->yAxis2);
        customPlot->graph(2)->setPen(QPen(Qt::blue));
        customPlot->graph(2)->setName("上：曲线3");

        customPlot->addGraph(customPlot->xAxis2, customPlot->yAxis2);
        QPen blueDotPen;
        blueDotPen.setColor(QColor(30, 40, 255, 150));
        blueDotPen.setStyle(Qt::DotLine);
        blueDotPen.setWidthF(10);
        customPlot->graph(3)->setPen(blueDotPen);
        customPlot->graph(3)->setName("上：曲线4");

        customPlot->addGraph(customPlot->yAxis2, customPlot->xAxis2);
        customPlot->graph(4)->setPen(QColor(50, 50, 50, 255));
        customPlot->graph(4)->setLineStyle(QCPGraph::lsNone);
        customPlot->graph(4)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 8));
        customPlot->graph(4)->setName("点围成的抛物线");

        QVector<double> x0(25), y0(25);
        QVector<double> x1(15), y1(15), y1err(15);
        QVector<double> x2(250), y2(250);
        QVector<double> x3(250), y3(250);
        QVector<double> x4(250), y4(250);
        for (int i = 0; i < 25; ++i) {
            x0[i] = 3 * i / 25.0;
            y0[i] = qExp(-x0[i] * x0[i] * 0.8) * (x0[i] * x0[i] + x0[i]);
        }
        for (int i = 0; i < 15; ++i) {
            x1[i] = 3 * i / 15.0;
            ;
            y1[i] = qExp(-x1[i] * x1[i]) * (x1[i] * x1[i]) * 2.6;
            y1err[i] = y1[i] * 0.25;
        }
        for (int i = 0; i < 250; ++i) {
            x2[i] = i / 250.0 * 3 * M_PI;
            x3[i] = x2[i];
            x4[i] = i / 250.0 * 100 - 50;
            y2[i] = qSin(x2[i] * 12) * qCos(x2[i]) * 10;
            y3[i] = qCos(x3[i]) * 10;
            y4[i] = 0.01 * x4[i] * x4[i] + 1.5 * (rand() / (double)RAND_MAX - 0.5) + 1.5 * M_PI;
        }

        customPlot->graph(0)->setData(x0, y0);
        customPlot->graph(1)->setData(x1, y1);
        errorBars->setData(y1err);
        customPlot->graph(2)->setData(x2, y2);
        customPlot->graph(3)->setData(x3, y3);
        customPlot->graph(4)->setData(x4, y4);

        customPlot->xAxis2->setVisible(true);
        customPlot->yAxis2->setVisible(true);

        customPlot->xAxis->setRange(0, 2.7);
        customPlot->yAxis->setRange(0, 2.6);
        customPlot->xAxis2->setRange(0, 3.0 * M_PI);
        customPlot->yAxis2->setRange(-70, 35);

        customPlot->xAxis2->setTicker(QSharedPointer<QCPAxisTickerPi>(new QCPAxisTickerPi));

        //    customPlot->plotLayout()->insertRow(0);
        //    customPlot->plotLayout()->addElement(0, 0, new QCPTextElement(customPlot, "一个图中的图形太多", QFont("sans", 12, QFont::Bold)));

        customPlot->xAxis->setLabel("底轴带有向外的刻度");
        customPlot->yAxis->setLabel("左轴标签");
        customPlot->xAxis2->setLabel("上轴标签");
        customPlot->yAxis2->setLabel("右轴标签");

        customPlot->xAxis->setTickLength(0, 5);
        customPlot->xAxis->setSubTickLength(0, 3);
        customPlot->yAxis2->setTickLength(3, 3);
        customPlot->yAxis2->setSubTickLength(1, 1);

        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("动态图1", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        //设置布局 首先新增一行一列用于放置标题和图例，这里将图例放置在一个空的轴矩形当中
        customPlot->plotLayout()->insertRow(0);
        customPlot->plotLayout()->insertColumn(0);

        QCPAxisRect* legendRect = new QCPAxisRect(customPlot, false);               // 不需要配置轴，因为我们只是把图例放在这里
        legendRect->setAutoMargins(QCP::msNone);                                    // 不计算边距
        legendRect->insetLayout()->addElement(customPlot->legend, Qt::AlignCenter); // 放置图例
        customPlot->legend->setVisible(true);
        customPlot->legend->setFillOrder(QCPLayoutGrid::foColumnsFirst); // 横向图例

        customPlot->axisRect()->setAutoMargins(QCP::msBottom | QCP::msRight); // 只计算下边距和右边距
        customPlot->plotLayout()->addElement(0, 0, new QCPTextElement(customPlot, "动态数据", QFont("sans", 12, QFont::Bold)));
        customPlot->plotLayout()->addElement(0, 1, legendRect);
        customPlot->plotLayout()->setRowStretchFactor(0, 0.1); // 让其尽可能小一点
        customPlot->plotLayout()->setColumnStretchFactor(0, 0.1);

        //设置图表
        /*
        添加一个柱状图和折线图，并且添加它们的数据，注意这里采用QCPAxisTickerText而不采用QCPAxisTickerTime或者
        QCPAxisTickerDateTime的原因，是因为我们不想要它们计算刻度线的位置，同时要注意让xAxis2保持与xAxis的联动，
        这里只需连接信号槽即可
        */

        // 设置柱状图
        QCPBars* bar = new QCPBars(customPlot->xAxis, customPlot->yAxis2);
        bar->setName("预购队列");
        bar->setPen(Qt::NoPen);
        bar->setBrush(QColor(194, 53, 49));

        // 设置折线图
        QCPGraph* graph = customPlot->addGraph(customPlot->xAxis, customPlot->yAxis);
        graph->setName("最新成交价");
        graph->setPen(QPen(QColor(47, 69, 84), 2));
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(47, 69, 84), QColor(Qt::white), 4));

        customPlot->yAxis->setLabel("价格");
        customPlot->yAxis->setRange(0, 30);
        customPlot->yAxis->setSubTicks(false);  // 设置子刻度线不显示
        customPlot->yAxis->setTickLength(0, 6); // 设置刻度线内外的长度
        customPlot->yAxis2->setLabel("预购量");
        customPlot->yAxis2->setVisible(true);
        customPlot->yAxis2->setRange(0, 1200);
        customPlot->yAxis2->setSubTicks(false);
        customPlot->yAxis2->setTickLength(0, 6);

        // 这里之所以用 QCPAxisTickerText，而不用 QCPAxisTickerTime / QCPAxisTickerDateTime 是因为它们会自动计算刻度线的位置，这是我们不想要的
        QSharedPointer<QCPAxisTickerText> timeTicker(new QCPAxisTickerText);
        QSharedPointer<QCPAxisTickerText> indexTicker(new QCPAxisTickerText);
        customPlot->xAxis->setTicker(timeTicker);
        customPlot->xAxis->setSubTicks(false);
        customPlot->xAxis2->setVisible(true);
        customPlot->xAxis2->setSubTicks(false);
        customPlot->xAxis2->setTicker(indexTicker);
        connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange))); // 重要：让xAxis2与xAxis保持联动

        // 添加数据
        QVector<double> datax, datay1, datay2;
        mLabels.clear();
        mIndexLabels.clear();
        QTime now = QTime::currentTime();
        for (int i = 0; i < 8; ++i) {
            mLabels.insert(0, now.toString("hh:mm:ss"));          // mLabels == QVector<QString>
            datax.insert(0, now.msecsSinceStartOfDay() / 1000.0); // 时间
            now = now.addMSecs(-2000);

            mIndexLabels.insert(0, QString::number(8 - i));
            datay1.insert(0, QRandomGenerator::global()->bounded(20) + 5); // 产生随机数，Qt 5.10 以下用qrand() % 20
            datay2.insert(0, QRandomGenerator::global()->bounded(1000));
        }

        mIndex = 8;
        mPositions = datax;

        graph->setData(datax, datay1);
        bar->setData(datax, datay2);

        timeTicker->setTicks(mPositions, mLabels); // 设置刻度线位置和刻度标签
        indexTicker->setTicks(mPositions, mIndexLabels);

        customPlot->xAxis->rescale();
        customPlot->xAxis->scaleRange(1.1); // 稍微缩放一下，以显示全部

        connect(&timer, &QTimer::timeout, this, [this] {
            QCPBars* bar = nullptr;
            bar = static_cast<QCPBars*>(customPlot->plottable(0));
            //去除之前的数据
            bar->data()->remove(mPositions.first());
            customPlot->graph(0)->data()->remove(mPositions.first());
            mPositions.removeFirst();
            mLabels.removeFirst();
            mIndexLabels.removeFirst();

            //添加新增数据
            // 计算当前时间
            double key = QTime::currentTime().msecsSinceStartOfDay() / 1000.0;
            QString label = QTime::currentTime().toString("hh:mm:ss");

            customPlot->graph(0)->addData(key, QRandomGenerator::global()->bounded(20) + 5);
            bar->addData(key, QRandomGenerator::global()->bounded(1000));

            mPositions.push_back(key);
            mLabels.push_back(label);
            mIndexLabels.push_back(QString::number(++mIndex));

            //重新设置轴并刷新图表
            QCPAxisTickerText* timeTicker = static_cast<QCPAxisTickerText*>(customPlot->xAxis->ticker().data());
            timeTicker->setTicks(mPositions, mLabels); // 重新设置x轴
            QCPAxisTickerText* indexTicker = static_cast<QCPAxisTickerText*>(customPlot->xAxis2->ticker().data());
            indexTicker->setTicks(mPositions, mIndexLabels);
            customPlot->xAxis->rescale();
            customPlot->xAxis->scaleRange(1.1);
            customPlot->replot(QCustomPlot::rpQueuedReplot);
        });
        timer.start(2000); // 2秒刷新一次

        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("动态轴游标", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        connect(customPlot->yAxis2, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis, SLOT(setRange(QCPRange)));
        customPlot->axisRect()->addAxis(QCPAxis::atRight);
        customPlot->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(30);
        customPlot->axisRect()->axis(QCPAxis::atRight, 1)->setPadding(30);

        mGraph1 = customPlot->addGraph(customPlot->xAxis, customPlot->axisRect()->axis(QCPAxis::atRight, 0));
        mGraph1->setPen(QPen(QColor(250, 120, 0)));

        mGraph2 = customPlot->addGraph(customPlot->xAxis, customPlot->axisRect()->axis(QCPAxis::atRight, 1));
        mGraph2->setPen(QPen(QColor(0, 180, 60)));

        mTag1 = new AxisTag(mGraph1->valueAxis());
        mTag1->setPen(mGraph1->pen());
        mTag2 = new AxisTag(mGraph2->valueAxis());
        mTag2->setPen(mGraph2->pen());

        connect(&timer, &QTimer::timeout, this, [this] {
            mGraph1->addData(mGraph1->dataCount(), qSin(mGraph1->dataCount() / 50.0) + qSin(mGraph1->dataCount() / 50.0 / 0.3843) * 0.25);
            mGraph2->addData(mGraph2->dataCount(), qCos(mGraph2->dataCount() / 50.0) + qSin(mGraph2->dataCount() / 50.0 / 0.4364) * 0.15);

            customPlot->xAxis->rescale(); //图表缩放
            mGraph1->rescaleValueAxis(false, true);
            mGraph2->rescaleValueAxis(false, true);
            customPlot->xAxis->setRange(customPlot->xAxis->range().upper, 100, Qt::AlignRight); //[lower, upper]x轴的范围
            //设置图表x轴范围 [upper-100，upper]

            double graph1Value = mGraph1->dataMainValue(mGraph1->dataCount() - 1);
            double graph2Value = mGraph2->dataMainValue(mGraph2->dataCount() - 1);
            mTag1->updatePosition(graph1Value);
            mTag2->updatePosition(graph2Value);
            mTag1->setText(QString::number(graph1Value, 'f', 2));
            mTag2->setText(QString::number(graph2Value, 'f', 2));

            customPlot->replot(QCustomPlot::rpQueuedReplot); //刷新图表
        });
        timer.start(80);

        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("动态图2", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        customPlot->setNotAntialiasedElements(QCP::aeAll);
        QFont font;
        font.setStyleStrategy(QFont::NoAntialias);
        customPlot->xAxis->setTickLabelFont(font);
        customPlot->yAxis->setTickLabelFont(font);
        customPlot->legend->setFont(font);

        customPlot->addGraph();
        customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
        customPlot->addGraph();
        customPlot->graph(1)->setPen(QPen(QColor(255, 110, 40)));

        QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
        timeTicker->setTimeFormat("%h:%m:%s");
        customPlot->xAxis->setTicker(timeTicker);
        customPlot->axisRect()->setupFullAxesBox();
        customPlot->yAxis->setRange(-1.2, 1.2);

        connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
        connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
        lastPointKey = 0;
        time = QTime::currentTime();
        connect(&timer, &QTimer::timeout, this, [this] {
            double key = time.elapsed() / 1000.0;
            if (key - lastPointKey > 0.05) // at most add point every 2 ms
            {
                customPlot->graph(0)->addData(key, qSin(key) + qrand() / (double)RAND_MAX * 1 * qSin(key / 0.3843));
                customPlot->graph(1)->addData(key, qCos(key) + qrand() / (double)RAND_MAX * 0.5 * qSin(key / 0.4364));
                // rescale value (vertical) axis to fit the current data:
                //customPlot->graph(0)->rescaleValueAxis();
                //customPlot->graph(1)->rescaleValueAxis(true);
                lastPointKey = key;
            }
            //使键轴范围与数据一起滚动（范围大小恒定为8）：
            customPlot->xAxis->setRange(key, 8, Qt::AlignRight);
            customPlot->replot(QCustomPlot::rpQueuedReplot);
        });
        timer.start(60);

        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("与图表交互", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        interactiveWidget* w = new interactiveWidget(nullptr, customPlot);
        w->move((desktop->width() - 1200) / 2, 30);
        w->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("散点样式", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        customPlot->legend->setFont(QFont("Helvetica", 9));
        customPlot->legend->setRowSpacing(-3);

        QVector<QCPScatterStyle::ScatterShape> shapes;
        shapes << QCPScatterStyle::ssCross;
        shapes << QCPScatterStyle::ssPlus;
        shapes << QCPScatterStyle::ssCircle;
        shapes << QCPScatterStyle::ssDisc;
        shapes << QCPScatterStyle::ssSquare;
        shapes << QCPScatterStyle::ssDiamond;
        shapes << QCPScatterStyle::ssStar;
        shapes << QCPScatterStyle::ssTriangle;
        shapes << QCPScatterStyle::ssTriangleInverted;
        shapes << QCPScatterStyle::ssCrossSquare;
        shapes << QCPScatterStyle::ssPlusSquare;
        shapes << QCPScatterStyle::ssCrossCircle;
        shapes << QCPScatterStyle::ssPlusCircle;
        shapes << QCPScatterStyle::ssPeace;
        shapes << QCPScatterStyle::ssCustom;

        QPen pen;
        for (int i = 0; i < shapes.size(); ++i) {
            customPlot->addGraph();
            pen.setColor(QColor(qSin(i * 0.3) * 100 + 100, qSin(i * 0.6 + 0.7) * 100 + 100, qSin(i * 0.4 + 0.6) * 100 + 100));
            QVector<double> x(10), y(10);
            for (int k = 0; k < 10; ++k) {
                x[k] = k / 10.0 * 4 * 3.14 + 0.01;
                y[k] = 7 * qSin(x[k]) / x[k] + (shapes.size() - i) * 5;
            }
            customPlot->graph()->setData(x, y);
            customPlot->graph()->rescaleAxes(true);
            customPlot->graph()->setPen(pen);
            customPlot->graph()->setName(QCPScatterStyle::staticMetaObject.enumerator(QCPScatterStyle::staticMetaObject.indexOfEnumerator("ScatterShape")).valueToKey(shapes.at(i)));
            customPlot->graph()->setLineStyle(QCPGraph::lsLine);

            if (shapes.at(i) != QCPScatterStyle::ssCustom) {
                customPlot->graph()->setScatterStyle(QCPScatterStyle(shapes.at(i), 16));
            } else {
                QPainterPath customScatterPath;
                for (int i = 0; i < 3; ++i) {
                    customScatterPath.cubicTo(qCos(2 * M_PI * i / 3.0) * 9, qSin(2 * M_PI * i / 3.0) * 9, qCos(2 * M_PI * (i + 0.9) / 3.0) * 9, qSin(2 * M_PI * (i + 0.9) / 3.0) * 9, 0, 0);
                }
                customPlot->graph()->setScatterStyle(QCPScatterStyle(customScatterPath, QPen(Qt::black, 0), QColor(40, 70, 255, 50), 16));
            }
        }

        customPlot->rescaleAxes();
        customPlot->xAxis->setTicks(false);
        customPlot->yAxis->setTicks(false);
        customPlot->xAxis->setTickLabels(false);
        customPlot->yAxis->setTickLabels(false);

        customPlot->axisRect()->setupFullAxesBox();
        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("自定义散点样式", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        customPlot->axisRect()->setBackground(QPixmap(":/image/solarpanels.jpg")); //图表背景

        customPlot->addGraph();
        customPlot->graph()->setLineStyle(QCPGraph::lsLine);

        QPen pen;
        pen.setColor(QColor(255, 200, 20, 200));
        pen.setStyle(Qt::DashLine);
        pen.setWidthF(2.5);

        customPlot->graph()->setPen(pen);
        customPlot->graph()->setBrush(QBrush(QColor(255, 200, 20, 70)));
        customPlot->graph()->setScatterStyle(QCPScatterStyle(QPixmap(":/image/sun.png")));

        customPlot->graph()->setName("2011年光伏能源晴雨表数据");

        QVector<double> year, value;
        year << 2005 << 2006 << 2007 << 2008 << 2009 << 2010 << 2011;
        value << 2.17 << 3.42 << 4.94 << 10.38 << 15.86 << 29.33 << 52.1;
        customPlot->graph()->setData(year, value);

        //  customPlot->plotLayout()->insertRow(0);//在第0列插入一行
        //  customPlot->plotLayout()->addElement(0, 0, new QCPTextElement(customPlot, "再生能源", QFont("sans", 12, QFont::Bold)));

        customPlot->xAxis->setLabel("年");
        customPlot->yAxis->setLabel("千兆瓦光电");
        customPlot->xAxis2->setVisible(true);
        customPlot->yAxis2->setVisible(true);
        customPlot->xAxis2->setTickLabels(false);
        customPlot->yAxis2->setTickLabels(false);
        customPlot->xAxis2->setTicks(false);
        customPlot->yAxis2->setTicks(false);
        customPlot->xAxis2->setSubTicks(false);
        customPlot->yAxis2->setSubTicks(false);
        customPlot->xAxis->setRange(2004.5, 2011.5);
        customPlot->yAxis->setRange(0, 52);

        customPlot->legend->setFont(QFont(font().family(), 7));
        customPlot->legend->setIconSize(50, 20);
        customPlot->legend->setVisible(true);
        customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft | Qt::AlignTop);

        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("纹理笔刷", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        customPlot->addGraph();

        QPen redDotPen;
        redDotPen.setStyle(Qt::DotLine);
        redDotPen.setColor(QColor(170, 100, 100, 180));
        redDotPen.setWidthF(2);
        customPlot->graph(0)->setPen(redDotPen);
        customPlot->graph(0)->setBrush(QBrush(QPixmap(":/image/balboa.jpg")));

        customPlot->addGraph();
        customPlot->graph(1)->setPen(QPen(Qt::red));

        customPlot->graph(0)->setChannelFillGraph(customPlot->graph(1));

        QVector<double> x(250);
        QVector<double> y0(250), y1(250);
        for (int i = 0; i < 250; ++i) {
            x[i] = 3 * i / 250.0;
            y0[i] = 1 + qExp(-x[i] * x[i] * 0.8) * (x[i] * x[i] + x[i]);
            y1[i] = 1 - qExp(-x[i] * x[i] * 0.4) * (x[i] * x[i]) * 0.1;
        }

        customPlot->graph(0)->setData(x, y0);
        customPlot->graph(1)->setData(x, y1);

        customPlot->xAxis2->setVisible(true);
        customPlot->yAxis2->setVisible(true);
        customPlot->xAxis2->setTickLabels(false);
        customPlot->yAxis2->setTickLabels(false);

        customPlot->xAxis->setRange(0, 2.5);
        customPlot->yAxis->setRange(0.9, 1.6);

        customPlot->axisRect()->setupFullAxesBox();

        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("柱状图", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        QLinearGradient gradient(0, 0, 0, 400);
        gradient.setColorAt(0, QColor(90, 90, 90));
        gradient.setColorAt(0.38, QColor(105, 105, 105));
        gradient.setColorAt(1, QColor(70, 70, 70));
        customPlot->setBackground(QBrush(gradient));

        QCPBars* regen = new QCPBars(customPlot->xAxis, customPlot->yAxis);
        regen->setAntialiased(false);
        regen->setStackingGap(1);
        regen->setName("可再生的");
        regen->setPen(QPen(QColor(0, 168, 140).lighter(130)));
        regen->setBrush(QColor(0, 168, 140));

        QCPBars* nuclear = new QCPBars(customPlot->xAxis, customPlot->yAxis);
        nuclear->setAntialiased(false);
        nuclear->setStackingGap(1);
        nuclear->setName("核能");
        nuclear->setPen(QPen(QColor(250, 170, 20).lighter(150)));
        nuclear->setBrush(QColor(250, 170, 20));

        QCPBars* fossil = new QCPBars(customPlot->xAxis, customPlot->yAxis);
        fossil->setAntialiased(false);
        fossil->setStackingGap(1);
        fossil->setName("化石燃料");
        fossil->setPen(QPen(QColor(111, 9, 176).lighter(170)));
        fossil->setBrush(QColor(111, 9, 176));

        nuclear->moveAbove(fossil); //移到它上面
        regen->moveAbove(nuclear);  //在最上层

        QVector<double> ticks;
        QVector<QString> labels;
        ticks << 1 << 2 << 3 << 4 << 5 << 6 << 7;
        labels << "美国"
               << "日本"
               << "德国"
               << "法国"
               << "英国"
               << "意大利"
               << "加拿大";
        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        textTicker->addTicks(ticks, labels);
        customPlot->xAxis->setTicker(textTicker);
        customPlot->xAxis->setTickLabelRotation(60);
        customPlot->xAxis->setSubTicks(false);
        customPlot->xAxis->setTickLength(0, 4);
        customPlot->xAxis->setRange(0, 8);
        customPlot->xAxis->setBasePen(QPen(Qt::white));
        customPlot->xAxis->setTickPen(QPen(Qt::white));
        customPlot->xAxis->grid()->setVisible(true);
        customPlot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
        customPlot->xAxis->setTickLabelColor(Qt::white);
        customPlot->xAxis->setLabelColor(Qt::white);

        customPlot->yAxis->setRange(0, 12.1);
        customPlot->yAxis->setPadding(5); //左边框再宽一点
        customPlot->yAxis->setLabel("人均用电量（2007）");
        customPlot->yAxis->setBasePen(QPen(Qt::white));
        customPlot->yAxis->setTickPen(QPen(Qt::white));
        customPlot->yAxis->setSubTickPen(QPen(Qt::white));
        customPlot->yAxis->grid()->setSubGridVisible(true);
        customPlot->yAxis->setTickLabelColor(Qt::white);
        customPlot->yAxis->setLabelColor(Qt::white);
        customPlot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
        customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

        QVector<double> fossilData, nuclearData, regenData;
        fossilData << 0.86 * 10.5 << 0.83 * 5.5 << 0.84 * 5.5 << 0.52 * 5.8 << 0.89 * 5.2 << 0.90 * 4.2 << 0.67 * 11.2;
        nuclearData << 0.08 * 10.5 << 0.12 * 5.5 << 0.12 * 5.5 << 0.40 * 5.8 << 0.09 * 5.2 << 0.00 * 4.2 << 0.07 * 11.2;
        regenData << 0.06 * 10.5 << 0.05 * 5.5 << 0.04 * 5.5 << 0.06 * 5.8 << 0.02 * 5.2 << 0.07 * 4.2 << 0.25 * 11.2;
        fossil->setData(ticks, fossilData);
        nuclear->setData(ticks, nuclearData);
        regen->setData(ticks, regenData);

        customPlot->legend->setVisible(true);
        customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignHCenter);
        customPlot->legend->setBrush(QColor(255, 255, 255, 100));
        customPlot->legend->setBorderPen(Qt::NoPen);
        QFont legendFont = font();
        legendFont.setPointSize(10);
        customPlot->legend->setFont(legendFont);
        customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("箱形图", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        QCPStatisticalBox* statistical = new QCPStatisticalBox(customPlot->xAxis, customPlot->yAxis);
        QBrush boxBrush(QColor(60, 60, 255, 100));
        boxBrush.setStyle(Qt::Dense6Pattern);
        statistical->setBrush(boxBrush);

        statistical->addData(1, 1.1, 1.9, 2.25, 2.7, 4.2);
        statistical->addData(2, 0.8, 1.6, 2.2, 3.2, 4.9, QVector<double>() << 0.7 << 0.34 << 0.45 << 6.2 << 5.84); // provide some outliers as QVector
        statistical->addData(3, 0.2, 0.7, 1.1, 1.6, 2.9);

        customPlot->xAxis->setSubTicks(false);
        customPlot->xAxis->setTickLength(0, 4);
        customPlot->xAxis->setTickLabelRotation(20);
        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        textTicker->addTick(1, "样本1");
        textTicker->addTick(2, "样本2");
        textTicker->addTick(3, "对照组");
        customPlot->xAxis->setTicker(textTicker);

        customPlot->yAxis->setLabel(QString::fromUtf8("二氧化碳吸收量[mg]"));
        customPlot->rescaleAxes();
        customPlot->xAxis->scaleRange(1.7, customPlot->xAxis->range().center());
        customPlot->yAxis->setRange(0, 7);
        customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("指向一点的文本箭头", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

        QCPItemText* textLabel = new QCPItemText(customPlot);
        textLabel->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
        textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
        textLabel->position->setCoords(0.5, 0); // 将位置放置在轴矩形的中心/顶部
        textLabel->setText("指向点（4,1.6）");
        textLabel->setFont(QFont(font().family(), 16)); // 使字体大一点
        textLabel->setPen(QPen(Qt::black));             // 在文本周围显示黑色边框

        QCPItemLine* arrow = new QCPItemLine(customPlot);
        arrow->start->setParentAnchor(textLabel->bottom);
        arrow->end->setCoords(4, 1.6);
        arrow->setHead(QCPLineEnding::esSpikeArrow);

        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("目标箭头", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        QCPGraph* graph = customPlot->addGraph();
        int n = 500;
        double phase = 0;
        double k = 3;
        QVector<double> x(n), y(n);
        for (int i = 0; i < n; ++i) {
            x[i] = i / (double)(n - 1) * 34 - 17;
            y[i] = qExp(-x[i] * x[i] / 20.0) * qSin(k * x[i] + phase);
        }
        graph->setData(x, y);
        graph->setPen(QPen(Qt::blue));
        graph->rescaleKeyAxis();
        customPlot->yAxis->setRange(-1.45, 1.65);
        customPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);

        //括号
        QCPItemBracket* bracket = new QCPItemBracket(customPlot);
        bracket->left->setCoords(-8, 1.1);
        bracket->right->setCoords(8, 1.1);
        bracket->setLength(13);

        //文本标签
        QCPItemText* wavePacketText = new QCPItemText(customPlot);
        wavePacketText->position->setParentAnchor(bracket->center);
        wavePacketText->position->setCoords(0, -10); // 从括号中心定位向上移动10个像素
        wavePacketText->setPositionAlignment(Qt::AlignBottom | Qt::AlignHCenter);
        wavePacketText->setText("波包（叠加后的波）");
        wavePacketText->setFont(QFont(font().family(), 10));

        // 添加粘在图形数据上的相位跟踪器（红色圆圈）
        QCPItemTracer* phaseTracer = new QCPItemTracer(customPlot);
        itemDemoPhaseTracer = phaseTracer; // so we can access it later in the bracketDataSlot for animation
        phaseTracer->setGraph(graph);
        phaseTracer->setGraphKey((M_PI * 1.5 - phase) / k);
        phaseTracer->setInterpolating(true);
        phaseTracer->setStyle(QCPItemTracer::tsCircle);
        phaseTracer->setPen(QPen(Qt::red));
        phaseTracer->setBrush(Qt::red);
        phaseTracer->setSize(7);

        QCPItemText* phaseTracerText = new QCPItemText(customPlot);
        phaseTracerText->position->setType(QCPItemPosition::ptAxisRectRatio);
        phaseTracerText->setPositionAlignment(Qt::AlignRight | Qt::AlignBottom);
        phaseTracerText->position->setCoords(1.0, 0.95); // 直角轴右下角
        phaseTracerText->setText("固定相点定义相速度vp");
        phaseTracerText->setTextAlignment(Qt::AlignLeft);
        phaseTracerText->setFont(QFont(font().family(), 9));
        phaseTracerText->setPadding(QMargins(8, 0, 0, 0));

        // 添加指向相位跟踪器的箭头，来自标签：
        QCPItemCurve* phaseTracerArrow = new QCPItemCurve(customPlot);
        phaseTracerArrow->start->setParentAnchor(phaseTracerText->left);
        phaseTracerArrow->startDir->setParentAnchor(phaseTracerArrow->start);
        phaseTracerArrow->startDir->setCoords(-40, 0);
        phaseTracerArrow->end->setParentAnchor(phaseTracer->position);
        phaseTracerArrow->end->setCoords(10, 10);
        phaseTracerArrow->endDir->setParentAnchor(phaseTracerArrow->end);
        phaseTracerArrow->endDir->setCoords(30, 30);
        phaseTracerArrow->setHead(QCPLineEnding::esSpikeArrow);
        phaseTracerArrow->setTail(QCPLineEnding(QCPLineEnding::esBar, (phaseTracerText->bottom->pixelPosition().y() - phaseTracerText->top->pixelPosition().y()) * 0.85));

        // 添加组速度跟踪器（绿色圆圈）：
        QCPItemTracer* groupTracer = new QCPItemTracer(customPlot);
        groupTracer->setGraph(graph);
        groupTracer->setGraphKey(5.5);
        groupTracer->setInterpolating(true);
        groupTracer->setStyle(QCPItemTracer::tsCircle);
        groupTracer->setPen(QPen(Qt::green));
        groupTracer->setBrush(Qt::green);
        groupTracer->setSize(7);

        QCPItemText* groupTracerText = new QCPItemText(customPlot);
        groupTracerText->position->setType(QCPItemPosition::ptAxisRectRatio);
        groupTracerText->setPositionAlignment(Qt::AlignRight | Qt::AlignTop);
        groupTracerText->position->setCoords(1.0, 0.20);
        groupTracerText->setText("波包中的固定位置定义组速度vg");
        groupTracerText->setTextAlignment(Qt::AlignLeft);
        groupTracerText->setFont(QFont(font().family(), 9));
        groupTracerText->setPadding(QMargins(8, 0, 0, 0));

        QCPItemCurve* groupTracerArrow = new QCPItemCurve(customPlot);
        groupTracerArrow->start->setParentAnchor(groupTracerText->left);
        groupTracerArrow->startDir->setParentAnchor(groupTracerArrow->start);
        groupTracerArrow->startDir->setCoords(-40, 0);
        groupTracerArrow->end->setCoords(5.5, 0.4);
        groupTracerArrow->endDir->setParentAnchor(groupTracerArrow->end);
        groupTracerArrow->endDir->setCoords(0, -40);
        groupTracerArrow->setHead(QCPLineEnding::esSpikeArrow);
        groupTracerArrow->setTail(QCPLineEnding(QCPLineEnding::esBar, (groupTracerText->bottom->pixelPosition().y() - groupTracerText->top->pixelPosition().y()) * 0.85));

        // 添加分散箭头:
        QCPItemCurve* arrow = new QCPItemCurve(customPlot);
        arrow->start->setCoords(1, -1.1);
        arrow->startDir->setCoords(-1, -1.3);
        arrow->endDir->setCoords(-5, -0.3);
        arrow->end->setCoords(-10, -0.2);
        arrow->setHead(QCPLineEnding::esSpikeArrow);

        // 添加分散箭头标签:
        QCPItemText* dispersionText = new QCPItemText(customPlot);
        dispersionText->position->setCoords(-6, -0.9);
        dispersionText->setRotation(40);
        dispersionText->setText("分散\nvp < vg");
        dispersionText->setFont(QFont(font().family(), 10));

        connect(&timer, &QTimer::timeout, this, [this] {
            double secs = QCPAxisTickerDateTime::dateTimeToKey(QDateTime::currentDateTime());

            int n = 500;
            double phase = secs * 5;
            double k = 3;
            QVector<double> x(n), y(n);
            for (int i = 0; i < n; ++i) {
                x[i] = i / (double)(n - 1) * 34 - 17;
                y[i] = qExp(-x[i] * x[i] / 20.0) * qSin(k * x[i] + phase);
            }
            customPlot->graph()->setData(x, y);
            itemDemoPhaseTracer->setGraphKey((8 * M_PI + fmod(M_PI * 1.5 - phase, 6 * M_PI)) / k);
            customPlot->replot(QCustomPlot::rpQueuedReplot);
        });
        timer.start(33);

        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("暗风格", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        QVector<double> x1(20), y1(20);
        QVector<double> x2(100), y2(100);
        QVector<double> x3(20), y3(20);
        QVector<double> x4(20), y4(20);
        for (int i = 0; i < x1.size(); ++i) {
            x1[i] = i / (double)(x1.size() - 1) * 10;
            y1[i] = qCos(x1[i] * 0.8 + qSin(x1[i] * 0.16 + 1.0)) * qSin(x1[i] * 0.54) + 1.4;
        }
        for (int i = 0; i < x2.size(); ++i) {
            x2[i] = i / (double)(x2.size() - 1) * 10;
            y2[i] = qCos(x2[i] * 0.85 + qSin(x2[i] * 0.165 + 1.1)) * qSin(x2[i] * 0.50) + 1.7;
        }
        for (int i = 0; i < x3.size(); ++i) {
            x3[i] = i / (double)(x3.size() - 1) * 10;
            y3[i] = 0.05 + 3 * (0.5 + qCos(x3[i] * x3[i] * 0.2 + 2) * 0.5) / (double)(x3[i] + 0.7) + qrand() / (double)RAND_MAX * 0.01;
        }
        for (int i = 0; i < x4.size(); ++i) {
            x4[i] = x3[i];
            y4[i] = (0.5 - y3[i]) + ((x4[i] - 2) * (x4[i] - 2) * 0.02);
        }

        QCPGraph* graph1 = customPlot->addGraph();
        graph1->setData(x1, y1);
        graph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
        graph1->setPen(QPen(QColor(120, 120, 120), 2));

        QCPGraph* graph2 = customPlot->addGraph();
        graph2->setData(x2, y2);
        graph2->setPen(Qt::NoPen);
        graph2->setBrush(QColor(200, 200, 200, 20));
        graph2->setChannelFillGraph(graph1);

        QCPBars* bars1 = new QCPBars(customPlot->xAxis, customPlot->yAxis);
        bars1->setWidth(9 / (double)x3.size());
        bars1->setData(x3, y3);
        bars1->setPen(Qt::NoPen);
        bars1->setBrush(QColor(10, 140, 70, 160));

        QCPBars* bars2 = new QCPBars(customPlot->xAxis, customPlot->yAxis);
        bars2->setWidth(9 / (double)x4.size());
        bars2->setData(x4, y4);
        bars2->setPen(Qt::NoPen);
        bars2->setBrush(QColor(10, 100, 50, 70));
        bars2->moveAbove(bars1);

        customPlot->addLayer("abovemain", customPlot->layer("main"), QCustomPlot::limAbove);
        customPlot->addLayer("belowmain", customPlot->layer("main"), QCustomPlot::limBelow);
        graph1->setLayer("abovemain");
        customPlot->xAxis->grid()->setLayer("belowmain");
        customPlot->yAxis->grid()->setLayer("belowmain");

        customPlot->xAxis->setBasePen(QPen(Qt::white, 1));
        customPlot->yAxis->setBasePen(QPen(Qt::white, 1));
        customPlot->xAxis->setTickPen(QPen(Qt::white, 1));
        customPlot->yAxis->setTickPen(QPen(Qt::white, 1));
        customPlot->xAxis->setSubTickPen(QPen(Qt::white, 1));
        customPlot->yAxis->setSubTickPen(QPen(Qt::white, 1));
        customPlot->xAxis->setTickLabelColor(Qt::white);
        customPlot->yAxis->setTickLabelColor(Qt::white);
        customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
        customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
        customPlot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
        customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
        customPlot->xAxis->grid()->setSubGridVisible(true);
        customPlot->yAxis->grid()->setSubGridVisible(true);
        customPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
        customPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
        customPlot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
        customPlot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
        QLinearGradient plotGradient;
        plotGradient.setStart(0, 0);
        plotGradient.setFinalStop(0, 350);
        plotGradient.setColorAt(0, QColor(80, 80, 80));
        plotGradient.setColorAt(1, QColor(50, 50, 50));
        customPlot->setBackground(plotGradient);
        QLinearGradient axisRectGradient;
        axisRectGradient.setStart(0, 0);
        axisRectGradient.setFinalStop(0, 350);
        axisRectGradient.setColorAt(0, QColor(80, 80, 80));
        axisRectGradient.setColorAt(1, QColor(30, 30, 30));
        customPlot->axisRect()->setBackground(axisRectGradient);

        customPlot->rescaleAxes();
        customPlot->yAxis->setRange(0, 2);

        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("财务图表", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        int n = 500;
        QVector<double> time(n), value1(n), value2(n);
        QDateTime start = QDateTime(QDate(2014, 6, 11));
        start.setTimeSpec(Qt::UTC);
        double startTime = start.toTime_t();
        double binSize = 3600 * 24;
        time[0] = startTime;
        value1[0] = 60;
        value2[0] = 20;
        qsrand(9);
        for (int i = 1; i < n; ++i) {
            time[i] = startTime + 3600 * i;
            value1[i] = value1[i - 1] + (qrand() / (double)RAND_MAX - 0.5) * 10;
            value2[i] = value2[i - 1] + (qrand() / (double)RAND_MAX - 0.5) * 3;
        }

        // create candlestick chart:
        QCPFinancial* candlesticks = new QCPFinancial(customPlot->xAxis, customPlot->yAxis);
        candlesticks->setName("Candlestick");
        candlesticks->setChartStyle(QCPFinancial::csCandlestick);
        candlesticks->data()->set(QCPFinancial::timeSeriesToOhlc(time, value1, binSize, startTime));
        candlesticks->setWidth(binSize * 0.9);
        candlesticks->setTwoColored(true);
        candlesticks->setBrushPositive(QColor(245, 245, 245));
        candlesticks->setBrushNegative(QColor(40, 40, 40));
        candlesticks->setPenPositive(QPen(QColor(0, 0, 0)));
        candlesticks->setPenNegative(QPen(QColor(0, 0, 0)));

        // create ohlc chart:
        QCPFinancial* ohlc = new QCPFinancial(customPlot->xAxis, customPlot->yAxis);
        ohlc->setName("OHLC");
        ohlc->setChartStyle(QCPFinancial::csOhlc);
        ohlc->data()->set(QCPFinancial::timeSeriesToOhlc(time, value2, binSize / 3.0, startTime)); // divide binSize by 3 just to make the ohlc bars a bit denser
        ohlc->setWidth(binSize * 0.2);
        ohlc->setTwoColored(true);

        QCPAxisRect* volumeAxisRect = new QCPAxisRect(customPlot);
        customPlot->plotLayout()->addElement(1, 0, volumeAxisRect);
        volumeAxisRect->setMaximumSize(QSize(QWIDGETSIZE_MAX, 100));
        volumeAxisRect->axis(QCPAxis::atBottom)->setLayer("axes");
        volumeAxisRect->axis(QCPAxis::atBottom)->grid()->setLayer("grid");

        customPlot->plotLayout()->setRowSpacing(0);
        volumeAxisRect->setAutoMargins(QCP::msLeft | QCP::msRight | QCP::msBottom);
        volumeAxisRect->setMargins(QMargins(0, 0, 0, 0));
        //为正（绿色）和负（红色）体积条创建两个条形图
        customPlot->setAutoAddPlottableToLegend(false);
        QCPBars* volumePos = new QCPBars(volumeAxisRect->axis(QCPAxis::atBottom), volumeAxisRect->axis(QCPAxis::atLeft));
        QCPBars* volumeNeg = new QCPBars(volumeAxisRect->axis(QCPAxis::atBottom), volumeAxisRect->axis(QCPAxis::atLeft));
        for (int i = 0; i < n / 5; ++i) {
            int v = qrand() % 20000 + qrand() % 20000 + qrand() % 20000 - 10000 * 3;
            (v < 0 ? volumeNeg : volumePos)->addData(startTime + 3600 * 5.0 * i, qAbs(v)); // add data to either volumeNeg or volumePos, depending on sign of v
        }
        volumePos->setWidth(3600 * 4);
        volumePos->setPen(Qt::NoPen);
        volumePos->setBrush(QColor(100, 180, 110));
        volumeNeg->setWidth(3600 * 4);
        volumeNeg->setPen(Qt::NoPen);
        volumeNeg->setBrush(QColor(180, 90, 90));

        connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), volumeAxisRect->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
        connect(volumeAxisRect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis, SLOT(setRange(QCPRange)));
        QSharedPointer<QCPAxisTickerDateTime> dateTimeTicker(new QCPAxisTickerDateTime);
        dateTimeTicker->setDateTimeSpec(Qt::UTC);
        dateTimeTicker->setDateTimeFormat("dd. MMMM");
        volumeAxisRect->axis(QCPAxis::atBottom)->setTicker(dateTimeTicker);
        volumeAxisRect->axis(QCPAxis::atBottom)->setTickLabelRotation(15);
        customPlot->xAxis->setBasePen(Qt::NoPen);
        customPlot->xAxis->setTickLabels(false);
        customPlot->xAxis->setTicks(false); // 只希望主轴矩形中有垂直网格，因此隐藏xAxis主干、记号和标签
        customPlot->xAxis->setTicker(dateTimeTicker);
        customPlot->rescaleAxes();
        customPlot->xAxis->scaleRange(1.025, customPlot->xAxis->range().center());
        customPlot->yAxis->scaleRange(1.1, customPlot->yAxis->range().center());

        QCPMarginGroup* group = new QCPMarginGroup(customPlot);
        customPlot->axisRect()->setMarginGroup(QCP::msLeft | QCP::msRight, group);
        volumeAxisRect->setMarginGroup(QCP::msLeft | QCP::msRight, group);

        customPlot->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("图表保存为PDF", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();
        savePDFwindow* w = new savePDFwindow;
        w->show();
    });
    flowlayout->addWidget(btn);

    btn = new QPushButton("游标跟随", ui->scrollAreaWidgetContents);
    btn->setFixedSize(size);
    btn->setStyleSheet(style);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, [this] {
        init_CustomPlot();

        //设置基本坐标轴（左侧Y轴和下方X轴）可拖动、可缩放、曲线可选、legend可选、设置伸缩比例，使所有图例可见
        customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
        customPlot->legend->setVisible(true);
        customPlot->legend->setFont(QFont("Helvetica", 9));
        customPlot->legend->setBrush(QColor(255, 255, 255, 100));

        customPlot->addGraph();
        customPlot->graph(0)->setPen(QPen(Qt::blue));
        customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); //填充

        customPlot->addGraph();

        QVector<double> x(251), y(251);
        for (int i = 0; i < 251; ++i) {
            x[i] = i;
            y[i] = qExp(-i / 150.0) * qCos(i / 10.0);
        }
        customPlot->graph(0)->setData(x, y);

        customPlot->graph(0)->rescaleAxes();
        customPlot->axisRect()->axis(QCPAxis::atBottom, 0)->setPadding(10);

        m_TraserX = new myTracer(customPlot, customPlot->graph(0), XAxisTracer);
        m_TraserY = new myTracer(customPlot, customPlot->graph(0), YAxisTracer);
        m_TraserD = new myTracer(customPlot, customPlot->graph(0), DataTracer);
        connect(customPlot, &QCustomPlot::mouseMove, this, &MainWindow::showTracer);

        customPlot->xAxis->setPadding(35);

        customPlot->replot(QCustomPlot::rpQueuedReplot);
        customPlot->show();
    });
    flowlayout->addWidget(btn);
}

void
MainWindow::init_CustomPlot()
{
    if (customPlot != nullptr) {
        timer.stop();
        disconnect(&timer, &QTimer::timeout, this, nullptr);

        if (m_TraserX != nullptr) {
            delete m_TraserX;
            m_TraserX = nullptr;
        }
        if (m_TraserY != nullptr) {
            delete m_TraserY;
            m_TraserY = nullptr;
        }
        if (m_TraserD != nullptr) {
            delete m_TraserD;
            m_TraserD = nullptr;
        }
        delete customPlot;
        customPlot = nullptr;
    }

    customPlot = new QCustomPlot;
    customPlot->setMinimumSize(1200, 800);
    customPlot->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
    customPlot->close();
    customPlot->legend->setVisible(true); // 显示图例

    //自定义图例位置 这里按比例给出图例的位置，在QCPAxisRect的(60%,40%)的位置，宽高分别为QCPAxisRect的(20%,20%)宽高的大小
    //    customPlot->axisRect()->insetLayout()->setInsetPlacement(0, QCPLayoutInset::ipFree);
    //    customPlot->axisRect()->insetLayout()->setInsetRect(0, QRectF(0.6, 0.0, 0.2, 0.2));

    customPlot->move((desktop->width() - customPlot->width()) / 2,
                     (desktop->height() - customPlot->height()) / 2);

    //清除所有图形
    //    customPlot->clearGraphs();
    //    customPlot->clearPlottables();
    //    customPlot->setBackground(QBrush(QColor(QColor(255,255,255))));
    //    customPlot->setViewport(customPlot->rect());//刷新
    //    customPlot->replot(QCustomPlot::rpQueuedRefresh);

    //    QLinearGradient plotGradient;
    //    plotGradient.setStart(0, 0);
    //    plotGradient.setFinalStop(0, 350);
    //    plotGradient.setColorAt(0, QColor(80, 80, 80));
    //    plotGradient.setColorAt(1, QColor(50, 50, 50));
    //    customPlot->setBackground(plotGradient);      // 使用渐变设置背景颜色(整个窗口的背景)

    //    QLinearGradient axisRectGradient;
    //    axisRectGradient.setStart(0, 0);
    //    axisRectGradient.setFinalStop(0, 350);
    //    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
    //    axisRectGradient.setColorAt(1, QColor(30, 30, 30));
    //    customPlot->axisRect()->setBackground(axisRectGradient);   // 设置QCPAxisRect背景颜色（四根坐标轴围成的矩形区域的背景颜色）

    //设置下方的x轴的风格
    //    customPlot->xAxis->setBasePen(QPen(Qt::red, 1));  // 轴线的画笔
    //    customPlot->xAxis->setTickPen(QPen(Qt::blue, 1));  // 轴刻度线的画笔
    //    customPlot->xAxis->setSubTickPen(QPen(Qt::green, 1)); // 轴子刻度线的画笔
    //    customPlot->xAxis->setTickLabelColor(Qt::black);  // 轴刻度文字颜色
    //    customPlot->xAxis->setLabel("标签");  // 只有设置了标签，轴标签的颜色才会显示
    //    customPlot->xAxis->setLabelColor(Qt::red);   // 轴标签颜色
    //    customPlot->xAxis->setTickLengthIn(3);       // 轴线内刻度的长度
    //    customPlot->xAxis->setTickLengthOut(5);      // 轴线外刻度的长度
    //    customPlot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);  // 设置轴线结束时的风格为 实角三角形但内部有凹陷的形状， setLowerEnding设置轴线开始时的风格

    //设置QCPGrid网格的风格(每条网格对应一个刻度)
    //    customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));     // 网格线(对应刻度)画笔
    //    customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    //    customPlot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine)); // 子网格线(对应子刻度)画笔
    //    customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    //    customPlot->xAxis->grid()->setSubGridVisible(true);     // 显示子网格线
    //    customPlot->yAxis->grid()->setSubGridVisible(true);
    //    customPlot->xAxis->grid()->setZeroLinePen(QPen(Qt::red));   // 设置刻度为0时的网格线的画笔
    //    customPlot->yAxis->grid()->setZeroLinePen(QPen(Qt::red));
}

//生成随机颜色
QString
MainWindow::get_rand_color()
{
    QString color = "";
    QString strR = "";
    QString strG = "";
    QString strB = "";

    qsrand(QTime(0, 0, 0).msecsTo(QTime::currentTime()));

    bool ok = true;
    strR.setNum(rand() % 256, 16);
    if (strR.toInt(&ok, 16) < 16) {
        strR.prepend("0");
    }
    strG.setNum(rand() % 256, 16);
    if (strG.toInt(&ok, 16) < 16) {
        strG.prepend("0");
    }
    strB.setNum(rand() % 256, 16);
    if (strB.toInt(&ok, 16) < 16) {
        strB.prepend("0");
    }
    color.append("#" + strR + strG + strB);
    return color;
}

void
MainWindow::showTracer(QMouseEvent* event)
{
    double x = customPlot->xAxis->pixelToCoord(event->pos().x());
    m_TraserX->updatePosition(x, 0);
    m_TraserX->setText(QString::number(x, 'f', 2));

    double y = 0;
    QSharedPointer<QCPGraphDataContainer> tmpContainer;
    tmpContainer = customPlot->graph(0)->data();
    //使用二分法快速查找所在点数据
    int low = 0, high = tmpContainer->size();
    while (high > low) {
        int middle = (low + high) / 2;
        if (x < tmpContainer->constBegin()->mainKey() ||
            x > (tmpContainer->constEnd() - 1)->mainKey())
            break;

        if (x == (tmpContainer->constBegin() + middle)->mainKey()) {
            y = (tmpContainer->constBegin() + middle)->mainValue();
            break;
        }
        if (x > (tmpContainer->constBegin() + middle)->mainKey()) {
            low = middle;
        } else if (x < (tmpContainer->constBegin() + middle)->mainKey()) {
            high = middle;
        }
        if (high - low <= 1) { //差值计算所在位置数据
            y = (tmpContainer->constBegin() + low)->mainValue() + ((x - (tmpContainer->constBegin() + low)->mainKey()) *
                                                                   ((tmpContainer->constBegin() + high)->mainValue() - (tmpContainer->constBegin() + low)->mainValue())) /
                                                                      ((tmpContainer->constBegin() + high)->mainKey() - (tmpContainer->constBegin() + low)->mainKey());
            break;
        }
    }

    m_TraserY->updatePosition(x, y);
    m_TraserY->setText(QString::number(y, 'f', 2));

    m_TraserD->updatePosition(x, y);
    m_TraserD->setText(QString("（%1，%2）").arg(QString::number(x, 'f', 2)).arg(QString::number(y, 'f', 2)));
    customPlot->replot(QCustomPlot::rpQueuedReplot);
}
