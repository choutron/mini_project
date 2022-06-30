#include<fstream>
#include<sstream>
#include<vector>

using namespace std;

int numOfDataPoints = 0;
int numOfSensors = 0;

class dataPoint{
    public:
        int id;
        string timestamp;
        float speed;
        bool isOutlier;

    //constructor
    dataPoint(){
        id = 0;
        timestamp = "";
        speed = 0;
        isOutlier = 0;
    }
};

dataPoint *ptr[1000] = {NULL};

class sensor{
    public:
        float maxSpeed;
        float minSpeed;
        float meanSpeed;
        string timestamp_min;
        string timestamp_max;
        int increment;
        int decrement;
        vector<float> speeds;

    sensor(){
        maxSpeed = 0.0;
        minSpeed = 0.0;
        meanSpeed = 0.0;
        timestamp_min = "";
        timestamp_max = "";
        increment = 0;
        decrement = 0;
    }
};

sensor *ptr_sensor[100] = {NULL};

string toString(string argv){
    stringstream parser;
    parser.str(argv);
    string x;
    parser >> x;
    return x;
}

void readCSV(int argc, char **argv = NULL){
    string fileName;
    if(argc == 1){
        fileName = "speed_data.csv";
    }   
    else{
        fileName = toString(argv[1]);
    }            

    ifstream fin(".\\data\\" + fileName);
    if(fin.good()){
        cout << "Opened " << fileName << " successfully." << "\n";
        string line;

        getline(fin, line); //ignore 1st line of file
        //loop throuh the file
        int i = 0;
        while(getline(fin, line)){
            ptr[i] = new dataPoint; //create new dataPoint object

            stringstream parser(line);
            string token;
            //save data into object
            getline(parser, token, ',');
            ptr[i]->id = stoi(token);
            getline(parser, token, ',');
            ptr[i]->timestamp = token;
            getline(parser, token, ',');
            ptr[i]->speed = stof(token);

            numOfDataPoints++;  //count number of data points
            if(numOfSensors < ptr[i]->id){  //count number of sensors
                numOfSensors = ptr[i]->id;
            }
            i++;
        }

        fin.close();
    }
    else{
        cout << "Error: Could not open " << fileName << ".\n";
    }
    
}
//---------------------------------------------START OF TASK 2.1-------------------------------------------
//mark the outlier data point and count the number of outlier data points
int outlierFinder(){
    int numOfOutliers = 0;
    //loop through the data point
    for(int i = 0; i < numOfDataPoints; i++){
        if(ptr[i]->speed < 900 || ptr[i]->speed > 1600){
            ptr[i]->isOutlier = 1;
            numOfOutliers++;   //count the number of outliers
        }
    }
    return numOfOutliers;
}

void outlierCSV(){
    ofstream fout(".\\data\\outlier_data.csv");
    if(fout.good()){
        cout << "Opened outlier_data.csv successfully." << "\n";
        fout << "number of outliers: " << outlierFinder() << "\n" << "id,time,values" << "\n";

        int i = 0;
        while(ptr[i]){
            if(ptr[i]->isOutlier == 1){
                fout << ptr[i]->id << "," << ptr[i]->timestamp << "," << ptr[i]->speed << "\n";
            }
            i++;
        }
    }
    else{
        cout << "Error: Could not open outlier_data.csv" << "\n";
    }
}
//--------------------------------------------END OF TASK 2.1-----------------------------------------------
//-------------------------------------------START OF TASK 2.2----------------------------------------------
string getSamplingTime(){
    //assume that the minimum sampling time is 1 minute
    int sHour, sMin; //starting time
    int eHour, eMin;  //end time
    
    //get starting hour, minute in integer form
    stringstream parser(ptr[0]->timestamp); //timestamp of first data point
    string token;
    getline(parser, token, ':');
    sHour = stoi(token);
    getline(parser, token, ':');
    sMin = stoi(token);

    //get end hour, minute in integer form
    parser.clear(); //clear all bad bit of the stream
    parser.str(ptr[numOfDataPoints - 1]->timestamp);   //timestamp of last data point
    getline(parser, token, ':');
    eHour = stoi(token);
    getline(parser, token, ':');
    eMin = stoi(token);

    int samplingHour, samplingMin;

    if(eHour < sHour){
        samplingHour = 24 - sHour + eHour;
    }
    else{
        samplingHour = eHour - sHour;
    }
    if(eMin < sMin){
        samplingMin = 60 - sMin + eMin;
        samplingHour--;
    }
    else{
        samplingMin = eMin - sMin;
    }

    token.clear();
    token = to_string(samplingHour) + ":" + to_string(samplingMin) + ":00";
    return token;
}

//find out min, max and mean value of each sensor/motor
void minMaxMean(){
    for(int i = 0; i < numOfSensors; i++){
        ptr_sensor[i] = new sensor; //create new sensor object
        ptr_sensor[i]->maxSpeed = 0;
        ptr_sensor[i]->minSpeed = 999999999;
        float sum = 0;
        for(int k = 0; k < numOfDataPoints; k++){
            if((ptr[k]->id == (i+1)) && (ptr[k]->isOutlier == 0)){
                if(ptr[k]->speed > ptr_sensor[i]->maxSpeed){
                    ptr_sensor[i]->maxSpeed = ptr[k]->speed;
                    ptr_sensor[i]->timestamp_max = ptr[k]->timestamp;
                }
                if(ptr[k]->speed < ptr_sensor[i]->minSpeed){
                    ptr_sensor[i]->minSpeed = ptr[k]->speed;
                    ptr_sensor[i]->timestamp_min = ptr[k]->timestamp;
                }

                ptr_sensor[i]->speeds.push_back(ptr[k]->speed); //save all the valid speed for later use
                sum = sum + ptr[k]->speed;
            }
        }
        //calculate the mean speed
        ptr_sensor[i]->meanSpeed = sum / ptr_sensor[i]->speeds.size();
    }
}

void summaryCSV(){
    minMaxMean();
    ofstream fout(".\\data\\data_summary.csv");
    if(fout.good()){
        cout << "Opened data_summary.csv successfully \n";

        string samplingTime = getSamplingTime();

        fout << "id,parameters,time,values\n";
        for(int i = 0; i < numOfSensors; i++){
            fout << i+1 << ",max," << ptr_sensor[i]->timestamp_max << "," << ptr_sensor[i]->maxSpeed << "\n";
            fout << i+1 << ",min," << ptr_sensor[i]->timestamp_min << "," << ptr_sensor[i]->minSpeed << "\n";
            fout << i+1 << ",mean,"<< samplingTime << "," << ptr_sensor[i]->meanSpeed << "\n";
        }
    }
    else{
        cout << "Error: Could not open data_summary.csv";
    }
}
//----------------------------------------------END OF TASK 2.2----------------------------------------------
//---------------------------------------------START OF TASK 2.3---------------------------------------------
void calculateSpeedVariations(){
    for(int i = 0; i < numOfSensors; i++){
        for(auto k = ptr_sensor[i]->speeds.begin(); k != (ptr_sensor[i]->speeds.end() - 1); k++){
            if((*k - *(k+1)) < -100){
                ptr_sensor[i]->increment++;
            }
            if((*k - *(k+1)) > 100){
                ptr_sensor[i]->decrement++;
            }
        }    
    }
}

void statisticsCSV(){
    calculateSpeedVariations();
    ofstream fout(".\\data\\data_statistics.csv");
    if(fout.good()){
        cout << "Opened data_statistics.csv successfully \n";

        fout << "id,direction,frequency\n";
        for(int i = 0; i < numOfSensors; i++){
            fout << i+1 << ",increment," << ptr_sensor[i]->increment << "\n";
            fout << i+1 << ",decrement," << ptr_sensor[i]->decrement << "\n";
        } 
    }
    else{
        cout << "Error: Could not open data_statistics.csv \n";
    }
}
//---------------------------------------------END OF TASK 2.3-----------------------------------------------

void deleteObj(){
    for(int i = 0; i < numOfSensors; i++){
        delete ptr_sensor[i];
    }
    for(int i = 0; i < numOfDataPoints; i++){
        delete ptr[i];
    }
}