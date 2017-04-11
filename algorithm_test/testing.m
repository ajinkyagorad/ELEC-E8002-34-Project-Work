

    A1 = csvread('testdata6.csv');
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
    
    
    R = sqrt(Xf.^2 + Yf.^2 + Zf.^2);
    
    
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
    
    L = 400
    F = fft(Rf);
    
   
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
    findpeaks(Rf,T)
    xlabel('time')
    ylabel('Amp')
    title('All Peaks')
    
    
    peak_magnitude(1:11) = 0;
    peak_index(1:11) = 0;
    peak_type(1:11) = 0;
 
    max_th = 6e-3 ;
    min_th = 1e-3;
	
	kk = 1;
   
	slop(1:L+1)= 0;

	val_previous = Rf(1);
    val_current = 0;
 
        for ii = 1:L
           
        if Rf(ii) <  min_th || Rf(ii) >  max_th
            continue;
        end
		val_current = Rf(ii);
	  
        
        if (val_current-val_previous) > 0 
            slop(ii)=1;
        elseif (val_current-val_previous)<0 
            slop(ii)=-1; 
        else
            slop(ii) = 0;
        end
    
        if(slop(ii)==0 || slop(ii)*slop(ii-1) < 0)
        if(ii == 1) 
        continue;
        end
       
        peak_index(kk) = ii;

        if(slop(ii-1) > 0)
         peak_magnitude(kk) = val_current;
         peak_type(kk) = 1;
        elseif(slop(ii-1) < 0)  %not happening
          peak_magnitude(kk) = val_current;
          peak_type(kk) = -1;
           fprintf('wtf : ii %d, kk = %d   amp = %5f, T = %5f \n',ii,kk , peak_magnitude(kk), ii*dt)
        else
            continue;
        end
         fprintf('ii %d, kk = %d   amp = %5f, T = %5f \n',ii,kk , peak_magnitude(kk), ii*dt)
        kk = kk + 1;
        if(kk > 6)
        break;
        end
			
        end
		
		val_previous = val_current;
        end
	
        
        
        cnt_slop = 0;
    
        peak_prob = 0;
        
        for ii = 1:kk
            index = peak_index(ii)
            for jj = 1:6   
                if(index + jj < L)
                    if(slop(index+jj) > 0)
                        cnt_slop = cnt_slop + 1;
                    end
                end
            end
            fprintf('cnt slop : %d\n',cnt_slop)
            if(cnt_slop  > 1)
               for jj = ii:kk-1
                   fprintf('filtring: jj %d, kk = %d   amp = %5f, T = %5f \n',jj,kk , peak_magnitude(jj), peak_index(jj)*dt)
                   peak_index(jj) = peak_index(jj+1) ;
                   peak_magnitude(jj) =  peak_magnitude(jj+1);
                   peak_type(jj) =  peak_type(jj+1);
               end
                   kk = jj-1
                   cnt_slop = 0;
            end
        end
	
	for ii = 1:kk
        fprintf('filtered : ii %d, kk = %d   amp = %5f, T = %5f \n',ii,kk , peak_magnitude(ii), peak_index(ii)*dt)
    end
	
	current_maxima = 0;
    previous_maxima = -1;
    current_minima = 0;
    previous_minima = -1;
    previous = -1;
    count_diff = 1;
    min_spacing = 40;
    
    difference(1:11) = 0;

	 
	  
		
	    for ii = 1:kk
        if(peak_type(ii) == 1)
           if(previous_maxima == -1)
               previous_maxima = ii;
               current_maxima = ii;
               if(previous == -1)
				previous  = ii;
               end
            continue;
               
           end
           
           current_maxima  = ii;
           
           if((peak_index(current_maxima) - peak_index(previous)) < min_spacing )
                
               if(peak_type(previous) ==1 && peak_magnitude(current_maxima) > peak_magnitude(previous) && difference(count_diff-1))
                    difference(count_diff-1) = difference(count_diff-1) + peak_index(current_maxima) - peak_index(previous);
                    previous_maxima = current_maxima;
                    previous  = current_maxima;
               end              
             continue;
          end
             
           difference(count_diff) = peak_index(current_maxima) - peak_index(previous_maxima)
           count_diff = count_diff + 1;
            
           previous_maxima = current_maxima;
           previous  = current_maxima;
           
          
        else
          fprintf('error -%d',peak_type(ii))
        end
        end
    
			 
    cnt = 0;
    gap = 0;
    for ii = 1:10
          if(difference(ii) > 0)
             gap = gap + difference(ii); 
             cnt = cnt + 1; 
          end
    end
    
    bpm = 200*cnt*60/gap
    
    
    
    
    
