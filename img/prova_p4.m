[y,Fs] = audioread('rl002.wav');
%195 ms fins a 225 ms
N = 600;
x = y(4201:4200+N);
r = zeros(1,N);
for i = 1:N %aquest corre les r
   for n = i:N %aquest corre la senyal
       r(i) = r(i)+ x(n)*x(n-i+1); 
   end
   r(i) = r(i)/N;
end
[M,I] = max(r(40:400));
figure;
subplot(2,1,1);
plot(0:1/20000:0.03-1/20000,x);
title('Time domain voiced signal');
subplot(2,1,2);
plot(r);
title('Autocorrelation');
hold on
plot(40:400,r(40:400),'r')
hold on
plot(I+39,M,'r*');
