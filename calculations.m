% Colin Russell
% 08/05/2020
% Ohio University School of EECS
% 
% This series of functions does several tasks:
% 1. Converts Latitude, Longitude, and Altitude/Height(LLH) values from the Ublox GPS
%   Reciever and DJI Drone to ECEF
% 2. Calcuates ENU measurements for the drone with respect to the Ublox reciever.
% 3. Calculates a Slant Distance between the Drone and the Ublox Reciever
% 4. Calculates a Heading Angle (Azimuth clockwise from true north)
% 5. Calcuates the Elevation Angle from the Ublox Reviever to the drone


fid = fopen('UbloxGPSPVTData.csv'); % Opens CSV File containing Ublox Data
getData = textscan(fid, '%d %s %f %f %f', 'HeaderLines', 1, 'Delimiter', ',');
ubloxLLH = horzcat(getData{1,3}, getData{1,4}, getData{1,5});
% ------------------------------------------------------------------
%ECEF Portion
ublox_time = getData{1,2};
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
%ENU Portion - This is the ENU measurement of the Drone with respect to the Ublox
%Reciever
enu = [];
i = 1;
while i <= size(ubloxLLH, 1)
    enu(i, 1:3) = ec2enu([ublox_ecef(i, 1:3)], [drone_ecef(i, 1:3)], [droneLLH(i, 1:3)]); %Ublox Reciever is the origin
    i = i + 1; 
end

%Slant Distance/Slant Range Caluclation - ECEF(XYZ of Drone and Ublox
%Distance Formula Difference)
slantDistance = [];
i = 1;
while i <= size(drone_ecef,1)
    slantDistance(i, 1) = sqrt(((drone_ecef(i,1)- ublox_ecef(i,1))^2)+((drone_ecef(i,2)- ublox_ecef(i,2))^2)+((drone_ecef(i,3)- ublox_ecef(i,3))^2));
    i = i + 1;
end

writematrix(slantDistance, 'slantDistance.txt');

%Heading Calculation
% ------------------------------------------------------------------
heading = [];
i = 1;
while i <= size(enu,1)
    heading(i,1) = wrapTo2Pi(atan2(enu(i, 1), enu(i, 2)))*180/pi; % The result, given in radians, was converted to degrees
    i = i + 1;
end

writematrix(heading, 'heading.txt'); %Creates TXT File with heading info

%Elevation Angle to Drone
elevationAngle = [];
i = 1;
while i <= size(enu,1)
    elevationAngle(i,1) = (wrapTo2Pi(acos(enu(i, 3)/ slantDistance(i,1))))*180/pi/2; % The result, given in radians, was converted to degrees
    % The result is divided by 2 to correct the calcuation 
    % wrapTo2Pi assures that the angles are between 0 and 2pi radians
    i = i + 1;
end
writematrix(elevationAngle, 'elevationAngle.txt') %Creates a TXT File with elevation angle info

fclose('all');