#ifndef NODE_H
#define NODE_H

#include "Utilities.h"
#include <vector>
#include <iostream>
#include <assert.h>
class CD;

class Node
{
    public:
        Node();
        void setSource(int nextNode);  //source node
        void setSink();  //sink node
        void setZ(int ZEqualVar,int nextNode = Constants::SINK_INDEX);  //Z node
        void setCondition(short type,int var1,int var2,int nextNodeTrue,int nextNodeFalse,bool flipOutEdges); //conditional node
        void setAssign(short type,int var1,int var2,int nextNode); //assignment node

        void setPush(int var1,int nextNode);
        void setSolve(int var1,int nextNodeTrue,int nextNodeFalse);

        int  evaluateNode         (vector<int> &varSol , vector<int> &nodeLTM , vector<int> &LASPlusX, vector<int> &LAEPlusX ,vector<int> &GSP,vector<CD> &MGICDs,int levelOfCD,bool isTesting,bool fAnimation);  //returns next node Index and updates the sol of Variable1 in case of assignment node

        void setNodeParameters(vector<int> parameters);  //used in loading CD nodes from file
        void setNextNode(bool cond , int nxt);
        void setNextNode(int nxt);
        void setVar1Var2(int var1,int var2);
        void setZEqualVar(int ZEqualVar);
        void writeVarAnimation(int varSol,const vector<int> &nodeLTM,const vector<int> &LASPlusX,const vector<int> &LAEPlusX);
        void resetIsUsedFlags();

        vector<int> getNodeParameters();  //used in saving CD nodes to file
        short getNodeType();
        int getNextNode(bool cond);
        int getNextNode();
        int getVar1();
        int getVar2();
        bool getfCycleNode();
        void setfCycleNode(bool flag);
        int  getZEqualVar();
        bool IsNodeUsed();
        ///returns true if
        ///(1)the node isn't conditional
        ///(2)the node is conditional and both out edges are used in solving the training data
        virtual ~Node();
    protected:
    private:
        short nodeType;
        int next,nextTrue,nextFalse;
        int ZSol;
        int variable1,variable2;

        bool isCycleLastUpdate;  //raised when node is at start or end of cycle
        bool nextTrueUsed;
        bool nextFalseUsed;
};

#endif // NODE_H
