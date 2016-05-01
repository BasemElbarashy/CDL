#ifndef GENERATOR_H
#define GENERATOR_H

#include <vector>
#include "CD.h"
#include "Utilities.h"
#include <utility>
#include <set>
#include <bitset>

class Generator
{
    public:
        Generator();
        void learnActions(const vector< vector<int> > &actions);
        void generatePCDsOfAction(int &actionToLearnIdx,int nextNodeIndex,int callNo,bool isNormalMode,int firstVarAssign,int firstVarCond,vector<CD> &MGICDs); /**/
        void generateVCDsOfActions(int call_no ,vector<int> tempHCD , vector<int> accumaltedCDARow,int NCDStartIndex,int PCDIndex,int numOfWA);
        void generateMGICDsOfActions(int call_no,int gamma,bool isSingleOption,bitset<Constants::MAX_NUM_ACTIONS> solvedActions = bitset<Constants::MAX_NUM_ACTIONS>().reset());
        int  calculateGamma();
        void prepareActions(const vector< vector<int> > &actions);
        void clearSomeVariables();
        vector< vector<int> > getPCDA();
        inline bool testAndSaveCD(CD &newCD,vector<CD> &MGICDs,int &actionToLearnIdx,bool isPredefinedUnused);
        int getnNodesInMGICDs();
        vector<CD> getPCDs();
        vector<CD> getMGICDs();
        vector<bitset<Constants::MAX_NUM_ACTIONS> > getVCDA();
        vector<vector<int> > getVCDs();
        vector<int> getLTM();
        vector<int> getLAS();
        vector<int> getLAE();

        int getnPCDGeneratorCalls();
        int getnMGICDGeneratorCalls();
        int getnVPCDs();
        int getgamma();
        float getPCDsGenerationTime();
        float getVCDsGenerationTime();
        float getMGICDsGenerationTime();
        int getNumOfPCDs();
        int getNumOfVCDs();

        virtual ~Generator();
    protected:
    private:
    //------------------------------------------------------- CDs
    vector<CD>            MGICDs;           //final model
    vector<bitset<Constants::MAX_NUM_ACTIONS> > MGICDsVCDA;
    vector<bitset<Constants::MAX_NUM_ACTIONS> > previousMGICDsVCDA;
    vector<CD>            PCDs;
    vector< vector<int> > PCDA;
    vector<vector<int> >  VCDs;             //indices of VCDs in PCDs
    vector<bitset<Constants::MAX_NUM_ACTIONS> > VCDA;
    map<vector<int>,pair<int,int> > permutations; //(permutaion ,(idx in PCDs,NumOfNodes) )
    int nNodesInMGICDs,nActions;
    //-------------------------------------------------------
    vector<int> LTM;
    vector<int> LAS;  //LTM Action Start
    vector<int> LAE;  //LTM Action End
    vector<vector<int> > LTMPlusX;
    vector<vector<int> > LASPlusX;
    vector<vector<int> > LAEPlusX;
    vector<vector<int> > Xs;
    vector<int> Ys;
    //--------------------------------------------------------
    int nPCDGeneratorCalls;
    int nMGICDGeneratorCalls;
    int nVPCDs;
    int gamma;
    float PCDsGenerationTime;     //in seconds
    float VCDsGenerationTime;     //in seconds
    float MGICDsGenerationTime;   //in seconds
};

#endif // GENERATOR_H
