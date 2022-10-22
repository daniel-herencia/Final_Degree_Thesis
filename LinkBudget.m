clear all; 
%%------------------DISTANCE TO ELEVATION CONVERSION---------------------%%
elevation = linspace (0,90,40);
earthRadius = 6370000; %%in meters
orbitHeight = [400000,450000,500000, 550000]; %% in meters.
orbitRadius = earthRadius + orbitHeight;
% distanceH400 = earthRadius*(sqrt((orbitRadius(1)/earthRadius)^2-cosd(elevation).^2)-sind(elevation));
% distanceH450 = earthRadius*(sqrt((orbitRadius(2)/earthRadius)^2-cosd(elevation).^2)-sind(elevation));
% distanceH500 = earthRadius*(sqrt((orbitRadius(3)/earthRadius)^2-cosd(elevation).^2)-sind(elevation));
distanceH550 = earthRadius*(sqrt((orbitRadius(4)/earthRadius)^2-cosd(elevation).^2)-sind(elevation));
%distanceH550_1 = earthRadius*(sqrt((orbitRadius(4)/earthRadius)^2-cosd(5).^2)-sind(5))

figure(1)
plot(distanceH550,elevation)
xlabel('Distance between ground-station and satellite (km)');
ylabel('Elevation angle (º)');
xline(550,'--');
grid;

%%
% figure(1)
% plot(distanceH400,elevation)
% xlabel('Distance between ground-station and satellite (km)');
% ylabel('Elevation angle (º)');
% xline(400,'--');
% grid;
% hold off;
% 
% figure(2)
% plot(distanceH450,elevation)
% xlabel('Distance between ground-station and satellite (km)');
% ylabel('Elevation angle (º)');
% xline(450,'--');
% grid;
% hold off;
% 
% figure(3)
% plot(distanceH500,elevation)
% xlabel('Distance between ground-station and satellite (km)');
% ylabel('Elevation angle (º)');
% xline(500,'--');
% grid;



%%-----------------------------------------------------------------------%%


%%----------------------------LINK BUDGET--------------------------------%%
elevation = linspace (0,90,40);
earthRadius = 6370000; %%in meters
orbitHeight = [400000,450000,500000,550000]; %% in meters.
orbitRadius = earthRadius + orbitHeight;
% distanceH400 = earthRadius*(sqrt((orbitRadius(1)/earthRadius)^2-cosd(elevation).^2)-sind(elevation));
% distanceH450 = earthRadius*(sqrt((orbitRadius(2)/earthRadius)^2-cosd(elevation).^2)-sind(elevation));
% distanceH500 = earthRadius*(sqrt((orbitRadius(3)/earthRadius)^2-cosd(elevation).^2)-sind(elevation));
distanceH550 = earthRadius*(sqrt((orbitRadius(4)/earthRadius)^2-cosd(elevation).^2)-sind(elevation));

transmittedPower = 22; %% dBm, for the [SX1262 = 22, SX1276 = 18.5] transceiver
f = 868000000; %% Hz
groundStationGain = 10; % Gain in dB, use different values 0,5,10,15,20...
lambda = 3*10^8 / f;
L_scint = 1;
L_pol = 3;

%138.55 = 147.55 dB - 3 dB - 3 dB - 3 dB [atmospheric losses,
%polarization losses, other losses]
% receivedPowerH400 = transmittedPower + groundStationGain - 20*log10(distanceH400) - 20*log10(f) + 138.55;
% receivedPowerH450 = transmittedPower + groundStationGain - 20*log10(distanceH450) - 20*log10(f) + 138.55;
% receivedPowerH500 = transmittedPower + groundStationGain - 20*log10(distanceH500) - 20*log10(f) + 138.55;
receivedPowerH550 = 17.15 + 11.8 - 20*log10((4*pi/lambda)*distanceH550) - L_scint - L_pol;

% The sensivility varies depending on the SF selected
sensitivityPowerSX1262 = [-123,-126,-129,-132,-134.5,-137]; % SF=7,8,9,10,11,12, in dBm.
%sensitivityPowerSX1276 = [-122.5,-125.5,-128.5,-131.5,-132.5,-135.5]; % SF=7,8,9,10,11,12, in dBm.
% The bitrate also depends on the chosen SF
SF = [7,8,9,10,11,12];
BW = 125000; % Bandwidth (Hz)
vector2 = 2*ones(size(SF));
bitRate = floor(SF*BW./vector2.^SF);
CR = 4/5; %Coding rate = 4/5, lowest value so the transmission is the fastest possible
dataRate = floor(bitRate * CR);

%Noise power calculation
% Tuplink = 290; %Kelvins
% Tdownlink = 2340; %Kelvins
% Kb = 1.38*10^-23; %Boltzmann's constant
% noisePowerUplink = 10*log10(Kb*Tuplink*BW)+30; %uplink noise power in dBm
% noisePowerDownlink = 10*log10(Kb*Tdownlink*BW)+30; % downlink noise power in dBm
% SNRdownlinkH400 = receivedPowerH400 - noisePowerDownlink;
% SNRuplinkH400 = receivedPowerH400 - noisePowerUplink;
% SNRdownlinkH450 = receivedPowerH450 - noisePowerDownlink;
% SNRuplinkH450 = receivedPowerH450 - noisePowerUplink;
% SNRdownlinkH500 =receivedPowerH500 - noisePowerDownlink;
% SNRuplinkH500 = receivedPowerH500 - noisePowerUplink;
SNRdownlinkH550 =receivedPowerH550 +116.84;

figure(4)
rP = plot(elevation,receivedPowerH550);
rP.LineWidth = 2.5;
xlabel('Elevation (º)');
ylabel('Received power (dBm)');
title('Received power and SNR for h = 550 km');
hold on;
a=yline(sensitivityPowerSX1262(1),'-.');
a.Color = [1 0 0];
a.LineWidth = 1;
hold on;
a=yline(sensitivityPowerSX1262(2),'-.');
a.Color = [0 1 0];
a.LineWidth = 1;
hold on;
a=yline(sensitivityPowerSX1262(3),'-.');
a.Color = [0 0 1];
a.LineWidth = 1;
hold on;
a=yline(sensitivityPowerSX1262(4),'-.');
a.Color = [1 1 0];
a.LineWidth = 1;
hold on;
a=yline(sensitivityPowerSX1262(5),'-.');
a.Color = [0 1 1];
a.LineWidth = 1;
hold on;
a=yline(sensitivityPowerSX1262(6),'-.');
a.Color = [1 0 1];
a.LineWidth = 1;
hold on;
legend('Received power','Sensitivity with SF=7','Sensitivity with SF=8','Sensitivity with SF=9','Sensitivity with SF=10','Sensitivity with SF=11','Sensitivity with SF=12');
grid;

yyaxis right
%plot(elevation,SNRdownlinkH550,':','color','b');
ylabel('SNR (dB)');
ylim([-21.2 -3.15]);
hold off;

% figure(4)
% rP = plot(elevation,receivedPowerH400);
% rP.LineWidth = 2.5;
% xlabel('Elevation (º)');
% ylabel('Received power (dBm)');
% title('Received power for h = 400 km');
% hold on;
% a=yline(sensitivityPowerSX1262(1),'-.');
% a.Color = [1 0 0];
% a.LineWidth = 1;
% hold on;
% a=yline(sensitivityPowerSX1262(2),'-.');
% a.Color = [0 1 0];
% a.LineWidth = 1;
% hold on;
% a=yline(sensitivityPowerSX1262(3),'-.');
% a.Color = [0 0 1];
% a.LineWidth = 1;
% hold on;
% a=yline(sensitivityPowerSX1262(4),'-.');
% a.Color = [1 1 0];
% a.LineWidth = 1;
% hold on;
% a=yline(sensitivityPowerSX1262(5),'-.');
% a.Color = [0 1 1];
% a.LineWidth = 1;
% hold on;
% a=yline(sensitivityPowerSX1262(6),'-.');
% a.Color = [1 0 1];
% a.LineWidth = 1;
% hold on;
% legend('Received power','Sensitivity with SF=7','Sensitivity with SF=8','Sensitivity with SF=9','Sensitivity with SF=10','Sensitivity with SF=11','Sensitivity with SF=12');
% grid;
% hold off;

% figure(5)
% rP = plot(elevation,receivedPowerH450);
% rP.LineWidth = 2.5;
% xlabel('Elevation (º)');
% ylabel('Received power (dBm)');
% title('Received power for h = 450 km');
% hold on;
% a=yline(sensitivityPowerSX1262(1),'-.');
% a.Color = [1 0 0];
% a.LineWidth = 1;
% hold on;
% a=yline(sensitivityPowerSX1262(2),'-.');
% a.Color = [0 1 0];
% a.LineWidth = 1;
% hold on;
% a=yline(sensitivityPowerSX1262(3),'-.');
% a.Color = [0 0 1];
% a.LineWidth = 1;
% hold on;
% a=yline(sensitivityPowerSX1262(4),'-.');
% a.Color = [1 1 0];
% a.LineWidth = 1;
% hold on;
% a=yline(sensitivityPowerSX1262(5),'-.');
% a.Color = [0 1 1];
% a.LineWidth = 1;
% hold on;
% a=yline(sensitivityPowerSX1262(6),'-.');
% a.Color = [1 0 1];
% a.LineWidth = 1;
% hold on;
% legend('Received power','Sensitivity with SF=7','Sensitivity with SF=8','Sensitivity with SF=9','Sensitivity with SF=10','Sensitivity with SF=11','Sensitivity with SF=12');
% grid;
% hold off;
% 
% 
% figure(6)
% rP = plot(elevation,receivedPowerH500);
% rP.LineWidth = 2.5;
% xlabel('Elevation (º)');
% ylabel('Received power (dBm)');
% title('Received power for h = 500 km');
% hold on;
% a=yline(sensitivityPowerSX1262(1),'-.');
% a.Color = [1 0 0];
% a.LineWidth = 1;
% hold on;
% a=yline(sensitivityPowerSX1262(2),'-.');
% a.Color = [0 1 0];
% a.LineWidth = 1;
% hold on;
% a=yline(sensitivityPowerSX1262(3),'-.');
% a.Color = [0 0 1];
% a.LineWidth = 1;
% hold on;
% a=yline(sensitivityPowerSX1262(4),'-.');
% a.Color = [1 1 0];
% a.LineWidth = 1;
% hold on;
% a=yline(sensitivityPowerSX1262(5),'-.');
% a.Color = [0 1 1];
% a.LineWidth = 1;
% hold on;
% a=yline(sensitivityPowerSX1262(6),'-.');
% a.Color = [1 0 1];
% a.LineWidth = 1;
% hold on;
% legend('Received power','Sensitivity with SF=7','Sensitivity with SF=8','Sensitivity with SF=9','Sensitivity with SF=10','Sensitivity with SF=11','Sensitivity with SF=12');
% grid;
% hold off;

figure(7)
ylim([-30 -5]);
% plot(elevation,SNRuplinkH400,'-','color','r');
% hold on;
% plot(elevation,SNRdownlinkH400,'-','color','b');
% hold on;
% plot(elevation,SNRuplinkH450,'--','color','r');
% hold on;
% plot(elevation,SNRdownlinkH450,'--','color','b');
% hold on;
% plot(elevation,SNRuplinkH500,':','color','r');
% hold on;
% plot(elevation,SNRdownlinkH500,':','color','b');
plot(elevation,SNRdownlinkH550,':','color','b');
title('SNR depending on the elevation angle');
xlabel('Elevation (º)');
ylabel('SNR (dB)');
% legend('SNR uplink for h = 400 km','SNR downlink for h = 400 km','SNR uplink for h = 450 km','SNR downlink for h = 450 km','SNR uplink for h = 500 km','SNR downlink for h = 500 km');
grid;
hold off;
