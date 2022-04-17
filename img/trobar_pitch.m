clear;
close all;
[y,Fs] = audioread('rl002.wav');
%195 ms fins a 225 ms
T = 30e-3;      %Periode de 30ms
N = Fs * T;     %Number of samples per window
n0 = 10000;      %Mostra on comença la finestra (8a finestra)
x = y(n0+1:n0+N); %audio enfinestrat
r = zeros(1,N); %aurocorrelació (l'hem possat del mateix tamany de x)
f0min = 50;     %Frecuencia minima on buscarem el pitch
f0max = 500;    %Frecuencia máxima on buscarem el pitch
Pmin = Fs / f0max;  %Periode mínim que pot tenir el pitch
Pmax = Fs / f0min;  %Periode máxim que pot tenir el pitch

%Calcula correlació
for i = 1:N %aquest corre les r
   for n = i:N %aquest corre la senyal
       r(i) = r(i)+ x(n)*x(n-i+1); 
   end
   r(i) = r(i)/N;
end

%Calcula

%Maxim i Index del maxim localitzat entre els periodes que conté el pitch
[M,I] = max( r(Pmin:Pmax));
k0 = I + Pmin - 1;   %Mostra on està el màxim dins del vector r sencer
f0 = Fs / k0;        %Frequencia en Hz del pitch
p0 = 10*log10(M);   %Potencia en dB del màxim

%Mostrar resultats
figure;
subplot(2,1,1);
plot(0 : 1/Fs : T - 1/Fs , x);
title('Time domain voiced signal');
subplot(2,1,2);
plot(r);
title('Autocorrelation');
hold on
plot(Pmin:Pmax,r(Pmin:Pmax),'r')
hold on
plot(k0,M,'ro','MarkerEdgeColor','k', 'MarkerFaceColor','r','MarkerSize',4);
text(k0+5,M,'\leftarrow Pitch')
text(k0-20,M*1.3,sprintf('k=%d f=%.1fHz',k0,f0))
hold on
plot([Pmin Pmax],[r(Pmin) r(Pmax)],'r|')
hold off
