/**
 * @file: parse_srt.cc
 * @author: Colin Russell
 * @date: 08/04/2020
 * @brief: This program parses the Ublox GPS sensor's telemetry data found in the CSV File
 * that was compiled with Ucenter into a KML File to be used with Google Earth.
 */

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <string>
using namespace std;

struct Telemetry{
    int index;
    string time;
    string date;
    string dateYMD; /// Date formatted as YYYY-MM-DD
    double latitude;
    double longitude;
    double altitude;
    string when;    /// KML When contains yyyy-mm-ddThh:mm:ss.xxxZ
    string coord;   /// KML Coord is arranged as "longitude" "latitude" "altitude"
};

void loadVector(vector<Telemetry> &data, ifstream &inputFileStream);

void fillKMLFile(vector<Telemetry> &data, ofstream &outs);

int main(){
    cout << setprecision(8) << fixed;
    string inputFileName = "Ublox GPS PVT Data.csv";
    //cout << "Enter the name of input file: ";
    //cin >> inputFileName;
    string outputFileName = "KML File for " + inputFileName + ".kml";
    vector <Telemetry> ubloxData;
    ifstream inputFileStream;
    inputFileStream.open(inputFileName);
    if (inputFileStream.fail()){
        cout << "Error opening the input file." << endl;
        exit(0);
    }
    loadVector(ubloxData, inputFileStream);
    ofstream outputKMLFile;
    outputKMLFile.open(outputFileName);
    if(outputKMLFile.fail()){
        cout << "Error opening the output file." << endl;
        exit(0);
    }
    fillKMLFile(ubloxData, outputKMLFile);
    inputFileStream.close(); outputKMLFile.close();
    return EXIT_SUCCESS;
}

void loadVector(vector<Telemetry> &data, ifstream &inputFileStream){
    string temp;
    Telemetry entry; /// Represents a blank entry to push back the vector "data"
    int dataSize = 0;
    while (getline(inputFileStream, temp)){
        if (temp.length() < 58){ /// If the length of the temp string is less than 58, the title entries are shown.
                                 /// This assumes that the order is "Index,UTC,Lat,Lon,Alt (MSL)"
            getline(inputFileStream, temp);
        }
        if (temp.length() == 60){ /// This accounts for when the index variable is between 0 and 99
            data.push_back(entry);
            data.at(dataSize).index = stoi(temp.substr(0,2));
            data.at(dataSize).time = temp.substr(3, 12);
            data.at(dataSize).date = temp.substr(16, 10);
            data.at(dataSize).latitude = stod(temp.substr(27, 11));
            data.at(dataSize).longitude = stod(temp.substr(39, 12));
            data.at(dataSize).altitude = stod(temp.substr(52, 7));
        }
        if (temp.length() == 61){ /// This accounts for when the index variable is between 100 and 999
            data.push_back(entry);
            data.at(dataSize).index = stoi(temp.substr(0, 3));
            data.at(dataSize).time = temp.substr(4, 12);
            data.at(dataSize).date = temp.substr(17, 10);
            data.at(dataSize).latitude = stod(temp.substr(28, 11));
            data.at(dataSize).longitude = stod(temp.substr(40, 12));
            data.at(dataSize).altitude = stod(temp.substr(53, 7));
        }
        if (temp.length() == 62){ /// This accounts for when the index variable is between 1000 and 9999
            data.push_back(entry);
            data.at(dataSize).index = stoi(temp.substr(0, 4));
            data.at(dataSize).time = temp.substr(5, 12);
            data.at(dataSize).date = temp.substr(18, 10);
            data.at(dataSize).latitude = stod(temp.substr(29, 11));
            data.at(dataSize).longitude = stod(temp.substr(41, 12));
            data.at(dataSize).altitude = stod(temp.substr(54, 7));
        }
        dataSize++;
    }
    for (int i = 0; i < data.size(); ++i){ /// Creates when and coord entries for use with the KML File
        data.at(i).dateYMD = data.at(i).date.substr(6, 4) + "-" + data.at(i).date.substr(0, 2) + "-" + data.at(i).date.substr(3, 2);
        data.at(i).when = "<when>" + data.at(i).dateYMD + "T" + data.at(i).time + "Z</when>";
        data.at(i).coord = "<gx:coord>" + to_string(data.at(i).longitude) + " " + to_string(data.at(i).latitude) + " " + to_string(data.at(i).altitude) + "</gx:coord>";
    }
}

void fillKMLFile(vector<Telemetry> &data, ofstream &outs){
    outs << "<?xml version=" << '"' << "1.0" << '"' << " encoding=" << '"' << "UTF-8" << '"' << "?>" << endl
         << "<kml xmlns= http:/" << "/www.opengis.net/kml/2.2" << '"' << " xmlns:gx=" << '"' << "http:/" << "/www.google.com/kml/ext/2.2" << '"' << endl
         << "    <Document>" << endl
         << "       <name>Ublox Data KML</name>" << endl
         << "    <LookAt>" << endl << "        <gx:TimeSpan>" << endl << "            <begin>" << data.at(0).dateYMD + "T" + data.at(0).time + "Z" << "</begin>" << endl
         << "            <end>" << data.at(data.size()-1).dateYMD + "T" + data.at(data.size()-1).time + "Z" << "</end>" << endl << "    </gx:TimeSpan>" << endl
         << "            <longitude>" << data.at(0).longitude << "</longitude>" << endl
         << "            <latitude>" << data.at(0).latitude << "</latitude>" << endl
         << "            <range>" << 2000 /****Replace this number */<< "</range>" << endl
         << "    </LookAt>" << endl;
        /// Information above includes the beginning of the XML KML file, the document infomation, and the LookAt Information
}