#include<iostream>
#include "speed_sim.h"

int main(int argc, char **argv){
    srand((unsigned) time(0));  //resets seed for random numb generator to the current time
    takeInput(argc, argv);
    getStartingTime();
    generateCSV();
    return 0;
}