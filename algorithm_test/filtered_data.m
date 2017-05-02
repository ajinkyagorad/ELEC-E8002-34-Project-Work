 A1 = csvread('heart.csv');
%    A1 = coeff * csvread('accelerometer_reading_for_about_1min_18sec_in_50ms_interval_device_status_on_chest.csv',1);

    
%%

    fs = 200;
    dt = 1/fs;

%    Y = sqrt(sum(A1.^2,2));
    X = A1(:,1);

    %Ynorm = Y - averaging_filter(Y);
    T = (0:length(X)-1) * dt;

    figure
    plot(T,X);
    
    
    L = length(X);
    F = fft(X);
    
   
    P2 = abs(F);
    P1 = P2(1:L/2+1);
    P1(2:end-1) = 2*P1(2:end-1);
    
    figure
    f = fs*(0:(L/2))/L;
    plot(f,P1) 
    title('Single-Sided Amplitude Spectrum of X(t)')
    xlabel('f (Hz)')
    ylabel('|P1(f)|')
    
    [maxValue,indexMax] = max(P2);
    frequency = indexMax * fs / L
    
