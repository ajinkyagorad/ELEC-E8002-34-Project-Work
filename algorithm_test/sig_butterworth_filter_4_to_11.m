%#codegen
function result = sig_butterworth_filter_4_to_11(data)
    fs = 200;
    
    hp_order = 2;
    fc = 4; % 0.5Hz
    [B_hp,A_hp] = butter(hp_order,fc/(fs/2),'high');
    
    lp_order = 2;
    fc = 11; % 11 Hz
    [B_lp,A_lp] = butter(lp_order,fc/(fs/2),'low');
    
    result = filter(B_hp,A_hp,data);
    result = filter(B_lp,A_lp,result);
end



