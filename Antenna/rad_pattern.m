load POCKET_CUBE.mat

Nth = 181;
Nph = 360;

inc_th=pi/(Nth-1);
inc_ph=2*pi/(Nph);

[ph,th]=meshgrid([0:Nph-1]*inc_ph,[0:Nth-1]*inc_th);

P=(abs(Eth).^2+abs(Eph).^2)/(120*pi);
t=P/max(max(P)); %Hacemos 2 veces max para calcular el máximo de la matriz
figure(11)
representa_3D(t);
figure(2)
representa_corte(t,'XY')
figure(3)
representa_corte(t,'ZX')
figure(4)
representa_corte(t,'ZY')

aux=sum(sum(t.*sin(th),'all'),'all')*inc_th*inc_ph;
dir1_2=4*pi/aux

d=dir1_2*t;
% figure(5)
% representa_3D(d);
% figure(6)
% representa_corte(d,'XY')
% figure(7)
% representa_corte(d,'ZX')
% figure(8)
% representa_corte(d,'ZY')

% Pin=0.1585;
% 
% g = 4*pi*P/Pin;
% 
% g_dB = 10*log10(g);
% 
% G_max = max(max(g_dB))
% 

g_dB = 10*log10(abs(Eth));
G = max(max(g_dB))
G_dB = 10*log10(t) + G;
% figure(5)
% representa_3D(G_dB);
% figure(6)
% representa_corte(G_dB,'XY')
% figure(7)
% representa_corte(G_dB,'ZX')
% figure(8)
% representa_corte(G_dB,'ZY')

