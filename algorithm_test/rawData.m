clear all;
M = csvread('heart.csv');

subplot(2,1,1);plot(M(:,1),M(:,2:4));title('accelero'); legend( 'x', 'y', 'z');
subplot(2,1,2);plot(M(:,1),M(:,5:7)); title('gyro'); legend('x', 'y', 'z');
