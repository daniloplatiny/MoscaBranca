%Abre a porta Serial
clc
s = serial('COM10','BaudRate',57600); % change the COM Port number as needed
%% Connect the serial port to Arduino
s.InputBufferSize = 1; % read only one byte every time
try
    fopen(s);
catch err
    fclose(instrfind);
    error('Make sure you select the correct COM Port where the Arduino is connected.');
end


for i=1:10
fprintf(ComTest, '*IDN?');
idn=fscanf(ComTest)
end

fclose(s);

