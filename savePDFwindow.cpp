﻿#include "savePDFwindow.h"
#include "ui_savepdfwindow.h"
#include <QFileDialog>
#include <QTextDocument>
#pragma execution_character_set("utf-8")
savePDFwindow::savePDFwindow(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::savePDFwindow)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowStaysOnTopHint);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(255, 255, 255));
    setAutoFillBackground(true);
    setPalette(palette);

    connect(ui->cbUseCurrentSize, SIGNAL(toggled(bool)), ui->sbWidth, SLOT(setDisabled(bool)));
    connect(ui->cbUseCurrentSize, SIGNAL(toggled(bool)), ui->sbHeight, SLOT(setDisabled(bool)));

    ui->plot->axisRect()->setMinimumSize(300, 180);
    setupPlot();

    // 注册plot document对象
    QCPDocumentObject* plotObjectHandler = new QCPDocumentObject(this);
    ui->textEdit->document()->documentLayout()->registerHandler(QCPDocumentObject::PlotTextFormat, plotObjectHandler);
}

savePDFwindow::~savePDFwindow()
{
    delete ui;
}

void
savePDFwindow::setupPlot()
{
    // The following plot setup is taken from the sine demo:
    // add two new graphs and set their look:
    ui->plot->addGraph();
    ui->plot->graph(0)->setPen(QPen(Qt::blue));                  // line color blue for first graph
    ui->plot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue
    ui->plot->addGraph();
    ui->plot->graph(1)->setPen(QPen(Qt::red)); // line color red for second graph
    // generate some points of data (y0 for first, y1 for second graph):
    QVector<double> x(250), y0(250), y1(250);
    for (int i = 0; i < 250; ++i) {
        x[i] = i;
        y0[i] = qExp(-i / 150.0) * qCos(i / 10.0); // exponentially decaying cosine
        y1[i] = qExp(-i / 150.0);                  // exponential envelope
    }
    // configure right and top axis to show ticks but no labels:
    // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
    ui->plot->xAxis2->setVisible(true);
    ui->plot->xAxis2->setTickLabels(false);
    ui->plot->yAxis2->setVisible(true);
    ui->plot->yAxis2->setTickLabels(false);
    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->plot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->plot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->plot->yAxis2, SLOT(setRange(QCPRange)));
    // pass data points to graphs:
    ui->plot->graph(0)->setData(x, y0);
    ui->plot->graph(1)->setData(x, y1);
    // let the ranges scale themselves so graph 0 fits perfectly in the visible area:
    ui->plot->graph(0)->rescaleAxes();
    // same thing for graph 1, but only enlarge ranges (in case graph 1 is smaller than graph 0):
    ui->plot->graph(1)->rescaleAxes(true);
    // Note: we could have also just called customPlot->rescaleAxes(); instead
    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

void
savePDFwindow::on_actionInsert_Plot_triggered()
{
    QTextCursor cursor = ui->textEdit->textCursor();

    // insert the current plot at the cursor position. QCPDocumentObject::generatePlotFormat creates a
    // vectorized snapshot of the passed plot (with the specified width and height) which gets inserted
    // into the text document.
    double width = ui->cbUseCurrentSize->isChecked() ? 0 : ui->sbWidth->value();
    double height = ui->cbUseCurrentSize->isChecked() ? 0 : ui->sbHeight->value();
    cursor.insertText(QString(QChar::ObjectReplacementCharacter), QCPDocumentObject::generatePlotFormat(ui->plot, width, height));

    ui->textEdit->setTextCursor(cursor);
}

void
savePDFwindow::on_actionSave_Document_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save document...", qApp->applicationDirPath(), "*.pdf");
    if (!fileName.isEmpty()) {
        QPrinter printer;
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        QMargins pageMargins(20, 20, 20, 20);
        QPageLayout pageLayout;
        pageLayout.setMode(QPageLayout::StandardMode);
        pageLayout.setOrientation(QPageLayout::Portrait);
        pageLayout.setPageSize(QPageSize(QPageSize::A4));
        pageLayout.setUnits(QPageLayout::Millimeter);
        pageLayout.setMargins(QMarginsF(pageMargins));
        printer.setPageLayout(pageLayout);
        ui->textEdit->document()->setPageSize(printer.pageRect().size());
        ui->textEdit->document()->print(&printer);
    }
}
