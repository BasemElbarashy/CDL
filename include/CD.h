#ifndef CD_H
#define CD_H
#include <set>
#include "Node.h"
#include <vector>
//#include <Memo.h>
using namespace std;

class CD
{
    public:
        CD();  ///add the first three nodes (Source,Sink,Z) ,set type = POSITIVE_CD
        void addNode(Node newNode);
        void addUpdateNode(Node newNode);
        void removeUpdateNode();
        void removeLastNode();
        void setSourceAction(int sourceAction);
        void setNextNode(int nodeIndex , int nextNodeIndex);
        void setNextNode(int ConditionalParentIndex ,bool cond, int nodeIndex);

        void replaceNode(int NodeIndex,Node newNode);
        void addCDToHCD(CD newCD);
        void removeCDFromHCD();
        void setType(int typeOfCD);
        void addPredefinedLTMVar(int var,int sol);
        void removePredefinedLTMVar(int var);
        void printCD();
        Node getNode(int nodeIndex);
        void setCycleNodeFlag(int NodeIdx,bool flag);
        int getNumOfNodes();
        int getSourceAction();
        int getType();
        vector<int> getGSP();
        set<int> getPredefinedValues();
        int evaluateCD(vector<int> &LTMPlusX, vector<int> &LASPlusX, vector<int> &LAEPlusX,vector<CD> &MGICDs,bool fAnimation,int levelOfCD,bool isTesting);
        int evaluateCD( vector<int> &CDLTM, vector<int> &CDLAS, vector<int> &CDLAE,vector<int> &question,vector<CD> &MGICDs,bool fAnimation,int levelOfCD,bool isTesting);
        int getSolOfVar(int var);
        void resetIsUsedFlags();
        void popGSP();
        void printVarSol();
        void setIsUsed();
        void resetIsUsed();
        bool getIsUsed();
        bool isHighLevel();
        virtual ~CD();
    protected:
    private:
        int type;  //POSITIVE_CD <default> or HYBRID_CD
        vector<Node> nodes;  //PCD nodes
        vector<int> inEdgeNode;
        int mSourceAction;
        vector<CD> CDsOfHCD; //CDs Of HCD
        vector<int> varSol;  //CDs Of HCD
        vector<int> GSP;     //generated subproblem
        bool isUsed;
        int lastVar;         //updated while adding solve or assignment nodes
        int afterPredefined;
};

#endif // CD_H
