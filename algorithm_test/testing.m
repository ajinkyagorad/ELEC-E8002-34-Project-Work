

    A1 = csvread('testdata4.csv');
%    A1 = coeff * csvread('accelerometer_reading_for_about_1min_18sec_in_50ms_interval_device_status_on_chest.csv',1);

    
%%

    fs = 200;
    dt = 1/fs;

%    Y = sqrt(sum(A1.^2,2));
    X = A1(:,2);
    Y = A1(:,3);
    Z = A1(:,4);
    %Ynorm = Y - averaging_filter(Y);
    T = (0:length(Y)-1) * dt;

    hp_order = 2;
    fc = 4; % 0.5Hz
    [B_hp,A_hp] = butter(hp_order,fc/(fs/2),'high');
    
    lp_order = 2;
    fc = 11; % xHz
    [B_lp,A_lp] = butter(lp_order,fc/(fs/2),'low');
    
    
    Xf = X - mean(X);
    Xf = filter(B_hp,A_hp,Xf);
    Xf = filter(B_lp,A_lp,Xf);
    
    Yf = Y - mean(Y);
    Yf = filter(B_hp,A_hp,Yf);
    Yf = filter(B_lp,A_lp,Yf);
    
    Zf = Z - mean(Z);
    Zf = filter(B_hp,A_hp,Zf);
    Zf = filter(B_lp,A_lp,Zf);
    
    
    R = sqrt(Xf.^2 + Yf.^2 + Zf.^2)
    
    
    hp_order = 2;
    fc = 0.66; % 0.5Hz
    [B_hp,A_hp] = butter(hp_order,fc/(fs/2),'high');
    
    lp_order = 2;
    fc = 2.5; % xHz
    [B_lp,A_lp] = butter(lp_order,fc/(fs/2),'low');
    
    
    Rf = R - mean(R);
    Rf = filter(B_hp,A_hp,Rf);
    Rf = filter(B_lp,A_lp,Rf);
    

    
  
    
    pick_1 = 0; pick_1_index =0;
    pick_2 = 0; pick_2_index =0;
    
    
    
    clf;
    subplot(2,1,1);

    plot(T,Rf);
    
    subplot(2,1,2);
    plot(T,Z);
    
    L = 300
    F = fft(Rf);
    
    
    
    %Compute the two-sided spectrum P2. Then compute the single-sided spectrum P1 based on P2 and the even-valued signal length L.
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
    frequency = indexMax * fs / L;
    
    bpm = frequency*60
    
    figure
    
    year = T; 
    relNums = Rf;
    findpeaks(relNums,year)
    xlabel('Year')
    ylabel('Sunspot Number')
    title('Find All Peaks')
    
    
    
    
