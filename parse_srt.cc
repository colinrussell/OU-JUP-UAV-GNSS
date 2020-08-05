/**
 * @file: parse_srt.cc
 * @author: Colin Russell
 * @date: 08/04/2020
 * @brief: This program parses longitude, latitude, and altitude data from the SRT File of a 
 *         DJI drone. This program assumes that altitude is atleast 100 and less than 1000.
 *         Two output files are created. One file will contain each telemetry entry. The other file
 *         will contain one entry of telemetry for each second, which is known as epic by epic.
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
        int second;
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

void fillVectorwithOneSecondDurationCounter(int sourceCount, int &oneSecondIndex, vector<Telemetry> &data, vector<Telemetry> &sourceData);
/**
 *  Function:   fillVectorwithOneSecondDurationCounter
 *              Fills the data vector with the date/time, latitude, longitude, and altitude for each second
 *
 *  @param sourceCount - index variable for droneData Vector
 *  @param oneSecondIndex - a new index that is created to represent the amount of entries for the epic-by-epic vector * 
 *  @param data- vector passed by reference that will be written with position and time data for each second
 *  @param sourceData- vector that contains reference data
 */
void fillOutputFile(int count, vector<Telemetry> &droneData, string outsFileName);
/**
 *  Function:   fillOutputFile
 *              Fills the output file with one entry per frame
 *
 *  @param count - index variable for droneData Vector
 *  @param droneData - vector that contains reference data
 *  @param outsFileName - string containing the name for the output file
 */
void fillOutputEpicByEpicFile(int oneSecondIndex, vector<Telemetry> &droneDataPerSecond, string outsFileName);
/**
 *  Function:   fillOutputEpicByEpicFile
 *              Fills the output file with one entry per second
 *
 *  @param oneSecondIndex - index variable for droneDataPerSecond Vector
 *  @param droneDataPerSecond - vector that contains reference data
 *  @param outsFileName - string containing the name for the output file
 */

int main(){
    cout << setprecision(6) << fixed;
    string inputFileName;
    cout << "Enter name of input file: ";
    cin >> inputFileName;
    string outputFileName = "CSV File for " + inputFileName + ".csv";
    string outsEpicByEpicFileName = inputFileName + " Epic-by-Epic.csv";
    vector<Telemetry> droneData;
    vector<Telemetry> droneDataPerSecond;
    ifstream inputFileStream;
    inputFileStream.open(inputFileName);
    if (inputFileStream.fail()){
        cout << "Error opening the input file." << endl;
        exit(0);
    }
    int count = 0;
    fillVectorFromFile(droneData, count, inputFileStream);
    int oneSecondIndex = 0;
    fillVectorwithOneSecondDurationCounter(count, oneSecondIndex, droneDataPerSecond, droneData);  /// count is the amount of index values / total frames from the original SRT File
    /// One second index will be used in the same way count was used for indexing the primary vector, droneData, which will be called sourceData in the  fillVectorwithOneSecondDurationCounter function
    fillOutputFile(count, droneData, outputFileName);
    fillOutputEpicByEpicFile(oneSecondIndex, droneDataPerSecond, outsEpicByEpicFileName);
    cout << "Both files have compiled successfully." << endl;
    inputFileStream.close();
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
            data.at(count).second = stoi(temp.substr(17, 2));
            fractionOfSecondString = temp.substr(20, 3) + temp.substr(24,3);
            thousandthSecond = stoi(fractionOfSecondString) / 1000000.0;
            timeModified = timeWithHourMinuteSecond + to_string(thousandthSecond).substr(1,7);
            dateModified = month + "/" + day + "/" + year;
            data.at(count).date = dateModified;
            data.at(count).time = timeModified;
            getline(inputFileStream, temp);
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

void fillVectorwithOneSecondDurationCounter(int sourceCount, int &oneSecondIndex, vector<Telemetry> &data, vector <Telemetry> &sourceData){
    Telemetry entry;
    for (int i = 1; i < sourceCount; ++i){
        int indexMinus1 = i - 1;
        if((sourceData.at(i).second == 0) && ((sourceData.at(indexMinus1).second == 59) || i == 1)){
            data.push_back(entry);
            data.at(oneSecondIndex).altitude = sourceData.at(i).altitude;
            data.at(oneSecondIndex).date = sourceData.at(i).date;
            data.at(oneSecondIndex).latitude = sourceData.at(i).latitude;
            data.at(oneSecondIndex).longitude = sourceData.at(i).longitude;
            data.at(oneSecondIndex).second = sourceData.at(i).second;
            data.at(oneSecondIndex).time = sourceData.at(i).time;
            data.at(oneSecondIndex).timecode = sourceData.at(i).timecode;
            oneSecondIndex++;
        }
        if ((sourceData.at(i).second > sourceData.at(indexMinus1).second) || ((sourceData.at(i).second == 59) && (sourceData.at(indexMinus1).second == 58))){
            data.push_back(entry);
            data.at(oneSecondIndex).altitude = sourceData.at(i).altitude;
            data.at(oneSecondIndex).date = sourceData.at(i).date;
            data.at(oneSecondIndex).latitude = sourceData.at(i).latitude;
            data.at(oneSecondIndex).longitude = sourceData.at(i).longitude;
            data.at(oneSecondIndex).second = sourceData.at(i).second;
            data.at(oneSecondIndex).time = sourceData.at(i).time;
            data.at(oneSecondIndex).timecode = sourceData.at(i).timecode;
            oneSecondIndex++;
        }
    }
}

void fillOutputFile(int count, vector<Telemetry> &droneData, string outsFileName){
    ofstream outputFileStream;
    outputFileStream.open(outsFileName);
    if (outputFileStream.fail())
    {
        cout << "Error opening output file." << endl;
        exit(0);
    }
    outputFileStream << "TimeCode, Frame, Date, Time, Latitude, Longitude, Altitude" << endl;
    outputFileStream << setprecision(6) << fixed;
    /// The for loop below fills the output CSV file.
    for (int i = 0; i < count; ++i)
    {
        outputFileStream << droneData.at(i).timecode << ", " << i + 1 << ", " << droneData.at(i).date << ", " << droneData.at(i).time << ", "
                         << droneData.at(i).latitude << ", " << droneData.at(i).longitude << ", " << droneData.at(i).altitude << endl;
    }
    outputFileStream.close();
}

void fillOutputEpicByEpicFile(int oneSecondIndex, vector<Telemetry> &droneDataPerSecond, string outsFileName)
{
    ofstream outsEpicByEpic;
    outsEpicByEpic.open(outsFileName);
    if (outsEpicByEpic.fail())
    {
        cout << "Error opening epic-by-epic output file." << endl;
        exit(0);
    }
    outsEpicByEpic << "Time, Latitude, Longitude, Altitude" << endl;
    outsEpicByEpic << setprecision(6) << fixed;
    /// The for loop below fills the output CSV file.
    for (int i = 0; i < oneSecondIndex; ++i)
    {
        outsEpicByEpic << droneDataPerSecond.at(i).time << ", "
                        << droneDataPerSecond.at(i).latitude << ", " << droneDataPerSecond.at(i).longitude << ", " << droneDataPerSecond.at(i).altitude << endl;
    }
    outsEpicByEpic.close();
}