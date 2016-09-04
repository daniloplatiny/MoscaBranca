s = serial('COM9');
set(s,'BaudRate',56000);
fopen(s);
fprintf(s,'*IDN?')
out = fscanf(s);
fclose(s)
delete(s)
clear s