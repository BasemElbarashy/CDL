#include "Driver.h"
#include <algorithm>
#include <ctime>
#include <string>         // std::string
#include <bitset>         // std::bitset
#include "Utilities.h"

Driver:: Driver(){/*ctor*/}
Driver::~Driver(){/*dtor*/}

//set hyperparameters for all sequences
void setHyperParameters(int maxNumOfNCDs,int maxNumCycle,int maxNumLTMSavedSequences,int maxNumValueConditions,
                        int maxNumAssignments,int maxNumPush,int maxNumSolve,
                        bool isNegativeAssignmentsAllowed);
//set hyperparameters for a specific sequence
void setHyperParameters(int sequenceNo,int maxNumOfNCDs,int maxNumCycle,int maxNumLTMSavedSequences,int maxNumValueConditions,
                        int maxNumAssignments,int maxNumPush,int maxNumSolve,
                        bool isNegativeAssignmentsAllowed);
int main()
{
    IOHandler IOH;
    Tester TE;
    /// 8 HyperParameters :
    /// NCDs, Cycles, SavedSequences, condition, assignment, Push, solve,isAssignBeforeAllowed

    ///--------------------- Group A Experiment ---------------------//
    IOH.loadDatasetsAndQuestionsFromFile("dataset/Group_A.dat");
    Constants::redirectStdoutToOutputFile = true;  //redirect stdout to outputFile 
                                                   //The detailed results will be saved in OutputFile.txt
    setHyperParameters(1,2,1,2,7,0,0,1);    //set hyperparamters for the 25 dataset in file Group_A.dat
    TE.learnAndTest(0);                      //learn & test all Datasets in file Group_A.dat
    //*/
    ///--------------------- Group B Experiment ---------------------//
    /*
    IOH.loadDatasetsAndQuestionsFromFile("dataset/Group_B.dat");
    Constants::redirectStdoutToOutputFile = true;

    setHyperParameters(   1,0,0,2,7,1,1,0);  //set hyperparamters for all sequenecs
    setHyperParameters(6, 1,2,0,2,7,1,1,0);  //set hyperparamters sequence no 6
    TE.learnAndTest(3);                      //learn & test Dataset 3 in file Group_B.dat

    setHyperParameters(   1,0,0,3,6,0,2,0);
    setHyperParameters(1, 1,2,1,3,7,0,2,0);
    setHyperParameters(11,1,0,0,2,4,6,2,0);
    TE.learnAndTest(2);

    setHyperParameters(   1,0,0,2,4,4,2,1);
    TE.learnAndTest(0);
    TE.learnAndTest(1);
    TE.learnAndTest(4);
    //*/
    ///--------------------- Group C Experiment ---------------------//
    /*
    IOH.loadDatasetsAndQuestionsFromFile("dataset/Group_C.dat");
    Constants::redirectStdoutToOutputFile = true;
    setHyperParameters(1,2,0,2,7,0,0,1);

    TE.learnAndTest();
    //*/
    ///--------------------------------------------------------------//
    return 0;
}


void setHyperParameters(int maxNumOfNCDs,int maxNumCycle,int maxNumLTMSavedSequences,int maxNumValueConditions,
                        int maxNumAssignments,int maxNumPush,int maxNumSolve,
                        bool isNegativeAssignmentsAllowed)
{
    int i,nTrainingLevels=1;
    if(maxNumSolve>0)    nTrainingLevels=2;

    for(i=0;i<Constants::MAX_NUM_ACTIONS;i++)
    {
        Constants::HyperParameters[i][0] = maxNumOfNCDs;
        Constants::HyperParameters[i][1] = maxNumCycle;
        Constants::HyperParameters[i][2] = maxNumLTMSavedSequences;
        Constants::HyperParameters[i][3] = maxNumValueConditions;
        Constants::HyperParameters[i][4] = maxNumAssignments;
        Constants::HyperParameters[i][5] = nTrainingLevels;
        Constants::HyperParameters[i][6] = maxNumPush;
        Constants::HyperParameters[i][7] = maxNumSolve;
        Constants::HyperParameters[i][8] = isNegativeAssignmentsAllowed;
    }
}

void setHyperParameters(int sequenceNo,int maxNumOfNCDs,int maxNumCycle,int maxNumLTMSavedSequences,int maxNumValueConditions,
                        int maxNumAssignments,int maxNumPush,int maxNumSolve,
                        bool isNegativeAssignmentsAllowed)
{
    int nTrainingLevels=1;
    if(maxNumSolve>0)    nTrainingLevels=2;

    Constants::HyperParameters[sequenceNo][0] = maxNumOfNCDs;
    Constants::HyperParameters[sequenceNo][1] = maxNumCycle;
    Constants::HyperParameters[sequenceNo][2] = maxNumLTMSavedSequences;
    Constants::HyperParameters[sequenceNo][3] = maxNumValueConditions;
    Constants::HyperParameters[sequenceNo][4] = maxNumAssignments;
    Constants::HyperParameters[sequenceNo][5] = nTrainingLevels;
    Constants::HyperParameters[sequenceNo][6] = maxNumPush;
    Constants::HyperParameters[sequenceNo][7] = maxNumSolve;
    Constants::HyperParameters[sequenceNo][8] = isNegativeAssignmentsAllowed;
}
