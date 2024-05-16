#include "solution.h"

Solution::Solution()
{
    this->generateWhiteNoise(n, constants::mu, constants::sigma);
    this->getSpeedErrors();
    this->getSpeedMeasurements();
    this->estimateOptimalQ();
    this->setupKalmanFilter();
    this->setupKalmanFilterP();
    this->setupKalmanFilterR();
    this->setupKalmanFilterQ();
    this->setupKalmanFilterLong();
    this->setupKalmanFilterShort();
}

// 1.1. White noise simulation
void Solution::generateWhiteNoise(nc::uint32 n, double mu, double sigma)
{
    // nc::random::seed(time(nullptr));
    w = mu + nc::random::rand<double>(nc::Shape{ 1, n }) * sigma;
    w = w - nc::mean(w);
    w = w / sqrt(nc::var(w));
}

// 1.2. INS error simulation
void Solution::getSpeedErrors()
{
    // Input noise matrix (continuous)
    nc::NdArray<double> B = nc::transpose(nc::NdArray<double>({ 0, 0, 1 }));

    // Input noise matrix (discrete)
    const nc::NdArray<double> G = static_cast<double>(constants::T) * B;

    for (int i = 0; i < n - 1; ++i)
    {
        x = nc::hstack({ x, nc::dot(F, x(x.rSlice(), i)) + 5e-08 * w[i] * G });
    }
}

// 1.3. Speed measurements simulation
void Solution::getSpeedMeasurements()
{
    const double V = 0.1 * nc::max(x(0, x.cSlice()))[0]; // Noise intensity
    std::cout << "Measurement noise = " << V << std::endl;

    for (int i = 0; i < n; ++i)
    {
        z = nc::hstack({ z, nc::dot(H, x(x.rSlice(), i)) + w[i] * V });
    }
}

void Solution::estimateOptimalQ()
{
    const nc::NdArray<double> q_power = nc::linspace(-20., -1., 20);
    std::array<double, 20> q;
    for (int i = 0; i < q_power.size(); ++i)
    {
        q[i] = nc::powerf(10., q_power[i]);
    }
    nc::NdArray<double> stddev_err = nc::zeros<double>(nc::Shape{ 4, static_cast<nc::uint32>(q.size()) });

    const double V = 0.1 * nc::max(x(0, x.cSlice()))[0]; // Noise intensity
    const double R = V * V;

    nc::NdArray<double> p_diag;
    for (int i = 0; i < 3; ++i)
    {
        p_diag = nc::append(p_diag, nc::dot( nc::max(x(i, x.cSlice())), nc::max(x(i, x.cSlice())) ));
    }
    const nc::NdArray<double> I = nc::eye<double>(nc::Shape{ 3, 3 });

    // Speed, angle and drift
    for (int i = 0; i < q.size(); ++i)
    {
        nc::NdArray<double> p = p_diag * I;
        const nc::NdArray<double> Q = q[i] * nc::NdArray<double>({ {0, 0, 0}, {0, 0, 0}, {0, 0, 1} });
        nc::NdArray<double> x_estimations = nc::transpose(nc::NdArray<double>({ 0, 0, 0 }));

        for (int j = 0; j < n - 1; ++j)
        {
            const nc::NdArray<double> P = nc::dot(nc::dot(F, p), nc::transpose(F)) + Q;
            const nc::NdArray<double> K = nc::dot(P, nc::transpose(H)) * nc::linalg::inv(nc::dot(nc::dot(H, P), nc::transpose(H)) + R);
            x_estimations = nc::hstack({ x_estimations, nc::dot(F, x_estimations(x_estimations.rSlice(), j)) + K * (z(0, j + 1) - nc::dot(nc::dot(H, F), x_estimations(x_estimations.rSlice(), j))) });
            p = nc::dot((I - nc::dot(K, H)), P);
            // Summary var
            stddev_err(0, i) = stddev_err(0, i) + nc::sum( (x(x.rSlice(), j + 1) - x_estimations(x_estimations.rSlice(), j + 1)) * (x(x.rSlice(), j + 1) - x_estimations(x_estimations.rSlice(), j + 1)) )[0] / constants::simulation_time;
            // Var for each component
            stddev_err(1, i) = stddev_err(1, i) + ((x(0, j + 1) - x_estimations(0, j + 1)) * (x(0, j + 1) - x_estimations(0, j + 1))) / constants::simulation_time;
            stddev_err(2, i) = stddev_err(2, i) + ((x(1, j + 1) - x_estimations(1, j + 1)) * (x(1, j + 1) - x_estimations(1, j + 1))) / constants::simulation_time;
            stddev_err(3, i) = stddev_err(3, i) + ((x(2, j + 1) - x_estimations(2, j + 1)) * (x(2, j + 1) - x_estimations(2, j + 1))) / constants::simulation_time;
        }
    }

    const size_t q_sum_min_index = nc::argmin(stddev_err(0, stddev_err.cSlice()))[0];
    const size_t q_speed_min_index = nc::argmin(stddev_err(1, stddev_err.cSlice()))[0];
    const size_t q_angle_min_index = nc::argmin(stddev_err(2, stddev_err.cSlice()))[0];
    const size_t q_drift_min_index = nc::argmin(stddev_err(3, stddev_err.cSlice()))[0];
    Q_optimal[0] = q[q_sum_min_index];
    Q_optimal[1] = q[q_speed_min_index];
    Q_optimal[2] = q[q_angle_min_index];
    Q_optimal[3] = q[q_drift_min_index];

    for (int i = 0; i < Q_optimal.size(); ++i)
    {
        std::cout << "Component #" << i << ": stddev = " << nc::min(stddev_err(i, stddev_err.cSlice()))[0] << ", optimal Q = " << Q_optimal[i] << std::endl;
    }
}

// 2.2. Default Kalman Filter
void Solution::setupKalmanFilter()
{
    nc::NdArray<double> p_diag;
    for (int i = 0; i < 3; ++i)
    {
        p_diag = nc::append(p_diag, nc::dot( nc::max(x(i, x.cSlice())), nc::max(x(i, x.cSlice())) ));
    }

    const nc::NdArray<double> I = nc::eye<double>(nc::Shape{ 3, 3 });
    const nc::NdArray<double> Q = Q_optimal[0] * nc::NdArray<double>({ {0, 0, 0}, {0, 0, 0}, {0, 0, 1} });
    const double V = 0.1 * nc::max(x(0, x.cSlice()))[0]; // Noise intensity
    const double R = V * V;
    nc::NdArray<double> p = p_diag * I;

    for (int i = 0; i < n - 1; ++i)
    {
        const nc::NdArray<double> P = nc::dot(nc::dot(F, p), nc::transpose(F)) + Q;
        const nc::NdArray<double> K = nc::dot(P, nc::transpose(H)) * nc::linalg::inv(nc::dot(nc::dot(H, P), nc::transpose(H)) + R);
        x_err = nc::hstack({ x_err, nc::dot(F, x_err(x_err.rSlice(), i)) + K * (z(0, i + 1) - nc::dot(nc::dot(H, F), x_err(x_err.rSlice(), i))) });
        p = nc::dot((I - nc::dot(K, H)), P);
    }
}

// 2.3. Kalman Filter with various P
void Solution::setupKalmanFilterP()
{
    {
        nc::NdArray<double> p_diag;
        for (int i = 0; i < 3; ++i)
        {
            p_diag = nc::append(p_diag, nc::dot( nc::max(x(i, x.cSlice())), nc::max(x(i, x.cSlice())) ));
        }

        const nc::NdArray<double> I = nc::eye<double>(nc::Shape{ 3, 3 });
        const nc::NdArray<double> Q = Q_optimal[0] * nc::NdArray<double>({ {0, 0, 0}, {0, 0, 0}, {0, 0, 1} });
        const double V = 0.1 * nc::max(x(0, x.cSlice()))[0]; // Noise intensity
        const double R = V * V;
        nc::NdArray<double> p = 0.01 * p_diag * I;

        for (int i = 0; i < n - 1; ++i)
        {
            const nc::NdArray<double> P = nc::dot(nc::dot(F, p), nc::transpose(F)) + Q;
            const nc::NdArray<double> K = nc::dot(P, nc::transpose(H)) * nc::linalg::inv(nc::dot(nc::dot(H, P), nc::transpose(H)) + R);
            x_err_pmin = nc::hstack({ x_err_pmin, nc::dot(F, x_err_pmin(x_err_pmin.rSlice(), i)) + K * (z(0, i + 1) - nc::dot(nc::dot(H, F), x_err_pmin(x_err_pmin.rSlice(), i))) });
            p = nc::dot((I - nc::dot(K, H)), P);
        }
    }

    {
        nc::NdArray<double> p_diag;
        for (int i = 0; i < 3; ++i)
        {
            p_diag = nc::append(p_diag, nc::dot( nc::max(x(i, x.cSlice())), nc::max(x(i, x.cSlice())) ));
        }

        const nc::NdArray<double> I = nc::eye<double>(nc::Shape{ 3, 3 });
        const nc::NdArray<double> Q = Q_optimal[0] * nc::NdArray<double>({ {0, 0, 0}, {0, 0, 0}, {0, 0, 1} });
        const double V = 0.1 * nc::max(x(0, x.cSlice()))[0]; // Noise intensity
        const double R = V * V;
        nc::NdArray<double> p = 100. * p_diag * I;

        for (int i = 0; i < n - 1; ++i)
        {
            const nc::NdArray<double> P = nc::dot(nc::dot(F, p), nc::transpose(F)) + Q;
            const nc::NdArray<double> K = nc::dot(P, nc::transpose(H)) * nc::linalg::inv(nc::dot(nc::dot(H, P), nc::transpose(H)) + R);
            x_err_pmax = nc::hstack({ x_err_pmax, nc::dot(F, x_err_pmax(x_err_pmax.rSlice(), i)) + K * (z(0, i + 1) - nc::dot(nc::dot(H, F), x_err_pmax(x_err_pmax.rSlice(), i))) });
            p = nc::dot((I - nc::dot(K, H)), P);
        }
    }
}

// 2.3. Kalman Filter with various R
void Solution::setupKalmanFilterR()
{
    {
        nc::NdArray<double> p_diag;
        for (int i = 0; i < 3; ++i)
        {
            p_diag = nc::append(p_diag, nc::dot( nc::max(x(i, x.cSlice())), nc::max(x(i, x.cSlice())) ));
        }

        const nc::NdArray<double> I = nc::eye<double>(nc::Shape{ 3, 3 });
        const nc::NdArray<double> Q = Q_optimal[0] * nc::NdArray<double>({ {0, 0, 0}, {0, 0, 0}, {0, 0, 1} });
        const double V = 0.1 * nc::max(x(0, x.cSlice()))[0]; // Noise intensity
        const double R = 0.01 * V * V;
        nc::NdArray<double> p = p_diag * I;

        for (int i = 0; i < n - 1; ++i)
        {
            const nc::NdArray<double> P = nc::dot(nc::dot(F, p), nc::transpose(F)) + Q;
            const nc::NdArray<double> K = nc::dot(P, nc::transpose(H)) * nc::linalg::inv(nc::dot(nc::dot(H, P), nc::transpose(H)) + R);
            x_err_rmin = nc::hstack({ x_err_rmin, nc::dot(F, x_err_rmin(x_err_rmin.rSlice(), i)) + K * (z(0, i + 1) - nc::dot(nc::dot(H, F), x_err_rmin(x_err_rmin.rSlice(), i))) });
            p = nc::dot((I - nc::dot(K, H)), P);
        }
    }

    {
        nc::NdArray<double> p_diag;
        for (int i = 0; i < 3; ++i)
        {
            p_diag = nc::append(p_diag, nc::dot( nc::max(x(i, x.cSlice())), nc::max(x(i, x.cSlice())) ));
        }

        const nc::NdArray<double> I = nc::eye<double>(nc::Shape{ 3, 3 });
        const nc::NdArray<double> Q = Q_optimal[0] * nc::NdArray<double>({ {0, 0, 0}, {0, 0, 0}, {0, 0, 1} });
        const double V = 0.1 * nc::max(x(0, x.cSlice()))[0]; // Noise intensity
        const double R = 100. * V * V;
        nc::NdArray<double> p = p_diag * I;

        for (int i = 0; i < n - 1; ++i)
        {
            const nc::NdArray<double> P = nc::dot(nc::dot(F, p), nc::transpose(F)) + Q;
            const nc::NdArray<double> K = nc::dot(P, nc::transpose(H)) * nc::linalg::inv(nc::dot(nc::dot(H, P), nc::transpose(H)) + R);
            x_err_rmax = nc::hstack({ x_err_rmax, nc::dot(F, x_err_rmax(x_err_rmax.rSlice(), i)) + K * (z(0, i + 1) - nc::dot(nc::dot(H, F), x_err_rmax(x_err_rmax.rSlice(), i))) });
            p = nc::dot((I - nc::dot(K, H)), P);
        }
    }
}

// 2.3. Kalman Filter with various Q
void Solution::setupKalmanFilterQ()
{
    {
        nc::NdArray<double> p_diag;
        for (int i = 0; i < 3; ++i)
        {
            p_diag = nc::append(p_diag, nc::dot( nc::max(x(i, x.cSlice())), nc::max(x(i, x.cSlice())) ));
        }

        const nc::NdArray<double> I = nc::eye<double>(nc::Shape{ 3, 3 });
        const nc::NdArray<double> Q = 0.01 * Q_optimal[0] * nc::NdArray<double>({ {0, 0, 0}, {0, 0, 0}, {0, 0, 1} });
        const double V = 0.1 * nc::max(x(0, x.cSlice()))[0]; // Noise intensity
        const double R = V * V;
        nc::NdArray<double> p = p_diag * I;

        for (int i = 0; i < n - 1; ++i)
        {
            const nc::NdArray<double> P = nc::dot(nc::dot(F, p), nc::transpose(F)) + Q;
            const nc::NdArray<double> K = nc::dot(P, nc::transpose(H)) * nc::linalg::inv(nc::dot(nc::dot(H, P), nc::transpose(H)) + R);
            x_err_qmin = nc::hstack({ x_err_qmin, nc::dot(F, x_err_qmin(x_err_qmin.rSlice(), i)) + K * (z(0, i + 1) - nc::dot(nc::dot(H, F), x_err_qmin(x_err_qmin.rSlice(), i))) });
            p = nc::dot((I - nc::dot(K, H)), P);
        }
    }

    {
        nc::NdArray<double> p_diag;
        for (int i = 0; i < 3; ++i)
        {
            p_diag = nc::append(p_diag, nc::dot( nc::max(x(i, x.cSlice())), nc::max(x(i, x.cSlice())) ));
        }

        const nc::NdArray<double> I = nc::eye<double>(nc::Shape{ 3, 3 });
        const nc::NdArray<double> Q = 100. * Q_optimal[0] * nc::NdArray<double>({ {0, 0, 0}, {0, 0, 0}, {0, 0, 1} });
        const double V = 0.1 * nc::max(x(0, x.cSlice()))[0]; // Noise intensity
        const double R = V * V;
        nc::NdArray<double> p = p_diag * I;

        for (int i = 0; i < n - 1; ++i)
        {
            const nc::NdArray<double> P = nc::dot(nc::dot(F, p), nc::transpose(F)) + Q;
            const nc::NdArray<double> K = nc::dot(P, nc::transpose(H)) * nc::linalg::inv(nc::dot(nc::dot(H, P), nc::transpose(H)) + R);
            x_err_qmax = nc::hstack({ x_err_qmax, nc::dot(F, x_err_qmax(x_err_qmax.rSlice(), i)) + K * (z(0, i + 1) - nc::dot(nc::dot(H, F), x_err_qmax(x_err_qmax.rSlice(), i))) });
            p = nc::dot((I - nc::dot(K, H)), P);
        }
    }
}

// 2.4. Kalman Filter with long-term estimation (from 90th minute)
void Solution::setupKalmanFilterLong()
{
    nc::NdArray<double> t_long = nc::linspace(1., 90. * 60., 90. * 60. + 1.);

    for (int i = 0; i < n - 1; ++i)
    {
        if (i < t_long.shape().cols)
        {
            x_err_long = nc::hstack({ x_err_long, x_err(x_err.rSlice(), i) });
        }
        else
        {

            x_err_long = nc::hstack({ x_err_long, nc::dot(F, x_err_long(x_err_long.rSlice(), i)) });
        }
    }
}

// 2.5. Kalman Filter with short-term estimation
void Solution::setupKalmanFilterShort()
{
    nc::NdArray<double> p_diag;
    for (int i = 0; i < 3; ++i)
    {
        p_diag = nc::append(p_diag, nc::dot(nc::max(x(i, x.cSlice())), nc::max(x(i, x.cSlice()))) );
    }
    const nc::NdArray<double> I = nc::eye<double>(nc::Shape{ 3, 3 });
    nc::NdArray<double> p = p_diag * I;
    const nc::NdArray<double> Q = Q_optimal[0] * nc::NdArray<double>({ {0, 0, 0}, {0, 0, 0}, {0, 0, 1} });
    const double V = 0.1 * nc::max(x(0, x.cSlice()))[0]; // Noise intensity
    const double R = V * V;

    // Time intervals
    nc::NdArray<int> t_5s = nc::NdArray<int>({ {5000, 5001, 5002, 5003, 5004},
                                                     {6000, 6001, 6002, 6003, 6004},
                                                     {7000, 7001, 7002, 7003, 7004} });
    nc::NdArray<int> t_5s_last = t_5s(t_5s.rSlice(), t_5s.shape().cols - 1) + 1;

    nc::NdArray<int> t_5m = nc::linspace(1000, 1299, 300);
    t_5m = nc::vstack( { t_5m, nc::linspace(3000, 3299, 300) } );
    nc::NdArray<int> t_5m_last = t_5m(t_5m.rSlice(), t_5m.shape().cols - 1) + 1;

    for (int i = 0; i < n - 1; ++i)
    {
        if (nc::any(t_5s_last == i)[0] || nc::any(t_5m_last == i)[0])
        {
            p = p_diag * I;
        }

        if (nc::any(t_5s == i)[0] || nc::any(t_5m == i)[0])
        {
            x_err_short = nc::hstack({ x_err_short, nc::dot(F, x_err_short(x_err_short.rSlice(), i)) });
        }
        else
        {
            const nc::NdArray<double> x_err_prev = nc::dot(F, x_err_short(x_err_short.rSlice(), i));
            const nc::NdArray<double> P = nc::dot(nc::dot(F, p), nc::transpose(F)) + Q;
            const nc::NdArray<double> K = nc::dot(nc::dot(P, nc::transpose(H)), nc::linalg::inv(nc::dot(nc::dot(H, P), nc::transpose(H)) + R));
            x_err_short = nc::hstack({ x_err_short, x_err_prev + nc::dot(K, (z(0, i + 1) - nc::dot(H, x_err_prev))) });
            p = nc::dot((I - nc::dot(K, H)), P);
        }
    }
}
