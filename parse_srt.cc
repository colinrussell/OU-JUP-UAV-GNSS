/**
 * @file: parse_srt.cc
 * @author: Colin Russell
 * @date: 7/26/2020
 * @brief: This program parses longitude, latitude, and altitude data from the SRT File of the 
 *         DJI Mavic Pro 2 Drone. This program assumes that altitude is atleast 100 and less than 1000. 
 */

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <string>
using namespace std;

struct Telemetry{
        string date;
        string time;
        double latitude;
        double longitude;
        double altitude;
        double diffTime;    /// Time between each frame, measured in milliseconds
        string timecode; /// Starting duration for each frame for corresponding video
};

void fillVectorFromFile (vector<Telemetry> &data, int &count, ifstream &inputFileStream);
/**
 *  Function:   fillVectorFromFile
 *              Fills the data vector with the date/time, latitude, longitude, and altitude at each given index
 *
 *  @param data - vector passed by reference that will be written with position and time data
 *  @param count - index variable
 *  @param inputFileStream - input file
 */

int main(){
    cout << setprecision(6) << fixed;
    string inputFileName;
    cout << "Enter name of input file: ";
    cin >> inputFileName;
    string outputFileName = "CSV File for " + inputFileName + ".csv";
    vector<Telemetry> droneData;
    ifstream inputFileStream;
    inputFileStream.open(inputFileName);
    if (inputFileStream.fail()){
        cout << "Error opening the input file." << endl;
        exit(0);
    }
    ofstream outputFileStream;
    outputFileStream.open(outputFileName);
    if (outputFileStream.fail()){
        cout << "Error opening output file." << endl;
        exit(0);
    }
    int count = 0;
    fillVectorFromFile(droneData, count, inputFileStream);
    outputFileStream << "TimeCode, Frame, Date, Time, Latitude, Longitude, Altitude" << endl;
    outputFileStream << setprecision(6) << fixed;
    
    /// The for loop below fills the output CSV file.
    for (int i = 0; i < count; ++i){
        outputFileStream << droneData.at(i).timecode << ", " << i + 1 << ", " << droneData.at(i).date << ", " << droneData.at(i).time << ", "
        << droneData.at(i).latitude << ", " << droneData.at(i).longitude << ", " << droneData.at(i).altitude << endl;
    }
    cout << "The output file compiled successfully." << endl;
    inputFileStream.close();
    outputFileStream.close();
    cout << droneData.size() << endl;
    return 0;
}

void fillVectorFromFile (vector<Telemetry> &data, int &count, ifstream &inputFileStream){
    string temp;
    string tcode;
    string month, day, year, timeWithHourMinuteSecond, fractionOfSecondString, dateModified, timeModified; /// The format in which the date and time are presented is modified
    double thousandthSecond;
    string latitudeString, longitudeString, altitudeString;
    /// These 3 strings are substrings of the string containing the line of the SRT file where the data is located
    Telemetry entry; /// Represents a blank entry to push back the vector droneData
    while (getline(inputFileStream, temp)){
        if (temp.length() == 29){
            data.push_back(entry);
            tcode = temp.substr(0,8) + "." + temp.substr(9,3);
            data.at(count).timecode = tcode;
        }
        if (temp.length() == 27){           /// gets the date and time
            year = temp.substr(0,4);
            month = temp.substr(5, 2);
            day = temp.substr(8, 2);
            timeWithHourMinuteSecond = temp.substr(11, 8);
            fractionOfSecondString = temp.substr(20, 3) + temp.substr(24,3);
            thousandthSecond = stoi(fractionOfSecondString) / 1000000.0;
            timeModified = timeWithHourMinuteSecond + to_string(thousandthSecond).substr(1,7);
            dateModified = month + "/" + day + "/" + year;
            data.at(count).date = dateModified;
            data.at(count).time = timeModified;
        }
        if (temp.length() == 186){          /// gets lat, long, and alt
            latitudeString = temp.substr(119, 9);
            data.at(count).latitude = stod(latitudeString);
            longitudeString = temp.substr(144, 10);
            data.at(count).longitude = stod(longitudeString);
            altitudeString = temp.substr(167, 10);
            data.at(count).altitude = stod(altitudeString);
            count++;
        }
    }
}