clc
clear variables
close all

%% 1. Input data reading from files

filename_A = 'A.txt';
filename_B = 'B.txt';
filename_C = 'C.txt';
filename_coord = 'coord_rad.txt';
filename_output = 'output.txt';

% Readmatrix
A = readmatrix(filename_A);
fprintf("A = \n");
disp(A);

dlmwrite('A_out.txt', A);

% Load
B = load(filename_B);
fprintf("B = \n");
disp(B);

% Line by line reading
C = [];
C_fid = fopen(filename_C);
while ~feof(C_fid)
    tline = fgetl(C_fid);
    C = [C; str2num(tline)];
end
fprintf("C = \n");
disp(C);

%% 2. Matrix transformations

% det, *, '
det_A = det(A);
B_multiplied = 5 * B;
C_transformed = C';

fprintf("det(A) = %d\n\n", det_A);
fprintf("5 * B = \n");
disp(B_multiplied);
fprintf("C^T = \n");
disp(C_transformed);


% Inverse matrix
inv_A = inv(A);
det_inv_A = det(inv_A);


fprintf("A^-1 = \n");
disp(inv_A);
fprintf("det(A^-1) = %f\n\n", det_inv_A);
fprintf("1/det(A) = %f\n\n", 1/det_A);

% Eye matrix
eye_matrix = eye(3);
diag_elements = diag(eye_matrix);

fprintf("Eye matrix = \n");
disp(eye_matrix);
fprintf("Diag elements = \n");
disp(diag_elements);

% C matrix transformations
size_C = size(C);

fprintf("C = \n");
disp(C);
fprintf("Size of C = \n");
disp(size_C);
fprintf("C sorted by rows ascending = \n");
disp(sort(C, 2));
fprintf("C sorted by rows descending = \n");
disp(sort(C, 2, 'descend'));
fprintf("C sorted by columns ascending = \n");
disp(sort(C));
fprintf("C sorted by columns descending = \n");
disp(sort(C, 'descend'));

% Reshaping C matrix
Q1 = reshape(C, 1, []);
Q2 = reshape(C, [], 1);

fprintf("C Q1 row = \n");
disp(Q1);
fprintf("C Q2 column = \n");
disp(Q2);

% Writing B column
Q1 = B(:);
writematrix(Q1, filename_output);

fprintf("B Q1 column = \n");
disp(Q1);

% Writing B row
Q2 = B(:)';

fprintf("B Q2 row = \n");
disp(Q2);

%% 3. Plotting

% 1 plot with 2 functions
f1 = figure();

x = linspace(-2*pi, 2*pi, 1e3);
plot(x, sin(x), 'LineWidth', 4, 'Color', 'blue', 'LineStyle', '--', 'Marker', 'x');
hold on;
plot(x, sin(15 * x));
hold off;

grid on;
title('Variant 2 plots');
xlabel('x');
ylabel('f(x)');
legend('sin(x)', 'sin(15*x)');

% 1st plot with 2 original functions and 2nd with 2 scaled functions
f2 = figure();

subplot(2, 1, 1);
x = linspace(-2*pi, 2*pi, 1e3);
plot(x, sin(x), 'LineWidth', 4, 'Color', 'blue', 'LineStyle', '--', 'Marker', 'x');
hold on;
plot(x, sin(15 * x));
hold off;

grid on;
title('Original functions');
xlabel('x');
ylabel('f(x)');
legend('sin(x)', 'sin(15*x)');

subplot(2, 1, 2);
x = linspace(-2*pi, 2*pi, 1e3);
plot(x, sin(x), 'LineWidth', 4, 'Color', 'blue', 'LineStyle', '--', 'Marker', 'x');
hold on;
plot(x, sin(15 * x));
hold off;

axis( [-pi, pi, -1, 1] );
grid on;
title('Scaled functions');
xlabel('x');
ylabel('f(x)');
legend('sin(x)', 'sin(15*x)');

% 3 dimensional plot
x = -2:0.25:2;
[X, Y] = meshgrid(x);
Z = X .* exp(-X.^2 - Y.^2);

f3 = figure();
surf(X, Y, Z);

x = linspace(-2, 2, 100);
y = linspace(-2, 2, 100);
z = x .* exp(-x.^2 - y.^2);

f4 = figure();
plot3(x, y, z, 'LineWidth', 2);

%% 4. Reading coords data from file

coords_data_rad = readmatrix(filename_coord);
rads_to_degrees = 180/pi;
coords_data_deg = rads_to_degrees * coords_data_rad;

f5 = figure();
degrees_table = array2table(coords_data_deg, 'VariableNames', {'Latitude', 'Longitude'});
geobubble(degrees_table, 'Latitude', 'Longitude');