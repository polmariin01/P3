close all;
%x = randn(1,400);
[x,fs1] = audioread('rl002.wav');
%x = [1,4,2,2,23,4,6,2,57,9,56,45,53,234,1,34,5,6,6,34,34,4,6,7,5,5,3,4,2,3,4,3,2,31,3,2,23,45,23,45,5,4,32,4,23,34,3,2,23,3,43,3,2,1,23,4,24,43,2,-20,12,3,4,3,2];
len_x = length(x);
%fs1 = 10;
d = len_x / fs1;
fd = 4; %Factor de delmació
x2 = zeros(1,floor(len_x/fd));
len_x2 = length(x2);
fs2 = fs1 / fd;
figure(2);
%for i = 1:len_x2-1
plot(0:1/fs1:d-1/fs1, x, 'b');
hold on;
for i = 1:len_x2-1
    %i
    x2(i) = x(fd*(i-1)+1);
    %fd*(i-1)+1
    for j = 1:fd-1
        %disp("for")
        if i>1
            a = x(fd*(i-1)-j+1) + x(fd*(i-1)+j+1);
            %fd*(i-1)-j+1
            %fd*(i-1)+j+1
        else
            %fd*(i-1)+j+1
            a = 2 * x(fd*i+j+1);
        end
        %disp("end")
        x2(i) = x2(i) + (1-j/fd) * a;
        %1-j/fd
    end
    x2(i) = x2(i) / fd;
%     if i>1
%         plot([(fd*(i-2)/fs1),(fd*(i-1))/fs1,(fd*(i)/fs1)], [0 1 0], 'r');
%         hold on;
%     elseif i == 1
%         plot([(fd*(i-1)),(fd*(i)/fs1)], [1 0], 'r');
%         hold on;
%     end
end
% hold off;
% figure 2;
plot(0:1/fs2:d-1/fs2, x2, 'c');
hold off;

%%
% for (i = 0; i < nouS; i++) {
%         x_ds[i] = x[fN*i];
%         for (j = 1; j < fN; j++) {
%           aux = (i>0)? x[fN*i + j] + x[fN*i - j] : x[fN*i + j];
%           x_ds[i] += (1 - j / fN) * aux;
%         }
%         x_ds[i] = (i>0)? aux / fN : (aux*2 / (fN+1));
%         if (verbose) cout << ": " << x_ds[i] << "\n";