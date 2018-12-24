close all;
clear all;
clc;

 disp('-------------ICCE---------------!');
 disp('Ideal Character Channel Emulator!');

 %   Serial Ports Initialization
 disp('Serial ports initialization.....');
 rs_rate = 38400;
 RxBufferSize = 131072;
 TxBufferSize = 131072;
 
 sport_A = serial('COM7', 'BaudRate', rs_rate, 'Parity','none', 'Terminator', '','InputBufferSize', RxBufferSize,'OutputBufferSize', TxBufferSize);
 fopen(sport_A)
 
 sport_B = serial('COM11', 'BaudRate', rs_rate, 'Parity','none', 'Terminator', '','InputBufferSize', RxBufferSize,'OutputBufferSize', TxBufferSize);
 fopen(sport_B)
 cnt = 0;
 
tx_cnt = 0;
rx_cnt = 0;     
while(1)
     % B-A
     len = sport_B.BytesAvailable;
     if len > 0
         data_in1(rx_cnt + 1:rx_cnt + len) = fread(sport_B,len);
         fwrite(sport_A, data_in1(rx_cnt + 1:rx_cnt + len), 'uchar'); 
         rx_cnt = rx_cnt + len;
     end
     
     % A-B
     len = sport_A.BytesAvailable;
     if len > 0
         data_in2(tx_cnt + 1:tx_cnt + len) = fread(sport_A,len);
         fwrite(sport_B, data_in2(tx_cnt + 1:tx_cnt + len), 'uchar');
         tx_cnt = tx_cnt + len;
     end
end

%%    End of Session
disp('Closing Serial ports .....');
fclose(sport_B)
clear sport_B
fclose(sport_A)
clear sport_A
disp('End of Session');
