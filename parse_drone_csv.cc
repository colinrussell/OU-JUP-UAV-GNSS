/**
 * @file: parse_drone.cc
 * @author: Colin Russell
 * @date: 08/06/2020
 * @brief: This program parses the DJI drone observation platform's GPS sensor telemetry data found in the CSV File
 * that was compiled with my program: parse_srt.cc into a KML File to be used with Google Earth.
 * This is intended to be used with "parse_ublox_csv.cc", "parse_srt.cc", and "calculations.m".
 */

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <string>
using namespace std;

struct Telemetry{
    string time;
    string dateYMD; /// Date formatted as YYYY-MM-DD
    string latitude;
    string longitude;
    string altitude;
    string when;    /// KML When contains yyyy-mm-ddThh:mm:ss.xxxZ
    string coord;   /// KML Coord is arranged as "longitude" "latitude" "altitude"
    double elevationAngle;
    double declineAngle;
    double heading; /// Heading where the Ublox GNSS is the Origin in relation to the Drone
    double slantDistance;
    string angles; /// gx angles for gx tour in KML
};

void loadVector(vector<Telemetry> &data, ifstream &inputFileStream);

void fillKMLFile(vector<Telemetry> &data, ofstream &outs);

int commaIndexPlus1(int commaIndex);

int main(){
    cout << setprecision(6) << fixed;
    string inputFileName = "DJI_0071.SRT Epic-by-Epic.csv";
    string outputFileName = "KML File for " + inputFileName + ".kml";
    vector <Telemetry> droneData;
    ifstream inputFileStream;
    inputFileStream.open(inputFileName);
    if (inputFileStream.fail()){
        cout << "Error opening the input file." << endl;
        exit(0);
    }
    loadVector(droneData, inputFileStream);
    ofstream outputKMLFile;
    outputKMLFile.open(outputFileName);
    if(outputKMLFile.fail()){
        cout << "Error opening the output file." << endl;
        exit(0);
    }
    fillKMLFile(droneData, outputKMLFile);
    inputFileStream.close(); outputKMLFile.close();
    return EXIT_SUCCESS;
}

void loadVector(vector<Telemetry> &data, ifstream &inputFileStream){
    ifstream inputElevationAngle;
    inputElevationAngle.open("MATLAB/elevationAngle.txt");
    if(inputElevationAngle.fail()){
        cout << "Error opening the elevation angle input file." << endl;
        exit(0);
    }
    ifstream inputHeading;
    inputHeading.open("MATLAB/heading.txt");
    if (inputElevationAngle.fail()) {
        cout << "Error opening the heading input file." << endl;
        exit(0);
    }
    ifstream inputSlantDistance;
    inputSlantDistance.open("MATLAB/slantDistance.txt");
    if (inputSlantDistance.fail()) {
        cout << "Error opening the slant distance input file." << endl;
        exit(0);
    }
    string temp;
    Telemetry entry; /// Represents a blank entry to push back the vector "data"
    int dataSize = 0;
    int commaIndex = 0;
    int hour = 0;
    while (getline(inputFileStream, temp)){
        if (temp.length() < 69){ /// If the length of the temp string is less than 58, the title entries are shown.
                                 /// This assumes that the order is "TimeCode, Frame, DiffTime, Date, Time, Latitude, Longitude, Altitude"
            getline(inputFileStream, temp);
        }
        data.push_back(entry);
        commaIndex = temp.find(", ");
        commaIndex = temp.find(", ", commaIndexPlus1(commaIndex));
        commaIndex = temp.find(", ", commaIndexPlus1(commaIndex));
        data.at(dataSize).dateYMD = temp.substr(commaIndex + 2, 10);
        commaIndex = temp.find(", ", commaIndexPlus1(commaIndex));
        hour = stoi(temp.substr(commaIndex + 2, 2)) + 4;  /// Adjusts hour for UTC - 4 hours
        data.at(dataSize).time = to_string(hour) + temp.substr(commaIndex + 4, 13);
        commaIndex = temp.find(", ", commaIndexPlus1(commaIndex));
        data.at(dataSize).latitude = (temp.substr(commaIndex + 2, 9));
        commaIndex = temp.find(", ", commaIndexPlus1(commaIndex));
        data.at(dataSize).longitude = (temp.substr(commaIndex + 2, 9));
        commaIndex = temp.find(", ", commaIndexPlus1(commaIndex));
        data.at(dataSize).altitude = (temp.substr(commaIndex + 2, 9));
        dataSize++;
    }
    for (int i = 0; i < data.size(); ++i){ /// Creates when and coord entries for use with the KML File
        data.at(i).when = "<when>" + data.at(i).dateYMD + "T" + data.at(i).time + "Z</when>";
        data.at(i).coord = "<gx:coord>" + (data.at(i).longitude) + " " + (data.at(i).latitude) + " " + (data.at(i).altitude) + "</gx:coord>";
    }
    for (int i = 0; i < data.size() && getline(inputElevationAngle, temp); ++i){
        data.at(i).elevationAngle = stod(temp);
        data.at(i).declineAngle = 90 - data[i].elevationAngle;
    }
    for (int i = 0; i < data.size() && getline(inputHeading, temp); ++i) {
        data.at(i).heading = stod(temp);
    }
    for (int i = 0; i < data.size() && getline(inputSlantDistance, temp); ++i){
        data.at(i).slantDistance = stod(temp);
    }
inputElevationAngle.close();
    inputHeading.close();
    inputSlantDistance.close();
}

void fillKMLFile(vector<Telemetry> &data, ofstream &outs){
    outs << "<?xml version=" << '"' << "1.0" << '"' << " encoding=" << '"' << "UTF-8" << '"' << "?>" << endl
         << "<kml xmlns= " << '"' << "http:/" << "/www.opengis.net/kml/2.2" << '"' << " xmlns:gx=" << '"' << "http:/" << "/www.google.com/kml/ext/2.2" << '"' << '>' << endl;
    outs << "    <LookAt>" << endl << "        <gx:TimeSpan>" << endl << "            <begin>" << data.at(0).dateYMD + "T" + data.at(0).time + "Z" << "</begin>" << endl
         << "            <end>" << data.at(data.size()-1).dateYMD + "T" + data.at(data.size()-1).time + "Z" << "</end>" << endl << "        </gx:TimeSpan>" << endl
         << "            <longitude>" << data.at(0).longitude << "</longitude>" << endl
         << "            <latitude>" << data.at(0).latitude << "</latitude>" << endl
         << "            <tilt>" << data.at(0).elevationAngle << "</tilt>" << endl /// This is the tilt angle - https://developers.google.com/kml/documentation/cameras
         << "            <heading>" << data.at(0).heading << "</heading>" << endl
         << "            <range>" << data.at(0).slantDistance << "</range>" << endl
        //<< "           <range>" << 2000 /****Replace this number */<< "</range>" << endl
         << "    </LookAt>" << endl;
        /// Information above includes the beginning of the XML KML file, the document infomation, and the LookAt Information
    outs << "    <Style id=" << '"' << "sh_movies" << '"' << '>' << endl
         << "        <IconStyle>" << endl
         << "            <scale>1.4</scale>" << endl
         << "            <Icon><href>http:/" << "/earth.google.com/images/kml/shapes/movies.png</href></Icon>" << endl
         << "hotSpot x=" << '"' << "0.5" << '"' << " y =" << '"' << "0" << '"' << " xunits=" << '"' <<"fraction" << '"' << " yunits=" << '"' <<"fraction" << '"' << "/>" << endl
        << "        </IconStyle>" << endl
        << "      <LineStyle><color>ff0880fd</color><width>4</width></LineStyle>" << endl
        << "    </Style>" << endl;

    outs << "    <StyleMap id = " << '"' << "msn_movies" << '"' << '>' <<  endl    /// This style info is a StyleMap to activate the style for the movie icon
         << "        <Pair>" << endl
         << "            <key>normal</key>" << " <styleUrl>#sn_movies</styleUrl>" << endl
         << "        </Pair>" << endl
         << "        <Pair>" << endl
         << "            <key>highlight</key>" << " <styleUrl>#sh_movies</styleUrl>" << endl
         << "        </Pair>" << endl
         << "    </StyleMap>" << endl;

    outs << "    <Style id=" << '"' << "sn_movies" << '"' << '>' << endl
        << "        <IconStyle>" << endl
        << "            <scale>1.2</scale>" << endl
        << "            <Icon><href>http:/" << "/earth.google.com/images/kml/shapes/movies.png</href></Icon>" << endl
        << "            <hotSpot x=" << '"' << "0.5" << '"' << " y =" << '"' << "0" << '"' << " xunits=" << '"' <<"fraction" << '"' << " yunits=" << '"' <<"fraction" << '"' << "/>" << endl
        << "        </IconStyle>" << endl
        << "      <LineStyle><color>ff0880fd</color><width>4</width></LineStyle>" << endl
        << "    </Style>" << endl;

    outs << "    <Placemark>" << endl
         << "        <name>Observation Platform</name>" << endl
         << "        <Snippet>" << "</Snippet>" << endl
         << "        <styleUrl>#msn_movies</styleUrl>" << endl
        << "        <gx:balloonVisibility>0</gx:balloonVisibility>"
        << "        <gx:Track>" << endl;
    for (int i = 0; i < data.size(); ++i){
        outs << "            " << data.at(i).when << endl;
    }
    for (int i = 0; i < data.size(); ++i) {
        outs << "            " << data.at(i).coord << endl;
    }
    for (int i = 0; i < data.size(); ++i) {
        data.at(i).angles = "<gx:angles> " + to_string(data.at(i).heading) + " " + to_string(data.at(i).elevationAngle) + " 0 </gx:angles>";
        outs << "            " << data.at(i).angles << endl;
    }
    outs << "        </gx:Track>" << endl
         << "        </Placemark>" << endl;
    outs << "</kml>";
}

int commaIndexPlus1(int commaindex){
    return commaindex + 1;
}