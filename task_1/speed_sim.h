#include<sstream>
#include<string>
#include<ctime>
#include<cstdlib>
#include<fstream>

using namespace std;

#define default_n 1     //sensor(s)
#define default_st 10   //seconds
#define default_si 60   //minutes

int n, st, si;
int *ptr_n = &n;
int *ptr_st = &st;
int *ptr_si = &si;
int prevHour, prevMin, prevSec;

int toInt(string argv){
    stringstream parser;
    parser.str(argv);
    int x;
    parser >> x;
    if(parser.fail()) return -1;    //if fails to convert string to int
    else    return x;
}

string toString(string argv){
    stringstream parser;
    parser.str(argv);
    string x;
    parser >> x;
    return x;
}

string toString(int h, int m, int s){
    stringstream ss;
    ss << h << ':' << m << ':' << s;
    string timestamp;
    ss >> timestamp;
    return timestamp;
}

void takeInput(int argc, char **argv){
    //print all args
    for(int i=1; i<argc; i++){
        cout << "arg[" << i << "]:" << argv[i] << endl;
    }

    bool n_exist = 0;
    bool st_exist = 0;
    bool si_exist = 0;
    //parse input
    try{
        for(int i=1; i<argc; i++){
            string temp = toString(argv[i]);
            if(temp == "-n"){
                if(toInt(argv[i+1]) == -1 || toInt(argv[i+1]) < 0)  throw invalid_argument("invalid argument");
                *ptr_n = toInt(argv[i+1]);
                n_exist = 1;
                continue;
            }
            if(temp == "-st"){
                if(toInt(argv[i+1]) == -1 || toInt(argv[i+1]) < 0)  throw invalid_argument("invalid argument");
                *ptr_st = toInt(argv[i+1]);
                st_exist = 1;
                continue;
            }
            if(temp == "-si"){
                if(toInt(argv[i+1]) == -1 || toInt(argv[i+1]) < 0)  throw invalid_argument("invalid argument");
                *ptr_si = toInt(argv[i+1]);
                si_exist = 1;
                continue;
            }
            if(toInt(argv[i]) == -1) throw 22;
           
        }
        //missing parameter is replaced by default value
        if(n_exist == 0)    *ptr_n = default_n;
        if(st_exist == 0)   *ptr_st = default_st;
        if(si_exist == 0)   *ptr_si = default_si;
    }
    catch(...){
        cout << "Error 1.1: Invalid command line argument \n";
    }
}

//generate speed from 0-3000 rpm with res = 0.2 rpm
float generateSpeed(){
    float randomNum;
    randomNum = (rand() % 15000) / 10.0 * 2;
    return randomNum;
}

void getStartingTime(){
    //get starting time as string
    time_t now = time(0);   // current date/time based on current system
    char* dt = ctime(&now); // convert now to string form
    //get only the time, discard date
    stringstream parser;
    parser.str(dt);
    string startingTime;
    for(int i = 0; i<4; i++)    parser >> startingTime;

    //save starting time in integer form
    string token;
    parser.str(startingTime);
    getline(parser, token, ':');
    prevHour = stoi(token);
    getline(parser, token, ':');
    prevMin = stoi(token);
    getline(parser, token, ':');    
    prevSec = stoi(token);
}

void updateTimestamp(){
    //assume that:  minimum sampling time is 1 second
    //              minimum sampling interval is 1 minute

    int h = 0; int m = 0; int s = 0;    //h, m, s is amount of hours, minutes, seconds need to be added to the previous timestamp each time the timestamp is updated
    m = st / 60;
    s = st - m*60;

    if(prevSec + s < 60){
        prevSec = prevSec + s;
    }    
    else{
        m++;
        prevSec = s - (60 - prevSec);
    }
    if(prevMin + m < 60){
        prevMin = prevMin + m;
    }    
    else{
        h++;
        prevMin = m - (60 - prevMin);
    }
    if(prevHour + h < 24){
        prevHour = prevHour + h;
    }   
    else{
        prevHour = 0;
    } 
}

void generateCSV(){
    //get the file name to contain Unix timestamp
    time_t now = time(0);
    string startingTime = to_string(now);
    string fileName = "speed_data_" + startingTime + ".csv";
    ofstream fout(".\\data\\" + fileName);

    /* for testing
    string fileName = "speed_data.csv";
    fstream fout;
    fout.open(fileName, ios::out);  //ios::out makes sure old file is overwritten
    */
   
    if(fout.good()){
        cout << "Opened " << fileName << " successfully \n";
        fout << "id, time, values" << "\n";

        int numOfCycles = (si*60) / st;
        int count = 0;
        while(count <= numOfCycles){
            string timestamp = toString(prevHour, prevMin, prevSec);
            for(int i=1; i < n+1; i++){
                fout << i << "," << timestamp << "," << generateSpeed() << "\n";    
            }
            count++;
            updateTimestamp();
        }
    }
    else{
        cout << "Error 1.5: Unable to save data \n";
    } 
}
