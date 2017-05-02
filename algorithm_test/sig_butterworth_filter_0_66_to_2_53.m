function result = sig_butterworth_filter_0_66_to_2_5( data)



	dbuffer(1:6) = 0;
    dv0(1:5) = { 0.0008023531890466889, 0.0, -0.0016047063780933778, 0.0, 0.0008023531890466889 };

    dv1(1:5) = { 1.0, -3.9151028964225354, 5.7518393939858576, -3.7582371566656905, 0.9215032055789143 };

    for n =1:5
    
  for k = 1:5
    dbuffer(k) = 0.0;
  end

  for j = 1:length(data)
      for k = 1:5
         dbuffer(k) = dbuffer(k+1);
      end
      test = k
      
      dbuffer(5) = 0.0;
      for k = 1:6
         dbuffer(k) = dbuffer(k) + times(data(j),dv0(k));
      end
      
      for k = 1:5
         dbuffer(k+1) = dbuffer(k+1) - times(dbuffer(0),dv1(k));
      end
      
    result(j) = dbuffer(0);  
  end
 


end

