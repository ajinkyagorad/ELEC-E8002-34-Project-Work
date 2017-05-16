

    A1 = csvread('testdata6.csv');
%    A1 = coeff * csvread('accelerometer_reading_for_about_1min_18sec_in_50ms_interval_device_status_on_chest.csv',1);

    
%%

    fs = 200;
    dt = 1/fs;

%    Y = sqrt(sum(A1.^2,2));
    X = A1(:,2);
    Y = A1(:,3);
    Z = A1(:,4);
    K = A1(:,5);
    clear max clear index
    %Ynorm = Y - averaging_filter(Y);
    L = length(X)
    T = (0:L-1) * dt;

    
    Xf = X - mean(X);
    Xf = sig_butterworth_filter_4_to_11(Xf);
    Xf_2 = sig_butterworth_filter_4_to_11_C(Xf);

    
    Yf = Y - mean(Y);
    Yf = sig_butterworth_filter_4_to_11(Yf);
    Yf_2 = sig_butterworth_filter_4_to_11_C(Yf);

    
    Zf = Z - mean(Z);
    Zf = sig_butterworth_filter_4_to_11(Zf);
    Zf_2 = sig_butterworth_filter_4_to_11_C(Zf);

    
    
    R = sqrt(Xf.^2 + Yf.^2 + Zf.^2);
    R_2 = sqrt(Xf_2.^2 + Yf_2.^2 + Zf_2.^2);
    
    
    hp_order = 2;
    fc = 0.66; % 0.5Hz
    [B_hp,A_hp] = butter(hp_order,fc/(fs/2),'high');
    
    lp_order = 2;
    fc = 2.5; % xHz
    [B_lp,A_lp] = butter(lp_order,fc/(fs/2),'low');
    
    
    Rf = R - mean(R);
    Rf = filter(B_hp,A_hp,Rf);
    Rf = filter(B_lp,A_lp,Rf);
    
    
    Rf_2 = R_2 - mean(R_2);
    Rf_2 = sig_butterworth_filter_0_66_to_2_5_C(Rf_2);

    
    pick_1 = 0; pick_1_index =0;
    pick_2 = 0; pick_2_index =0;
    
    
    
    clf;
    subplot(4,1,1);
    plot(T, Rf);
    
    subplot(4,1,2);
    plot(T, Rf_2);
    
    subplot(4,1,3);
    plot(T,K);
    
    subplot(4,1,4);
    plot(T,R);
    
   
    %Rf = K;
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
    
    clear max clear index
    [maxValue,indexMax] = max(P2);
    frequency = indexMax * fs / L;
    
    bpm_fft = frequency*60
    
    figure
    findpeaks(Rf,T)
    xlabel('time')
    ylabel('Amp')
    title('All Peaks')
    
    
    peak_magnitude(1:11) = 0;
    peak_index(1:11) = 0;
    peak_type(1:11) = 0;
 
   
    min_th = 100;
	min_spacing = 70;
     
	kk = 1;
   
	slop(1:L+1)= 0;

	val_previous = Rf(1);
    val_current = 0;
    previous_index = 1;
    
        for ii = 2:L
        
        if Rf(ii) <  min_th    
            continue;
        end
        
        if ii - previous_index ~= 1
            previous_index = ii;
            val_previous = Rf(ii);
            continue;
        end
        
        previous_index = ii;
		val_current = Rf(ii);
	  
        
        if (val_current-val_previous) > 0 
            slop(ii)=1;
        elseif (val_current-val_previous)<0 
            slop(ii)=-1; 
        else
            slop(ii) = 0;
        end
       %fprintf('ch : ii %d ,slop(ii) = %d,slop(ii-1) = %d kk = %d   amp = %5f, T = %5f \n',ii,slop(ii),slop(ii-1) , kk , peak_magnitude(kk), ii*dt)  
        if((slop(ii)==0 || slop(ii) < 0) && slop(ii-1) > 0)
        if(ii == 1) 
        continue;
        end
         
        peak_index(kk) = ii;

        if(slop(ii-1) > 0)
         peak_magnitude(kk) = val_current;
         peak_type(kk) = 1;
        elseif(slop(ii-1) < 0)  
          peak_magnitude(kk) = val_current;
          peak_type(kk) = -1;
          % fprintf('wtf : ii %d ,slop(ii) = %d,slop(ii-1) = %d kk = %d   amp = %5f, T = %5f \n',ii,slop(ii),slop(ii-1) , kk , peak_magnitude(kk), ii*dt)
        else
            continue;
        end
        
         %fprintf('#ii %d, kk = %d   amp = %5f, T = %5f, type = %d\n',ii,kk , peak_magnitude(kk), ii*dt,  peak_type(kk))
        kk = kk + 1;
        if(kk > 9)
        break;
        end
			
        end
		
		val_previous = val_current;
        
        end
	
        
        
        cnt_slop_m = 0;
        cnt_slop_p = 0;
        obs_m = 0;
        obs_p = 0;
        peak_prob = 0;
        window = 6
        prob_limit = window - window/3;
        for ii = 1:kk
            cnt_slop_m = 0;
            cnt_slop_p = 0;
            obs_m = 0;
            obs_p = 0;
            index = peak_index(ii)
            
            for jj = 1:window   
                if(index + jj < L)
                     obs_m = obs_m+1;
                     %fprintf('index #ii %d, slop = %d \n',jj, slop(index+jj))
                    if(slop(index+jj) > 0)
                        cnt_slop_m = cnt_slop_m + 1;
                    end
                end
                
                if(index - jj > 1)
                     obs_p = obs_p+1;
                    % fprintf('index #ii %d, slop = %d \n',jj, slop(index-jj))
                    if(slop(index-jj) < 0)
                        cnt_slop_p = cnt_slop_p + 1;
                    end
                end
            end
%             fprintf('cnt slop m : %d, cnt slop p: %d \n', cnt_slop_m, cnt_slop_p)
%             fprintf('filter #ii %d, index = %d ,  amp = %5f, T = %5f \n',ii, index , peak_magnitude(ii), index*dt)
%             fprintf('forward projection #peak_magnitude(jj) %d, peak_magnitude(jj+1) = %d \n',peak_magnitude(ii), peak_magnitude(ii+1))
            
            if(cnt_slop_m  > 3 || cnt_slop_p  > 3 || ((peak_magnitude(ii) <  peak_magnitude(ii+1))&&((peak_index(ii+1) - peak_index(ii)) < min_spacing )))
               for jj = ii:kk
                   %fprintf('filtring: jj %d, kk = %d   amp = %5f, T = %5f \n',jj,kk , peak_magnitude(jj), peak_index(jj)*dt)
                   peak_index(jj) = peak_index(jj+1) ;
                   peak_magnitude(jj) =  peak_magnitude(jj+1);
                   peak_type(jj) =  peak_type(jj+1);
               end
                   kk = jj-1
                   
            end
        end
	
	for ii = 1:kk-1
        fprintf('filtered : ii %d, kk = %d   amp = %5f, T = %5f \n',ii,kk , peak_magnitude(ii), peak_index(ii)*dt)
    end
	
	current_maxima = 0;
    previous_maxima = -1;
    current_minima = 0;
    previous_minima = -1;
    count_diff = 1;

     amp = 0;
	 cnt_amp = 0;
    difference(1:11) = 0;
	  
		
	    for ii = 1:kk-1
            
        if(peak_type(ii) == 1)
           if(previous_maxima == -1)
               previous_maxima = ii;
               current_maxima = ii;
               continue;  
           end
           
           current_maxima  = ii;
           
           if((peak_index(current_maxima) - peak_index(previous_maxima)) < min_spacing )
                 %fprintf('filtered : peak_index(current_maxima) %d, peak_index(previous_maxima) = %d   peak_type(current_maxima) = %d \n',peak_index(current_maxima),peak_index(previous_maxima) ,peak_type(current_maxima))
               if(peak_type(current_maxima) == 1 && peak_magnitude(current_maxima) > peak_magnitude(previous_maxima) && difference(count_diff-1))
                    difference(count_diff-1) = difference(count_diff-1) + peak_index(current_maxima) - peak_index(previous_maxima);
                      amp = amp - peak_magnitude(previous_maxima) + peak_magnitude(current_maxima)
                      fprintf('filtered : difference(count_diff-1) %d\n',difference(count_diff-1))
                    previous_maxima = current_maxima;
               end              
             continue;
           end
             
           amp = amp + peak_magnitude(current_maxima)
           cnt_amp =  cnt_amp + 1;
           difference(count_diff) = peak_index(current_maxima) - peak_index(previous_maxima)
           count_diff = count_diff + 1;
            
           previous_maxima = current_maxima;
      
           
          
        else
          fprintf('error -%d',peak_type(ii))
        end
        end
    
   strength = amp/cnt_amp
			 
    cnt = 0;
    gap = 0;
    for ii = 1:10
          if(difference(ii) > 0)
             gap = gap + difference(ii); 
             cnt = cnt + 1; 
          end
    end
    
    bpm = 200*cnt*60/gap
    
    
    
    
    
