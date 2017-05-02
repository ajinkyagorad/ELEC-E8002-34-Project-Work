%#codegen
function result = sig_butterworth_filter_0_66_to_2_5( data)

    fs = 200;
    
    hp_order = 2;
    fc = 0.66; % 0.5Hz
    [B_hp,A_hp] = butter(hp_order,fc/(fs/2),'high');
    
    lp_order = 2;
    fc = 2.5; % 2.5 Hz
    [B_lp,A_lp] = butter(lp_order,fc/(fs/2),'low');
    
    result = filter(B_hp,A_hp,data);
    result = filter(B_lp,A_lp,result);


end

