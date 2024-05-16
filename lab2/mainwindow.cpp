#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->initializeValues();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializeValues()
{
    x = solution->x;
    z = solution->z;
    t = solution->t;
    x_err = solution->x_err;
    x_err_pmin = solution->x_err_pmin;
    x_err_pmax = solution->x_err_pmax;
    x_err_rmin = solution->x_err_rmin;
    x_err_rmax = solution->x_err_rmax;
    x_err_qmin = solution->x_err_qmin;
    x_err_qmax = solution->x_err_qmax;
    x_err_long = solution->x_err_long;
    x_err_short = solution->x_err_short;
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    ui->customPlot->clearPlottables();
    ui->customPlot_2->clearPlottables();
    ui->customPlot_3->clearPlottables();

    switch (index)
    {
    // 1. INS errors simulation
    case 0:
        plotSpeedAndMeasurementsErrors();
        plotDeflectionAngleError();
        plotDriftSpeedError();
        break;

    // 2. Kalman Filter (default)
    case 1:
        plotSpeedAndMeasurementsFK();
        plotDeflectionAngleFK();
        plotDriftSpeedFK();
        break;

    // 3. Kalman Filter (P - min/max)
    case 2:
        plotSpeedAndMeasurementsP();
        plotDeflectionAngleP();
        plotDriftSpeedP();
        break;

    // 4. Kalman Filter (R - min/max)
    case 3:
        plotSpeedAndMeasurementsR();
        plotDeflectionAngleR();
        plotDriftSpeedR();
        break;

    // 5. Kalman Filter (Q - min/max)
    case 4:
        plotSpeedAndMeasurementsQ();
        plotDeflectionAngleQ();
        plotDriftSpeedQ();
        break;

    // 6. Kalman Filter (Long estimation)
    case 5:
        plotSpeedAndMeasurementsLong();
        plotDeflectionAngleLong();
        plotDriftSpeedLong();
        break;

    // 7. Kalman Filter (Short estimation)
    case 6:
        plotSpeedAndMeasurementsShort();
        plotDeflectionAngleShort();
        plotDriftSpeedShort();
        break;
    }
}

void MainWindow::plotSpeedAndMeasurementsErrors()
{
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot->graph(0)->setData(t.toQtVector(), (x(0, x.cSlice()) * 3.6).toQtVector());

    ui->customPlot->addGraph();
    ui->customPlot->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot->graph(1)->setData(t.toQtVector(), (z * 3.6).toQtVector());

    ui->customPlot->legend->setVisible(true);
    ui->customPlot->graph(0)->setName("Speed error");
    ui->customPlot->graph(1)->setName("Speed measurements error");

    ui->customPlot->graph(0)->rescaleAxes();
    ui->customPlot->graph(1)->rescaleAxes(true);

    ui->customPlot->xAxis->setLabel("t, c");
    ui->customPlot->yAxis->setLabel("δV, km/h");

    ui->customPlot->plotLayout()->insertRow(0);
    ui->customPlot->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot, "Speed & Speed measurements errors", QFont("Arial", 12, QFont::Bold)));

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot->replot();

    if (ui->customPlot->plotLayout()->rowCount() > 0) {
        ui->customPlot->plotLayout()->removeAt(0);
        ui->customPlot->plotLayout()->simplify();
    }
}

void MainWindow::plotDeflectionAngleError()
{
    ui->customPlot_2->addGraph();
    ui->customPlot_2->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot_2->graph(0)->setData(t.toQtVector(), (x(1, x.cSlice()) * 180. / nc::constants::pi).toQtVector());

    ui->customPlot_2->graph(0)->rescaleAxes();

    ui->customPlot_2->xAxis->setLabel("t, c");
    ui->customPlot_2->yAxis->setLabel("Φ, deg");

    ui->customPlot_2->plotLayout()->insertRow(0);
    ui->customPlot_2->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot_2, "Angle error", QFont("Arial", 12, QFont::Bold)));

    ui->customPlot_2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot_2->replot();

    if (ui->customPlot_2->plotLayout()->rowCount() > 0) {
        ui->customPlot_2->plotLayout()->removeAt(0);
        ui->customPlot_2->plotLayout()->simplify();
    }
}

void MainWindow::plotDriftSpeedError()
{
    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot_3->graph(0)->setData(t.toQtVector(), (x(2, x.cSlice()) * 180. / nc::constants::pi * 3600.).toQtVector());

    ui->customPlot_3->graph(0)->rescaleAxes();

    ui->customPlot_3->xAxis->setLabel("t, c");
    ui->customPlot_3->yAxis->setLabel("ω dr, deg/h");

    ui->customPlot_3->plotLayout()->insertRow(0);
    ui->customPlot_3->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot_3, "Drift speed error", QFont("Arial", 12, QFont::Bold)));

    ui->customPlot_3->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot_3->replot();

    if (ui->customPlot_3->plotLayout()->rowCount() > 0) {
        ui->customPlot_3->plotLayout()->removeAt(0);
        ui->customPlot_3->plotLayout()->simplify();
    }
}

void MainWindow::plotSpeedAndMeasurementsFK()
{
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot->graph(0)->setData(t.toQtVector(), (x(0, x.cSlice()) * 3.6).toQtVector());

    ui->customPlot->addGraph();
    ui->customPlot->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot->graph(1)->setData(t.toQtVector(), (x_err(0, x_err.cSlice()) * 3.6).toQtVector());

    ui->customPlot->legend->setVisible(true);
    ui->customPlot->graph(0)->setName("Speed error");
    ui->customPlot->graph(1)->setName("Speed error estimation");

    ui->customPlot->graph(0)->rescaleAxes();
    ui->customPlot->graph(1)->rescaleAxes(true);

    ui->customPlot->xAxis->setLabel("t, c");
    ui->customPlot->yAxis->setLabel("δV, km/h");

    ui->customPlot->plotLayout()->insertRow(0);
    ui->customPlot->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot, "Speed measurement error", QFont("Arial", 12, QFont::Bold)));

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot->replot();

    if (ui->customPlot->plotLayout()->rowCount() > 0) {
        ui->customPlot->plotLayout()->removeAt(0);
        ui->customPlot->plotLayout()->simplify();
    }
}

void MainWindow::plotDeflectionAngleFK()
{
    ui->customPlot_2->addGraph();
    ui->customPlot_2->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot_2->graph(0)->setData(t.toQtVector(), (x(1, x.cSlice()) * 180. / nc::constants::pi).toQtVector());

    ui->customPlot_2->addGraph();
    ui->customPlot_2->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot_2->graph(1)->setData(t.toQtVector(), (x_err(1, x_err.cSlice()) * 180. / nc::constants::pi).toQtVector());

    ui->customPlot_2->legend->setVisible(true);
    ui->customPlot_2->graph(0)->setName("Angle error");
    ui->customPlot_2->graph(1)->setName("Angle error estimation");

    ui->customPlot_2->graph(0)->rescaleAxes();
    ui->customPlot_2->graph(1)->rescaleAxes(true);

    ui->customPlot_2->xAxis->setLabel("t, c");
    ui->customPlot_2->yAxis->setLabel("Φ, deg");

    ui->customPlot_2->plotLayout()->insertRow(0);
    ui->customPlot_2->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot_2, "Angle error", QFont("Arial", 12, QFont::Bold)));

    ui->customPlot_2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot_2->replot();

    if (ui->customPlot_2->plotLayout()->rowCount() > 0) {
        ui->customPlot_2->plotLayout()->removeAt(0);
        ui->customPlot_2->plotLayout()->simplify();
    }
}

void MainWindow::plotDriftSpeedFK()
{
    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot_3->graph(0)->setData(t.toQtVector(), (x(2, x.cSlice()) * 180. / nc::constants::pi * 3600.).toQtVector());

    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot_3->graph(1)->setData(t.toQtVector(), (x_err(2, x_err.cSlice()) * 180. / nc::constants::pi * 3600.).toQtVector());

    ui->customPlot_3->legend->setVisible(true);
    ui->customPlot_3->graph(0)->setName("Drift speed error");
    ui->customPlot_3->graph(1)->setName("Drift speed error estimation");

    ui->customPlot_3->graph(0)->rescaleAxes();
    ui->customPlot_3->graph(1)->rescaleAxes(true);

    ui->customPlot_3->xAxis->setLabel("t, c");
    ui->customPlot_3->yAxis->setLabel("ω dr, deg/h");

    ui->customPlot_3->plotLayout()->insertRow(0);
    ui->customPlot_3->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot_3, "Drift speed error", QFont("Arial", 12, QFont::Bold)));

    ui->customPlot_3->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot_3->replot();

    if (ui->customPlot_3->plotLayout()->rowCount() > 0) {
        ui->customPlot_3->plotLayout()->removeAt(0);
        ui->customPlot_3->plotLayout()->simplify();
    }
}

void MainWindow::plotSpeedAndMeasurementsP()
{
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot->graph(0)->setData(t.toQtVector(), (x(0, x.cSlice()) * 3.6).toQtVector());

    ui->customPlot->addGraph();
    ui->customPlot->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot->graph(1)->setData(t.toQtVector(), (x_err(0, x_err.cSlice()) * 3.6).toQtVector());

    ui->customPlot->addGraph();
    ui->customPlot->graph(2)->setPen(QPen(Qt::green));
    ui->customPlot->graph(2)->setData(t.toQtVector(), (x_err_pmin(0, x_err_pmin.cSlice()) * 3.6).toQtVector());

    ui->customPlot->addGraph();
    ui->customPlot->graph(3)->setPen(QPen(Qt::black));
    ui->customPlot->graph(3)->setData(t.toQtVector(), (x_err_pmax(0, x_err_pmax.cSlice()) * 3.6).toQtVector());

    ui->customPlot->legend->setVisible(true);
    ui->customPlot->graph(0)->setName("Speed error");
    ui->customPlot->graph(1)->setName("Speed error estimation");
    ui->customPlot->graph(2)->setName("Speed error estimation with Pmin");
    ui->customPlot->graph(3)->setName("Speed error estimation with Pmax");

    ui->customPlot->graph(0)->rescaleAxes();
    ui->customPlot->graph(1)->rescaleAxes(true);
    ui->customPlot->graph(2)->rescaleAxes(true);
    ui->customPlot->graph(3)->rescaleAxes(true);

    ui->customPlot->xAxis->setLabel("t, c");
    ui->customPlot->yAxis->setLabel("δV, km/h");

    ui->customPlot->plotLayout()->insertRow(0);
    ui->customPlot->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot, "Speed measurement error with various P", QFont("Arial", 12, QFont::Bold)));

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot->replot();

    if (ui->customPlot->plotLayout()->rowCount() > 0) {
        ui->customPlot->plotLayout()->removeAt(0);
        ui->customPlot->plotLayout()->simplify();
    }
}

void MainWindow::plotDeflectionAngleP()
{
    ui->customPlot_2->addGraph();
    ui->customPlot_2->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot_2->graph(0)->setData(t.toQtVector(), (x(1, x.cSlice()) * 180. / nc::constants::pi).toQtVector());

    ui->customPlot_2->addGraph();
    ui->customPlot_2->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot_2->graph(1)->setData(t.toQtVector(), (x_err(1, x_err.cSlice()) * 180. / nc::constants::pi).toQtVector());

    ui->customPlot_2->addGraph();
    ui->customPlot_2->graph(2)->setPen(QPen(Qt::green));
    ui->customPlot_2->graph(2)->setData(t.toQtVector(), (x_err_pmin(1, x_err_pmin.cSlice()) * 180. / nc::constants::pi).toQtVector());

    ui->customPlot_2->addGraph();
    ui->customPlot_2->graph(3)->setPen(QPen(Qt::black));
    ui->customPlot_2->graph(3)->setData(t.toQtVector(), (x_err_pmax(1, x_err_pmax.cSlice()) * 180. / nc::constants::pi).toQtVector());

    ui->customPlot_2->legend->setVisible(true);
    ui->customPlot_2->graph(0)->setName("Angle error");
    ui->customPlot_2->graph(1)->setName("Angle error estimation");
    ui->customPlot_2->graph(2)->setName("Angle error estimation with Pmin");
    ui->customPlot_2->graph(3)->setName("Angle error estimation with Pmax");

    ui->customPlot_2->graph(0)->rescaleAxes();
    ui->customPlot_2->graph(1)->rescaleAxes(true);
    ui->customPlot_2->graph(2)->rescaleAxes(true);
    ui->customPlot_2->graph(3)->rescaleAxes(true);

    ui->customPlot_2->xAxis->setLabel("t, c");
    ui->customPlot_2->yAxis->setLabel("Φ, deg");

    ui->customPlot_2->plotLayout()->insertRow(0);
    ui->customPlot_2->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot_2, "Angle error with various P", QFont("Arial", 12, QFont::Bold)));

    ui->customPlot_2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot_2->replot();

    if (ui->customPlot_2->plotLayout()->rowCount() > 0) {
        ui->customPlot_2->plotLayout()->removeAt(0);
        ui->customPlot_2->plotLayout()->simplify();
    }
}

void MainWindow::plotDriftSpeedP()
{
    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot_3->graph(0)->setData(t.toQtVector(), (x(2, x.cSlice()) * 180. / nc::constants::pi * 3600.).toQtVector());

    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot_3->graph(1)->setData(t.toQtVector(), (x_err(2, x_err.cSlice()) * 180. / nc::constants::pi * 3600.).toQtVector());

    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph(2)->setPen(QPen(Qt::green));
    ui->customPlot_3->graph(2)->setData(t.toQtVector(), (x_err_pmin(2, x_err_pmin.cSlice()) * 180. / nc::constants::pi * 3600.).toQtVector());

    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph(3)->setPen(QPen(Qt::black));
    ui->customPlot_3->graph(3)->setData(t.toQtVector(), (x_err_pmax(2, x_err_pmax.cSlice()) * 180. / nc::constants::pi * 3600.).toQtVector());

    ui->customPlot_3->legend->setVisible(true);
    ui->customPlot_3->graph(0)->setName("Drift speed error");
    ui->customPlot_3->graph(1)->setName("Drift speed error estimation");
    ui->customPlot_3->graph(2)->setName("Drift speed error estimation with Pmin");
    ui->customPlot_3->graph(3)->setName("Drift speed error estimation with Pmax");

    ui->customPlot_3->graph(0)->rescaleAxes();
    ui->customPlot_3->graph(1)->rescaleAxes(true);
    ui->customPlot_3->graph(2)->rescaleAxes(true);
    ui->customPlot_3->graph(3)->rescaleAxes(true);

    ui->customPlot_3->xAxis->setLabel("t, c");
    ui->customPlot_3->yAxis->setLabel("ω dr, deg/h");

    ui->customPlot_3->plotLayout()->insertRow(0);
    ui->customPlot_3->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot_3, "Drift speed error with various P", QFont("Arial", 12, QFont::Bold)));

    ui->customPlot_3->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot_3->replot();

    if (ui->customPlot_3->plotLayout()->rowCount() > 0) {
        ui->customPlot_3->plotLayout()->removeAt(0);
        ui->customPlot_3->plotLayout()->simplify();
    }
}

void MainWindow::plotSpeedAndMeasurementsR()
{
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot->graph(0)->setData(t.toQtVector(), (x(0, x.cSlice()) * 3.6).toQtVector());

    ui->customPlot->addGraph();
    ui->customPlot->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot->graph(1)->setData(t.toQtVector(), (x_err(0, x_err.cSlice()) * 3.6).toQtVector());

    ui->customPlot->addGraph();
    ui->customPlot->graph(2)->setPen(QPen(Qt::green));
    ui->customPlot->graph(2)->setData(t.toQtVector(), (x_err_rmin(0, x_err_rmin.cSlice()) * 3.6).toQtVector());

    ui->customPlot->addGraph();
    ui->customPlot->graph(3)->setPen(QPen(Qt::black));
    ui->customPlot->graph(3)->setData(t.toQtVector(), (x_err_rmax(0, x_err_rmax.cSlice()) * 3.6).toQtVector());

    ui->customPlot->legend->setVisible(true);
    ui->customPlot->graph(0)->setName("Speed error");
    ui->customPlot->graph(1)->setName("Speed error estimation");
    ui->customPlot->graph(2)->setName("Speed error estimation with Rmin");
    ui->customPlot->graph(3)->setName("Speed error estimation with Rmax");

    ui->customPlot->graph(0)->rescaleAxes();
    ui->customPlot->graph(1)->rescaleAxes(true);
    ui->customPlot->graph(2)->rescaleAxes(true);
    ui->customPlot->graph(3)->rescaleAxes(true);

    ui->customPlot->xAxis->setLabel("t, c");
    ui->customPlot->yAxis->setLabel("δV, km/h");

    ui->customPlot->plotLayout()->insertRow(0);
    ui->customPlot->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot, "Speed measurement error with various R", QFont("Arial", 12, QFont::Bold)));

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot->replot();

    if (ui->customPlot->plotLayout()->rowCount() > 0) {
        ui->customPlot->plotLayout()->removeAt(0);
        ui->customPlot->plotLayout()->simplify();
    }
}

void MainWindow::plotDeflectionAngleR()
{
    ui->customPlot_2->addGraph();
    ui->customPlot_2->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot_2->graph(0)->setData(t.toQtVector(), (x(1, x.cSlice()) * 180. / nc::constants::pi).toQtVector());

    ui->customPlot_2->addGraph();
    ui->customPlot_2->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot_2->graph(1)->setData(t.toQtVector(), (x_err(1, x_err.cSlice()) * 180. / nc::constants::pi).toQtVector());

    ui->customPlot_2->addGraph();
    ui->customPlot_2->graph(2)->setPen(QPen(Qt::green));
    ui->customPlot_2->graph(2)->setData(t.toQtVector(), (x_err_rmin(1, x_err_rmin.cSlice()) * 180. / nc::constants::pi).toQtVector());

    ui->customPlot_2->addGraph();
    ui->customPlot_2->graph(3)->setPen(QPen(Qt::black));
    ui->customPlot_2->graph(3)->setData(t.toQtVector(), (x_err_rmax(1, x_err_rmax.cSlice()) * 180. / nc::constants::pi).toQtVector());

    ui->customPlot_2->legend->setVisible(true);
    ui->customPlot_2->graph(0)->setName("Angle error");
    ui->customPlot_2->graph(1)->setName("Angle error estimation");
    ui->customPlot_2->graph(2)->setName("Angle error estimation with Rmin");
    ui->customPlot_2->graph(3)->setName("Angle error estimation with Rmax");

    ui->customPlot_2->graph(0)->rescaleAxes();
    ui->customPlot_2->graph(1)->rescaleAxes(true);
    ui->customPlot_2->graph(2)->rescaleAxes(true);
    ui->customPlot_2->graph(3)->rescaleAxes(true);

    ui->customPlot_2->xAxis->setLabel("t, c");
    ui->customPlot_2->yAxis->setLabel("Φ, deg");

    ui->customPlot_2->plotLayout()->insertRow(0);
    ui->customPlot_2->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot_2, "Angle error with various R", QFont("Arial", 12, QFont::Bold)));

    ui->customPlot_2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot_2->replot();

    if (ui->customPlot_2->plotLayout()->rowCount() > 0) {
        ui->customPlot_2->plotLayout()->removeAt(0);
        ui->customPlot_2->plotLayout()->simplify();
    }
}

void MainWindow::plotDriftSpeedR()
{
    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot_3->graph(0)->setData(t.toQtVector(), (x(2, x.cSlice()) * 180. / nc::constants::pi * 3600.).toQtVector());

    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot_3->graph(1)->setData(t.toQtVector(), (x_err(2, x_err.cSlice()) * 180. / nc::constants::pi * 3600.).toQtVector());

    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph(2)->setPen(QPen(Qt::green));
    ui->customPlot_3->graph(2)->setData(t.toQtVector(), (x_err_rmin(2, x_err_rmin.cSlice()) * 180. / nc::constants::pi * 3600.).toQtVector());

    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph(3)->setPen(QPen(Qt::black));
    ui->customPlot_3->graph(3)->setData(t.toQtVector(), (x_err_rmax(2, x_err_rmax.cSlice()) * 180. / nc::constants::pi * 3600.).toQtVector());

    ui->customPlot_3->legend->setVisible(true);
    ui->customPlot_3->graph(0)->setName("Drift speed error");
    ui->customPlot_3->graph(1)->setName("Drift speed error estimation");
    ui->customPlot_3->graph(2)->setName("Drift speed error estimation with Rmin");
    ui->customPlot_3->graph(3)->setName("Drift speed error estimation with Rmax");

    ui->customPlot_3->graph(0)->rescaleAxes();
    ui->customPlot_3->graph(1)->rescaleAxes(true);
    ui->customPlot_3->graph(2)->rescaleAxes(true);
    ui->customPlot_3->graph(3)->rescaleAxes(true);

    ui->customPlot_3->xAxis->setLabel("t, c");
    ui->customPlot_3->yAxis->setLabel("ω dr, deg/h");

    ui->customPlot_3->plotLayout()->insertRow(0);
    ui->customPlot_3->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot_3, "Drift speed error with various R", QFont("Arial", 12, QFont::Bold)));

    ui->customPlot_3->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot_3->replot();

    if (ui->customPlot_3->plotLayout()->rowCount() > 0) {
        ui->customPlot_3->plotLayout()->removeAt(0);
        ui->customPlot_3->plotLayout()->simplify();
    }
}

void MainWindow::plotSpeedAndMeasurementsQ()
{
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot->graph(0)->setData(t.toQtVector(), (x(0, x.cSlice()) * 3.6).toQtVector());

    ui->customPlot->addGraph();
    ui->customPlot->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot->graph(1)->setData(t.toQtVector(), (x_err(0, x_err.cSlice()) * 3.6).toQtVector());

    ui->customPlot->addGraph();
    ui->customPlot->graph(2)->setPen(QPen(Qt::green));
    ui->customPlot->graph(2)->setData(t.toQtVector(), (x_err_qmin(0, x_err_qmin.cSlice()) * 3.6).toQtVector());

    ui->customPlot->addGraph();
    ui->customPlot->graph(3)->setPen(QPen(Qt::black));
    ui->customPlot->graph(3)->setData(t.toQtVector(), (x_err_qmax(0, x_err_qmax.cSlice()) * 3.6).toQtVector());

    ui->customPlot->legend->setVisible(true);
    ui->customPlot->graph(0)->setName("Speed error");
    ui->customPlot->graph(1)->setName("Speed error estimation");
    ui->customPlot->graph(2)->setName("Speed error estimation with Qmin");
    ui->customPlot->graph(3)->setName("Speed error estimation with Qmax");

    ui->customPlot->graph(0)->rescaleAxes();
    ui->customPlot->graph(1)->rescaleAxes(true);
    ui->customPlot->graph(2)->rescaleAxes(true);
    ui->customPlot->graph(3)->rescaleAxes(true);

    ui->customPlot->xAxis->setLabel("t, c");
    ui->customPlot->yAxis->setLabel("δV, km/h");

    ui->customPlot->plotLayout()->insertRow(0);
    ui->customPlot->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot, "Speed measurement error with various Q", QFont("Arial", 12, QFont::Bold)));

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot->replot();

    if (ui->customPlot->plotLayout()->rowCount() > 0) {
        ui->customPlot->plotLayout()->removeAt(0);
        ui->customPlot->plotLayout()->simplify();
    }
}

void MainWindow::plotDeflectionAngleQ()
{
    ui->customPlot_2->addGraph();
    ui->customPlot_2->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot_2->graph(0)->setData(t.toQtVector(), (x(1, x.cSlice()) * 180. / nc::constants::pi).toQtVector());

    ui->customPlot_2->addGraph();
    ui->customPlot_2->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot_2->graph(1)->setData(t.toQtVector(), (x_err(1, x_err.cSlice()) * 180. / nc::constants::pi).toQtVector());

    ui->customPlot_2->addGraph();
    ui->customPlot_2->graph(2)->setPen(QPen(Qt::green));
    ui->customPlot_2->graph(2)->setData(t.toQtVector(), (x_err_qmin(1, x_err_qmin.cSlice()) * 180. / nc::constants::pi).toQtVector());

    ui->customPlot_2->addGraph();
    ui->customPlot_2->graph(3)->setPen(QPen(Qt::black));
    ui->customPlot_2->graph(3)->setData(t.toQtVector(), (x_err_qmax(1, x_err_qmax.cSlice()) * 180. / nc::constants::pi).toQtVector());

    ui->customPlot_2->legend->setVisible(true);
    ui->customPlot_2->graph(0)->setName("Angle error");
    ui->customPlot_2->graph(1)->setName("Angle error estimation");
    ui->customPlot_2->graph(2)->setName("Angle error estimation with Qmin");
    ui->customPlot_2->graph(3)->setName("Angle error estimation with Qmax");

    ui->customPlot_2->graph(0)->rescaleAxes();
    ui->customPlot_2->graph(1)->rescaleAxes(true);
    ui->customPlot_2->graph(2)->rescaleAxes(true);
    ui->customPlot_2->graph(3)->rescaleAxes(true);

    ui->customPlot_2->xAxis->setLabel("t, c");
    ui->customPlot_2->yAxis->setLabel("Φ, deg");

    ui->customPlot_2->plotLayout()->insertRow(0);
    ui->customPlot_2->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot_2, "Angle error with various Q", QFont("Arial", 12, QFont::Bold)));

    ui->customPlot_2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot_2->replot();

    if (ui->customPlot_2->plotLayout()->rowCount() > 0) {
        ui->customPlot_2->plotLayout()->removeAt(0);
        ui->customPlot_2->plotLayout()->simplify();
    }
}

void MainWindow::plotDriftSpeedQ()
{
    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot_3->graph(0)->setData(t.toQtVector(), (x(2, x.cSlice()) * 180. / nc::constants::pi * 3600.).toQtVector());

    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot_3->graph(1)->setData(t.toQtVector(), (x_err(2, x_err.cSlice()) * 180. / nc::constants::pi * 3600.).toQtVector());

    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph(2)->setPen(QPen(Qt::green));
    ui->customPlot_3->graph(2)->setData(t.toQtVector(), (x_err_qmin(2, x_err_qmin.cSlice()) * 180. / nc::constants::pi * 3600.).toQtVector());

    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph(3)->setPen(QPen(Qt::black));
    ui->customPlot_3->graph(3)->setData(t.toQtVector(), (x_err_qmax(2, x_err_qmax.cSlice()) * 180. / nc::constants::pi * 3600.).toQtVector());

    ui->customPlot_3->legend->setVisible(true);
    ui->customPlot_3->graph(0)->setName("Drift speed error");
    ui->customPlot_3->graph(1)->setName("Drift speed error estimation");
    ui->customPlot_3->graph(2)->setName("Drift speed error estimation with Qmin");
    ui->customPlot_3->graph(3)->setName("Drift speed error estimation with Qmax");

    ui->customPlot_3->graph(0)->rescaleAxes();
    ui->customPlot_3->graph(1)->rescaleAxes(true);
    ui->customPlot_3->graph(2)->rescaleAxes(true);
    ui->customPlot_3->graph(3)->rescaleAxes(true);

    ui->customPlot_3->xAxis->setLabel("t, c");
    ui->customPlot_3->yAxis->setLabel("ω dr, deg/h");

    ui->customPlot_3->plotLayout()->insertRow(0);
    ui->customPlot_3->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot_3, "Drift speed error with various Q", QFont("Arial", 12, QFont::Bold)));

    ui->customPlot_3->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot_3->replot();

    if (ui->customPlot_3->plotLayout()->rowCount() > 0) {
        ui->customPlot_3->plotLayout()->removeAt(0);
        ui->customPlot_3->plotLayout()->simplify();
    }
}

void MainWindow::plotSpeedAndMeasurementsLong()
{
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot->graph(0)->setData(t.toQtVector(), (x(0, x.cSlice()) * 3.6).toQtVector());

    ui->customPlot->addGraph();
    ui->customPlot->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot->graph(1)->setData(t.toQtVector(), (x_err_long(0, x_err_long.cSlice()) * 3.6).toQtVector());

    ui->customPlot->legend->setVisible(true);
    ui->customPlot->graph(0)->setName("Speed error");
    ui->customPlot->graph(1)->setName("Speed error estimation");

    ui->customPlot->graph(0)->rescaleAxes();
    ui->customPlot->graph(1)->rescaleAxes(true);

    ui->customPlot->xAxis->setLabel("t, c");
    ui->customPlot->yAxis->setLabel("δV, km/h");

    ui->customPlot->plotLayout()->insertRow(0);
    ui->customPlot->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot, "Speed measurement error with long estimation", QFont("Arial", 12, QFont::Bold)));

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot->replot();

    if (ui->customPlot->plotLayout()->rowCount() > 0) {
        ui->customPlot->plotLayout()->removeAt(0);
        ui->customPlot->plotLayout()->simplify();
    }
}

void MainWindow::plotDeflectionAngleLong()
{
    ui->customPlot_2->addGraph();
    ui->customPlot_2->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot_2->graph(0)->setData(t.toQtVector(), (x(1, x.cSlice()) * 180. / nc::constants::pi).toQtVector());

    ui->customPlot_2->addGraph();
    ui->customPlot_2->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot_2->graph(1)->setData(t.toQtVector(), (x_err_long(1, x_err_long.cSlice()) * 180. / nc::constants::pi).toQtVector());

    ui->customPlot_2->legend->setVisible(true);
    ui->customPlot_2->graph(0)->setName("Angle error");
    ui->customPlot_2->graph(1)->setName("Angle error estimation");

    ui->customPlot_2->graph(0)->rescaleAxes();
    ui->customPlot_2->graph(1)->rescaleAxes(true);

    ui->customPlot_2->xAxis->setLabel("t, c");
    ui->customPlot_2->yAxis->setLabel("Φ, deg");

    ui->customPlot_2->plotLayout()->insertRow(0);
    ui->customPlot_2->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot_2, "Angle error with long estimation", QFont("Arial", 12, QFont::Bold)));

    ui->customPlot_2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot_2->replot();

    if (ui->customPlot_2->plotLayout()->rowCount() > 0) {
        ui->customPlot_2->plotLayout()->removeAt(0);
        ui->customPlot_2->plotLayout()->simplify();
    }
}

void MainWindow::plotDriftSpeedLong()
{
    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot_3->graph(0)->setData(t.toQtVector(), (x(2, x.cSlice()) * 180. / nc::constants::pi * 3600.).toQtVector());

    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot_3->graph(1)->setData(t.toQtVector(), (x_err_long(2, x_err_long.cSlice()) * 180. / nc::constants::pi * 3600.).toQtVector());

    ui->customPlot_3->legend->setVisible(true);
    ui->customPlot_3->graph(0)->setName("Drift speed error");
    ui->customPlot_3->graph(1)->setName("Drift speed error estimation");

    ui->customPlot_3->graph(0)->rescaleAxes();
    ui->customPlot_3->graph(1)->rescaleAxes(true);

    ui->customPlot_3->xAxis->setLabel("t, c");
    ui->customPlot_3->yAxis->setLabel("ω dr, deg/h");

    ui->customPlot_3->plotLayout()->insertRow(0);
    ui->customPlot_3->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot_3, "Drift speed error with long estimation", QFont("Arial", 12, QFont::Bold)));

    ui->customPlot_3->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot_3->replot();

    if (ui->customPlot_3->plotLayout()->rowCount() > 0) {
        ui->customPlot_3->plotLayout()->removeAt(0);
        ui->customPlot_3->plotLayout()->simplify();
    }
}

void MainWindow::plotSpeedAndMeasurementsShort()
{
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot->graph(0)->setData(t.toQtVector(), (x(0, x.cSlice()) * 3.6).toQtVector());

    ui->customPlot->addGraph();
    ui->customPlot->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot->graph(1)->setData(t.toQtVector(), (x_err_short(0, x_err_short.cSlice()) * 3.6).toQtVector());

    ui->customPlot->legend->setVisible(true);
    ui->customPlot->graph(0)->setName("Speed error");
    ui->customPlot->graph(1)->setName("Speed error estimation");

    ui->customPlot->graph(0)->rescaleAxes();
    ui->customPlot->graph(1)->rescaleAxes(true);

    ui->customPlot->xAxis->setLabel("t, c");
    ui->customPlot->yAxis->setLabel("δV, km/h");

    ui->customPlot->plotLayout()->insertRow(0);
    ui->customPlot->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot, "Speed measurement error with short estimation", QFont("Arial", 12, QFont::Bold)));

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot->replot();

    if (ui->customPlot->plotLayout()->rowCount() > 0) {
        ui->customPlot->plotLayout()->removeAt(0);
        ui->customPlot->plotLayout()->simplify();
    }
}

void MainWindow::plotDeflectionAngleShort()
{
    ui->customPlot_2->addGraph();
    ui->customPlot_2->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot_2->graph(0)->setData(t.toQtVector(), (x(1, x.cSlice()) * 180. / nc::constants::pi).toQtVector());

    ui->customPlot_2->addGraph();
    ui->customPlot_2->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot_2->graph(1)->setData(t.toQtVector(), (x_err_short(1, x_err_short.cSlice()) * 180. / nc::constants::pi).toQtVector());

    ui->customPlot_2->legend->setVisible(true);
    ui->customPlot_2->graph(0)->setName("Angle error");
    ui->customPlot_2->graph(1)->setName("Angle error estimation");

    ui->customPlot_2->graph(0)->rescaleAxes();
    ui->customPlot_2->graph(1)->rescaleAxes(true);

    ui->customPlot_2->xAxis->setLabel("t, c");
    ui->customPlot_2->yAxis->setLabel("Φ, deg");

    ui->customPlot_2->plotLayout()->insertRow(0);
    ui->customPlot_2->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot_2, "Angle error with short estimation", QFont("Arial", 12, QFont::Bold)));

    ui->customPlot_2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot_2->replot();

    if (ui->customPlot_2->plotLayout()->rowCount() > 0) {
        ui->customPlot_2->plotLayout()->removeAt(0);
        ui->customPlot_2->plotLayout()->simplify();
    }
}

void MainWindow::plotDriftSpeedShort()
{
    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot_3->graph(0)->setData(t.toQtVector(), (x(2, x.cSlice()) * 180. / nc::constants::pi * 3600.).toQtVector());

    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot_3->graph(1)->setData(t.toQtVector(), (x_err_short(2, x_err_short.cSlice()) * 180. / nc::constants::pi * 3600.).toQtVector());

    ui->customPlot_3->legend->setVisible(true);
    ui->customPlot_3->graph(0)->setName("Drift speed error");
    ui->customPlot_3->graph(1)->setName("Drift speed error estimation");

    ui->customPlot_3->graph(0)->rescaleAxes();
    ui->customPlot_3->graph(1)->rescaleAxes(true);

    ui->customPlot_3->xAxis->setLabel("t, c");
    ui->customPlot_3->yAxis->setLabel("ω dr, deg/h");

    ui->customPlot_3->plotLayout()->insertRow(0);
    ui->customPlot_3->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot_3, "Drift speed error with short estimation", QFont("Arial", 12, QFont::Bold)));

    ui->customPlot_3->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot_3->replot();

    if (ui->customPlot_3->plotLayout()->rowCount() > 0) {
        ui->customPlot_3->plotLayout()->removeAt(0);
        ui->customPlot_3->plotLayout()->simplify();
    }
}

//


