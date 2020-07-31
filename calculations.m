% Colin Russell
% 07/30/2020
% Ohio University School of EECS
% 
% Convert Latitude, Longitude, and Altitude/Height(LLH) values from the Ublox GPS
% Reciever and DJI Drone to ECEF and from ECEF to ENU.

fid = fopen('UbloxGPSPVTData.csv'); % Opens CSV File containing Ublox Data
getData = textscan(fid, '%d %s %f %f %f', 'HeaderLines', 1, 'Delimiter', ',');
ubloxLLH = horzcat(getData{1,3}, getData{1,4}, getData{1,5});
% ------------------------------------------------------------------
%ECEF Portion
ublox_ecef = []; %Array for ECEF Values from the Ublox GPS Reciever
i = 1;
while i <= size(ubloxLLH, 1) % There will be 3 columns
    ublox_ecef(i, 1:3) = llh2ec(ubloxLLH(i,1:3));
    i = i + 1;
end

% This process will read the drone's geographical data into a table and then
% an array
droneTable = readtable('DJI_0071Epic-by-Epic.csv');
droneTable.Time = []; % The time column is be removed
droneLLH = table2array(droneTable); 
droneTable = []; % This table is cleared to save memory

drone_ecef = []; %Array for ECEF Values from the drone
i = 1;

while i <= size(droneLLH, 1)
    drone_ecef(i, 1:3) = llh2ec(droneLLH(i, 1:3));
    i = i + 1;
end
% ------------------------------------------------------------------
%ENU Portion
enu = [];
i = 1;
while i <= size(droneLLH, 1)
    enu(i, 1:3) = ec2enu([ublox_ecef(i, 1:3)], [drone_ecef(i, 1:3)], [droneLLH(i, 1:3)]);
    i = i + 1; 
end

fclose('all');