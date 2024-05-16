#ifndef SOLUTION_H
#define SOLUTION_H

#include "NumCpp.hpp"

namespace constants
{
    constexpr double simulation_time = 60 * 60 * 2;
    constexpr double R = 6'371'300;
    constexpr double g = 9.81;

    // Simulation interval
    constexpr int T = 1;

    // Drift speed initial value (low accuracy)
    constexpr double betta = 0.1;

    // White noise parameters
    constexpr double mu = 0.;
    constexpr double sigma = 1.;
}

class Solution
{
public:
    Solution();

    const nc::NdArray<double> A = { {0, -constants::g, 0}, {1 / constants::R, 0, 1}, {0, 0, 0} };

    // Transition matrix
    const nc::NdArray<double> F = static_cast<double>(constants::T) * A + nc::eye<double>(nc::Shape{ 3, 3 });
    const nc::NdArray<double> H = { 1, 0, 0 }; // Observation matrix

    // State vector
    nc::NdArray<double> x = nc::transpose(nc::NdArray<double>({ 0, 0, constants::betta * nc::constants::pi / 180 / 3600 }));
    nc::NdArray<double> z;

    nc::NdArray<double> t = nc::linspace(0., constants::simulation_time, constants::simulation_time + 1);
    const nc::uint32 n = std::size(t);

    std::array<double, 4> Q_optimal;

    nc::NdArray<double> x_err = nc::transpose(nc::NdArray<double>({ 0, 0, 0 }));

    nc::NdArray<double> x_err_pmin = nc::transpose(nc::NdArray<double>({ 0, 0, 0 }));
    nc::NdArray<double> x_err_pmax = nc::transpose(nc::NdArray<double>({ 0, 0, 0 }));

    nc::NdArray<double> x_err_rmin = nc::transpose(nc::NdArray<double>({ 0, 0, 0 }));
    nc::NdArray<double> x_err_rmax = nc::transpose(nc::NdArray<double>({ 0, 0, 0 }));

    nc::NdArray<double> x_err_qmin = nc::transpose(nc::NdArray<double>({ 0, 0, 0 }));
    nc::NdArray<double> x_err_qmax = nc::transpose(nc::NdArray<double>({ 0, 0, 0 }));

    nc::NdArray<double> x_err_long = nc::transpose(nc::NdArray<double>({ 0, 0, 0 }));
    nc::NdArray<double> x_err_short = nc::transpose(nc::NdArray<double>({ 0, 0, 0 }));

private:
    nc::NdArray<double> w;
    void generateWhiteNoise(nc::uint32 n, double mu = 0., double sigma = 1.);

    void getSpeedErrors();
    void getSpeedMeasurements();

    void estimateOptimalQ();

    void setupKalmanFilter();
    void setupKalmanFilterP();
    void setupKalmanFilterR();
    void setupKalmanFilterQ();
    void setupKalmanFilterLong();
    void setupKalmanFilterShort();
};

#endif // SOLUTION_H
