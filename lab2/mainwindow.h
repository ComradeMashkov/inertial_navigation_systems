#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "solution.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;

    std::unique_ptr<Solution> solution { new Solution() };

    nc::NdArray<double> x;
    nc::NdArray<double> z;
    nc::NdArray<double> t;

    nc::NdArray<double> x_err;

    nc::NdArray<double> x_err_pmin;
    nc::NdArray<double> x_err_pmax;

    nc::NdArray<double> x_err_rmin;
    nc::NdArray<double> x_err_rmax;

    nc::NdArray<double> x_err_qmin;
    nc::NdArray<double> x_err_qmax;

    nc::NdArray<double> x_err_long;
    nc::NdArray<double> x_err_short;

    void initializeValues();

    void plotSpeedAndMeasurementsErrors();
    void plotDeflectionAngleError();
    void plotDriftSpeedError();

    void plotSpeedAndMeasurementsFK();
    void plotDeflectionAngleFK();
    void plotDriftSpeedFK();

    void plotSpeedAndMeasurementsP();
    void plotDeflectionAngleP();
    void plotDriftSpeedP();

    void plotSpeedAndMeasurementsR();
    void plotDeflectionAngleR();
    void plotDriftSpeedR();

    void plotSpeedAndMeasurementsQ();
    void plotDeflectionAngleQ();
    void plotDriftSpeedQ();

    void plotSpeedAndMeasurementsLong();
    void plotDeflectionAngleLong();
    void plotDriftSpeedLong();

    void plotSpeedAndMeasurementsShort();
    void plotDeflectionAngleShort();
    void plotDriftSpeedShort();
};
#endif // MAINWINDOW_H
