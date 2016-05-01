#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <map>


using namespace std;


class Constants
{
    public:
        Constants();
        virtual ~Constants();

        static const int MAX_KEY_LENGTH = 500;
        static const int NUM_NODE_PARAMETERS = 7;

        static const int UNDEFINED_VAR = -1000000,RIGHT_ANSWER=2, WRONG_ANSWER=3;
        static const int POSITIVE_CD = 0, HYBRID_CD = 1;
        static const int SOURCE_INDEX = 2,SINK_INDEX=0,Z_INDEX=1; //indices in nodes array of CD
        //default indices : source = 2 , sink = 0 , Z = 1  //follow the same order of creating
        //them in constructor of CD

        static const int START_VAR = 0, END_VAR = 1, Z_VAR = 2;   //variables indices in var_sol array
        //negative variables -> constants   [from Key]    ==> -1,-2,-3,....
        //positive variables -> variables   [from Action] ==> 0,1,2,3,.... but 0 and 1 are reserved for start and end variable
        //key: index 1 based ==> all items are positive numbers [ !=0 ]
        //
        static const short ASSIGN_BEFORE = -1, ASSIGN_AFTER = 1,  //both must be +1 and -1
                           COND_TIME = 0, COND_VALUE = 2, SINK = 3,
                           SOURCE = 4,Z_NODE = 5,PUSH = 6,SOLVE = 7; //types of nodes

        static const int MAX_NUM_PCDS=10000,MAX_NUM_VCDS=100000,MAX_NUM_ACTIONS = 50,
                         MAX_NUM_VARIABLES=40,MAX_NUM_ITEMS=40;

        static const int LTM_SEPARATOR = -1; // at the borders of any action in the LTM vector
        static const int FIRST_LEVEL_CD = 0;

        static string keysFilePath, CDsFilePath, CDsVsActionsFilePath, actionsFilePath ,
                          sentencesFilePath,datasetsAndQuestions,animationFilePath;
        //Hyper-parameters
        static int HyperParameters[MAX_NUM_ACTIONS][9];
        static int redirectStdoutToOutputFile;

    protected:
    private:
};

#endif // UTILITIES_H
