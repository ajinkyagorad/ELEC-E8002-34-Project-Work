function result = sig_butterworth_filter_0_66_to_2_5_C(data)

    x = data;
    y(1:length(x))=0;
    for n =1:5
        y(n) = x(n);
    end
    
    for n =5:length(x)
     y(n) = (1 * x(n- 4))+ (  0 * x(n- 3))+ ( -2 * x(n- 2))+ (  0 * x(n- 1))+ (  1 * x(n- 0))+ ( -0.9215032056 * y(n- 4))+ (  3.7582371567 * y(n- 3))+ ( -5.7518393940 * y(n- 2))+ (  3.9151028964 * y(n- 1));
    end
    

    result = y;



% 	dbuffer(1:5) = 0;
%     dv0(1:5) = [ 0.0008023531890466889, 0.0, -0.0016047063780933778, 0.0, 0.0008023531890466889 ];
% 
%     dv1(1:5) = [ 1.0, -3.9151028964225354, 5.7518393939858576, -3.7582371566656905, 0.9215032055789143 ];
% 
% 
% 
%   for k = 1:5
%     dbuffer(k) = 0.0;
%   end
%   L = length(x)
%   for j = 1:L
%       for k = 1:4
%          dbuffer(k) = dbuffer(k+1);
%       end
%    
%       
%       dbuffer(5) = 0.0;
%       for k = 1:5
%          dbuffer(k) = dbuffer(k) + times(data(j),dv0(k));
%       end
%       
%       for k = 1:4
%          dbuffer(k+1) = dbuffer(k+1) - times(dbuffer(1),dv1(k+1));
%       end
%       
%       result(j) = dbuffer(1);  
%   end
 


end

