function r = correlation(x,N)
    %Calcula correlació
    r = zeros(1,N);
    for i = 1:N %aquest corre les r
       for n = i:N %aquest corre la senyal
           r(i) = r(i)+ x(n)*x(n-i+1); 
       end
       r(i) = r(i)/N;
    end

end