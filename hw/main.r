source("clc.r")
clear <- 0
class(clear) <- "cleanup"


deg2rad <- function(deg) {
    (deg * pi) / 180
}


##  ------------------------------------------------------------
##                      External parameters
##                      Earth-fixed frame
##  ------------------------------------------------------------


# Gravity force acceleration absolute value, m/s^2
g_ef <- 9.815

# WGS-84 major axle, m
a_ef <- 6378137

# WGS-84 minor axle, m
b_ef <- 6356752.3

# WGS-84 eccentricity
e_ef <- (a^2 - b^2) / a^2

# Earth's rotation speed, rad/s
U_ef <- 2 * pi / (24 * 60 * 60)

# Object height, m
H_ef <- 0


##  ------------------------------------------------------------
##                      Simulation parameters
##                      Navigation frame
##  ------------------------------------------------------------


# Measurement acquisition frequency, Hz
nu_N <- 10

# Measurement acquisition time interval, s
h_N1 <- 1 / nu_N
h_N3 <- h_N1

# Final simulation time, s
time_finite <- 2 * 60 * 60

# Simulation time interval
time <- seq(from = 0, to = time_finite, by = h_N1)


##  ------------------------------------------------------------
##                      Initial coordinates
##                      
##  ------------------------------------------------------------


# Longitude value
phi_0 <- deg2rad(27)

# Latitude value
lambda_0 <- deg2rad(13)

# Azimuth value
eps_0 <- deg2rad(0)


##  ------------------------------------------------------------
##                      Sensors inaccuracies
##                      
##  ------------------------------------------------------------


# Are inaccuracies enabled?
inaccuracies_enabled <- 1

# Acceleremoter bias (quartz AK-18T)
bias <- 25 * g * 1e-6

# Drift speed
w_dr <- deg2rad(0.01) / 3600


##  ------------------------------------------------------------
##                      Rotation mode
##                      
##  ------------------------------------------------------------


# Is rotation enabled?
rotation_enabled <- 0

# Rotation axis
axis <- 2

# Rotation angle
angle <- deg2rad(10)

# Rotation start time
rotation_start_time <- 60 * 60

# Rotation duration
rotation_duration <- 5


##  ------------------------------------------------------------
##                      Initialization
##                      
##  ------------------------------------------------------------

sum_Wx <- 0
sum_Wy <- 0
sum_Kx <- 0
sum_Ky <- 0

Q_f <- matrix(
    data = c(1, 0, 0, 0),
    nrow = 1,
    ncol = 4
)

omega <- matrix(
    data = c(0, 0, 0),
    nrow = 3,
    ncol = 1
)

psi <- matrix(
    data = 0,
    nrow = 1,
    ncol = length(time)
)

theta <- matrix(
    data = 0,
    nrow = 1,
    ncol = length(time)
)

gamma <- matrix(
    data = 0,
    nrow = 1,
    ncol = length(time)
)

heading <- matrix(
    data = 0,
    nrow = 1,
    ncol = length(time)
)

V <- matrix(
    data = 0,
    nrow = 3,
    ncol = length(time)
)

U <- matrix(
    data = 0,
    nrow = 3,
    ncol = length(time)
)

V_N <- matrix(
    data = 0,
    nrow = 1,
    ncol = length(time)
)

V_E <- matrix(
    data = 0,
    nrow = 1,
    ncol = length(time)
)

phi <- matrix(
    data = 0,
    nrow = 1,
    ncol = length(time)
) + phi_0

lambda <- matrix(
    data = 0,
    nrow = 1,
    ncol = length(time)
) + lambda_0

eps <- matrix(
    data = 0,
    nrow = 1,
    ncol = length(time)
) + eps_0


##  ------------------------------------------------------------
##                      Sensors values
##                      
##  ------------------------------------------------------------


gyr_N <- matrix(
    data = c(0, U_ef * cos(phi_0), U_ef * sin(phi_0)),
    nrow = 3,
    ncol = 1
)

acc_N <- matrix(
    data = c(0, 0, g_ef),
    nrow = 3,
    ncol = 1
)

C_B2N <- diag(3)

B_E2N <- matrix(
    data = 0,
    nrow = 3,
    ncol = 3
)
B_E2N[1, 1] <- -sin(phi_0) * cos(lambda_0) * sin(eps_0) - sin(lambda_0) * cos(eps_0)
B_E2N[1, 2] <- sin(phi_0) * sin(lambda_0) * sin(eps_0) + cos(lambda_0) * cos(eps_0)
B_E2N[1, 3] <- cos(phi_0) * sin(eps_0)
B_E2N[2, 1] <- -sin(phi_0) * cos(lambda_0) * cos(eps_0) + sin(lambda_0) * sin(eps_0)
B_E2N[2, 2] <- -sin(phi_0) * sin(lambda_0) * cos(eps_0) - cos(lambda_0) * sin(eps_0)
B_E2N[2, 3] <- cos(phi_0) * cos(eps_0)
B_E2N[3, 1] <- cos(phi_0) * cos(lambda_0)
B_E2N[3, 2] <- cos(phi_0) * sin(lambda_0)
B_E2N[3, 3] <- sin(phi_0)

B_PE2N <- B_E2N

i <- matrix(
    data = c(1, 2, 3),
    nrow = 1,
    ncol = 3
)

j <- matrix(
    data = c(4, 5, 6),
    nrow = 3,
    ncol = 1
)

gyr_B <- t(C_B2N) %*% gyr_N + w_dr * inaccuracies_enabled
print(norm(gyr_B))