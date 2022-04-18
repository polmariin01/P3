clear;
close all;
[y,Fs] = audioread('rl002.wav');
%195 ms fins a 225 ms
T = 30e-3;      %Periode de 30ms
N = Fs * T;     %Number of samples per window
n0 = 38100;      %Mostra on comença la finestra (8a finestra)
x = y(n0+1:n0+N); %audio enfinestrat
fmin = 50;
fmax = 500;
pmin = Fs / fmax;
pmax = Fs / fmin;

subplot(2,1,1);
plot(0:1/Fs:T-1/Fs , x);
title("Senyal x")
r = correlation(x,N);
s = size(r);
s = s(2);
subplot(2,1,2);
plot(r)

ext = 0;
rmax = 0;

for i = pmin : pmax
    if r(i) > rmax
        rmax = r(i);
    end
    hold on;
    if i > 1
        if i < s
            if (r(i)-r(i-1)) * (r(i+1)-r(i)) < 0
                ext = ext + 1;
                plot(i,r(i),'ro')
            end
        end
    end
end
disp(ext)
titulo = sprintf("Extremos relativos correlación (%d extrems)",ext);
title(titulo);
hold off;
%     
% for(iR = iRMax = r.begin() + npitch_min; iR < r.begin() + npitch_max; iR++){
%   if(*iR > *iRMax) iRMax = iR;
%   if(iR != r.begin() && iR != r.end() && (*iR-*prev(iR,1))*(*next(iR,1)-*iR) < 0) ext++;
% }