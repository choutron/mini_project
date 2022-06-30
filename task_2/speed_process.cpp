#include<iostream>
#include "speed_process.h"

int main(int argc, char **argv){
    readCSV(argc, argv);
    outlierCSV();
    summaryCSV();
    statisticsCSV();
    deleteObj();
    cout << "--------------End of program--------------";
    return 0;
}