#ifndef SAVEPDFWINDOW_H
#define SAVEPDFWINDOW_H

#include "qcpdocumentobject.h"
#include <QMainWindow>
#pragma execution_character_set("utf-8")
namespace Ui {
class savePDFwindow;
}

class savePDFwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit savePDFwindow(QWidget* parent = 0);
    ~savePDFwindow();
    void setupPlot();

private slots:
    void on_actionInsert_Plot_triggered();
    void on_actionSave_Document_triggered();

private:
    Ui::savePDFwindow* ui;
};

#endif
