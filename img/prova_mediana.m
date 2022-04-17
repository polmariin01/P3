%Proves filtre de mediana
PM = 1; %Siglas de Puta Matlab. Per a ajustar que aquest fill de puta comença els vectors a 1 i no a 0 com una persona normal

median_error = 2;
median_N = median_error * 2 + 1;
f0 = [1,4,2,3,5,6,7,4,4,2,3,4,5,6,7,5,2,3]

f0_aux = f0;
buffer_median = zeros(1,median_N);

for i = 1:median_error %Els primers valors seran iguals (el filtre comença a la mostra número 'median_error')
    f0(i) = f0_aux(i);
end

for i = 1:median_N  %Coloquem els primers N valors a un vector auxiliar per a calcular la mediana
    buffer_median(i) = f0_aux(i);
end
s = size(f0);
s = s(2);
j = 1;
for i = median_error + PM : s - median_error     % Recorrem el vector f0
    i
    buffer_median
    j=1;
    while j ~= median_N
        if buffer_median(j) > buffer_median(j+1)
            aux = buffer_median(j);                 % intercanvi de valors si no estan ordenats
            buffer_median(j) = buffer_median(j+1);
            buffer_median(j+1) = aux;
            if j > PM
                j = j - 2;
            end
        end
        j = j + 1;
    end
    disp(buffer_median)
    f0(i) = buffer_median(median_error + PM);   %Posem el valor corresponent al vector f0
    for j = 1 : median_N
        buffer_median(j) = f0_aux(i + j - 1 - median_error);
    end
    
end

%     for(j=i-median_error; j<i+median_error;j++) //Tornem a crear el vector amb els valors directes del vector f0
%       buffer_median[j] = f0_aux[j];

%%
% Codi en C/C++
%   int i, j=0;
%   int median_error = 1;                 /// Nombre d'errors que pot corregir el filtre de mediana
%   int median_N = median_error * 2 + 1;  /// Tamany del filtre de mediana per a corregir n errors
%   vector<float> f0_aux = f0;            /// Copia del vector f0 per a fer el processat
%   float buffer_median[median_N];        /// Buffer per a calcular la mediana, fem una especie de circular queue
% 
%   for (i = 0; i < median_error; i++) //Els primers valors seran iguals (el filtre comença a la mostra número 'median_error')
%     f0[i] = f0_aux[i];
%   
%   for (i = 0; i < median_N; i++){  //Coloquem els primers N valors a un vector auxiliar per a calcular la mediana
%     buffer_median[i] = f0_aux[i];
%   }
% 
%   //Filtre de mediana comença
%   float aux;
%   for (i = median_error; i < f0.size() - median_error - 1; i++) { //Recorrem el vector f0
%     //for(j = 0; j < median_N; j++) { //Ordenem el buffer, bucle de cops que recorre el buffer
%     //for(int j = 0; j < median_N - 1; j++){ /// \TODO Substituir aixo per un whil i probarho que funcioni bé, fer proves al MATLAB
%     while(j != median_N-1) {
%       if(buffer_median[j] > buffer_median[j+1]){
%         aux = buffer_median[j];                 //intercanvi de valors si no estan ordenats
%         buffer_median[j] = buffer_median[j+1];
%         buffer_median[j+1] = aux;
%         j--;                          //si ha hagut canvi, torna enrere per veure si ha de tornar-la a canviar la que ha baixat
%       } else{j++;}                    //si ja estaven ordenats, segueix avançant a ver que onda
%     } //en principi esta ja el vector ordenat
%     f0[i] = buffer_median[median_error]; //Posem el valor corresponent al vector f0
%     for(j=i-median_error; j<i+median_error;j++) //Tornem a crear el vector amb els valors directes del vector f0
%       buffer_median[j] = f0_aux[j];
%   }

