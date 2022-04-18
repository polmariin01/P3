close all;
N = 2001;
%x = zeros(1,N);
w = zeros(1,N);

for i=1:N
    w(i) = 0.54 - 0.46 * cos((2*3.141592*i)/(N-1));
end
plot(1:N,w);
axis([0 N 0 1])


