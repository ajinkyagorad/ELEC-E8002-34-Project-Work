function result = sig_butterworth_filter_4_to_11_C(data)

	x = data;
    y(1:length(x))=0;
    for n =1:5
        y(n) = x(n);
    end
    
    for n =5:length(x)
     y(n) = (1 * x(n- 4))+ (  0 * x(n- 3))+ ( -2 * x(n- 2))+ (  0 * x(n- 1))+ (  1 * x(n- 0))+ ( -0.7327260304 * y(n- 4))+ (  3.087883381 * y(n- 3))+ ( -4.9675926050 * y(n- 2))+ (  3.6108197545 * y(n- 1));
    end
    


   result = y;
		 

   
%    
% 	dbuffer(1:5) = 0;
%     dv0(1:5) = [ 0.0020805671354598072, 0.0,-0.0041611342709196144, 0.0, 0.0020805671354598072 ];
% 
%     dv1(1:5) = [ 1.0, -3.8478114968688084, 5.5721601378375674, -3.5994720753697242, 0.87521454825368439 ];
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




