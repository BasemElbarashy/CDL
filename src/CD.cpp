#include "CD.h"
#include <iostream>
#include "Memo.h"
#include "IOHandler.h"
#include <bitset>         // std::bitset
#include <set>

//print variable
#define PVV(x)  cout<<#x<<" = "<<x<<"\n";
//print vector content
#define PMV(x)  cout<<"\n===========\n"; {for(int PMVC=0;PMVC<x.size();PMVC++) PVV(x[PMVC]);} cout<<"===========\n";
IOHandler IOH;


CD::CD()
{
    type = Constants::POSITIVE_CD;
    Node source,sink,z;
    z.setZ(-100,Constants::SINK_INDEX);
    source.setSource(Constants::Z_INDEX);
    sink.setSink();

    nodes.push_back(sink);
    nodes.push_back(z);
    nodes.push_back(source);

    varSol.assign(Constants::MAX_NUM_VARIABLES , Constants::UNDEFINED_VAR);
    inEdgeNode.assign(Constants::MAX_NUM_VARIABLES , Constants::UNDEFINED_VAR);
    inEdgeNode[Constants::Z_INDEX] = Constants::SOURCE_INDEX;
    lastVar = 2;
    afterPredefined = 3;
    //source node added as a third node to make adding nodes easier in the future
    //adding node: 1)change the previous one to point to the new one
    //             2)change the new one to point to z-node
    //-100 is just an initial value and must be modified in generator
}



CD::~CD()
{
    //dtor
}

bool CD::isHighLevel()
{
    for(int i =0;i<nodes.size();i++)
        if(nodes[i].getNodeType() == Constants::SOLVE)
            return true;
    return false;
}

int CD::evaluateCD(vector<int> &LTMPlusX, vector<int> &LASPlusX, vector<int> &LAEPlusX,vector<CD> &MGICDs,bool fAnimation,int levelOfCD,bool isTesting)
{
    if(type == Constants::POSITIVE_CD)
    {
        static int CurrentNodeIndex;
        CurrentNodeIndex=Constants::SOURCE_INDEX;
        for(int i=afterPredefined;i<=lastVar;i++)
            varSol[i] = Constants::UNDEFINED_VAR;
        //---------------------- predefined vars based on current question---------------
        //The other predefined variables are already saved in varSol
        varSol[Constants::START_VAR] = LASPlusX.back();
        varSol[Constants::END_VAR]   = LAEPlusX.back();
        varSol[Constants::Z_VAR]     = Constants::UNDEFINED_VAR;
        //-------------------------------------------------------------------------------
        while(CurrentNodeIndex!=Constants::SINK_INDEX)
        {
            CurrentNodeIndex = nodes[CurrentNodeIndex].evaluateNode(varSol , LTMPlusX,LASPlusX,LAEPlusX,GSP,MGICDs,levelOfCD,isTesting,fAnimation);
        }
        if(fAnimation)
        {
            if(varSol[Constants::Z_VAR] == Constants::UNDEFINED_VAR)
                IOH.addToAnimation("Failed\n");
            else
                IOH.addToAnimation("Succeeded\n");
        }
        return varSol[Constants::Z_VAR];
    }

    if(type == Constants::HYBRID_CD)
    {
        ///the first CD must succeed and the rest NCDs must return Undefined
        static int fAllNCDsUndefined,ans,i;
        fAllNCDsUndefined = true;

        if(fAnimation)   IOH.addToAnimation("NegativePart\n");

        for(i=1;i<CDsOfHCD.size();i++)
            if(CDsOfHCD[i].evaluateCD(LTMPlusX,LASPlusX,LAEPlusX,MGICDs,fAnimation,levelOfCD,isTesting) != Constants::UNDEFINED_VAR)
            {
                fAllNCDsUndefined = false;
                break;
            }

        if(fAllNCDsUndefined)
        {
            if(fAnimation)   IOH.addToAnimation("PositivePart\n");
            return CDsOfHCD[0].evaluateCD(LTMPlusX,LASPlusX,LAEPlusX,MGICDs,fAnimation,levelOfCD,isTesting);
        }
        else                  return Constants::UNDEFINED_VAR;
    }
}

int CD::evaluateCD( vector<int> &CDLTM, vector<int> &CDLAS, vector<int> &CDLAE,vector<int> &question,vector<CD> &MGICDs,bool fAnimation,int levelOfCD,bool isTesting)
{
    vector<int> tmpCDLTM;
    vector<int> tmpCDLAS;
    vector<int> tmpCDLAE;

    tmpCDLTM = CDLTM;
    tmpCDLAS = CDLAS;
    tmpCDLAE = CDLAE;
    int ans;

    tmpCDLAS.push_back(tmpCDLAE.back());
    for(int i = 0;i<question.size();i++)
        tmpCDLTM.push_back(question[i]);
    tmpCDLTM.push_back( int(Constants::LTM_SEPARATOR ));
    tmpCDLAE.push_back(tmpCDLTM.size()-1);

    ans = evaluateCD(tmpCDLTM,tmpCDLAS,tmpCDLAE,MGICDs,fAnimation,levelOfCD,isTesting);

    return ans;
}

void CD::resetIsUsedFlags()
{
    static int i;
    for(i = 0;i<nodes.size();i++)
        nodes[i].resetIsUsedFlags();
}

void CD::setType(int typeOfCD)
{
    type = typeOfCD;
}

void CD::addUpdateNode(Node newNode)
{
    nodes.push_back(newNode);
    //---------------------------  flipping the conditional node
    if(nodes[nodes.size()-2].getNodeType() == Constants::COND_VALUE ||
       nodes[nodes.size()-2].getNodeType() == Constants::SOLVE)
    {
        if(nodes[inEdgeNode[Constants::Z_INDEX]].getNextNode(true) == Constants::Z_INDEX)
        {
            nodes[inEdgeNode[Constants::Z_INDEX]].setNextNode(true , nodes.size()-1);
            nodes[inEdgeNode[Constants::Z_INDEX]].setNextNode(false, Constants::Z_INDEX);
        }
        else
        {
            nodes[inEdgeNode[Constants::Z_INDEX]].setNextNode(false, nodes.size()-1);
            nodes[inEdgeNode[Constants::Z_INDEX]].setNextNode(true , Constants::Z_INDEX);
        }
    }
    else
    {
        nodes[nodes.size()-2].setNextNode(nodes.size()-1);
    }
    //---------------------------
}

void CD::removeUpdateNode()
{
    //---------------------------  flip the conditional node
    if(nodes[nodes.size()-2].getNodeType() == Constants::COND_VALUE ||
       nodes[nodes.size()-2].getNodeType() == Constants::SOLVE)
    {
        if(nodes[inEdgeNode[Constants::Z_INDEX]].getNextNode(true) == Constants::Z_INDEX)
        {
            nodes[inEdgeNode[Constants::Z_INDEX]].setNextNode(true , Constants::SINK_INDEX);
            nodes[inEdgeNode[Constants::Z_INDEX]].setNextNode(false, Constants::Z_INDEX);
        }
        else
        {
            nodes[inEdgeNode[Constants::Z_INDEX]].setNextNode(false, Constants::SINK_INDEX);
            nodes[inEdgeNode[Constants::Z_INDEX]].setNextNode(true , Constants::Z_INDEX);
        }
    }
    nodes.pop_back();
    //---------------------------
}

void CD::addNode(Node newNode)
{
    nodes.push_back(newNode);
    //---------------------------
    //make the last node (nodes[nodes.size()-2]) points to the new node (nodes[nodes.size()-1])...
    //not to Z-node

    if(nodes[inEdgeNode[Constants::Z_INDEX]].getNodeType() == Constants::COND_VALUE ||
       nodes[inEdgeNode[Constants::Z_INDEX]].getNodeType() == Constants::SOLVE)
    {
        if(nodes[inEdgeNode[Constants::Z_INDEX]].getNextNode(true) == Constants::Z_INDEX)
            nodes[inEdgeNode[Constants::Z_INDEX]].setNextNode(true, nodes.size()-1);
        else
            nodes[inEdgeNode[Constants::Z_INDEX]].setNextNode(false, nodes.size()-1);
    }
    else
    {
        nodes[inEdgeNode[Constants::Z_INDEX]].setNextNode(nodes.size()-1);
    }

    if(newNode.getNodeType() == Constants::ASSIGN_AFTER ||
       newNode.getNodeType() == Constants::ASSIGN_BEFORE||
       newNode.getNodeType() == Constants::SOLVE)
        lastVar = newNode.getVar1();

    inEdgeNode[nodes.size()-1] = inEdgeNode[Constants::Z_INDEX];
    inEdgeNode[Constants::Z_INDEX] = nodes.size()-1;
    //---------------------------
}

void CD::removeLastNode() //reverse the operations in addNode
{

    if(nodes[inEdgeNode[nodes.size()-1]].getNodeType() == Constants::COND_VALUE ||
       nodes[inEdgeNode[nodes.size()-1]].getNodeType() == Constants::SOLVE)
    {
        if(nodes[inEdgeNode[nodes.size()-1]].getNextNode(true) == (nodes.size()-1))
            nodes[inEdgeNode[nodes.size()-1]].setNextNode(true, Constants::Z_INDEX);
        else
            nodes[inEdgeNode[nodes.size()-1]].setNextNode(false, Constants::Z_INDEX);
    }
    else
    {
        nodes[inEdgeNode[nodes.size()-1]].setNextNode(Constants::Z_INDEX);
    }
    inEdgeNode[Constants::Z_INDEX] = inEdgeNode[nodes.size()-1];
    nodes.pop_back();
}


void CD::addCDToHCD(CD newCD)
{
    if(CDsOfHCD.empty())            //Source action of the HCD = Source action of its PCD
        setSourceAction(newCD.getSourceAction());

    setType(Constants::HYBRID_CD);
    CDsOfHCD.push_back(newCD);
}

void CD::removeCDFromHCD()
{
    CDsOfHCD.pop_back();
}

int CD::getNumOfNodes()
{
    if(type == Constants::POSITIVE_CD)
        return nodes.size();
    if(type == Constants::HYBRID_CD){
        int i,j;
        for(i=0,j=0;i<CDsOfHCD.size();i++)  j+=CDsOfHCD[i].getNumOfNodes();
        return j;
    }
}



Node CD::getNode(int nodeIndex){
    return nodes.at(nodeIndex);
}


void CD::setNextNode(int ConditionalParentIndex ,bool cond, int nodeIndex)
{
    nodes[ConditionalParentIndex].setNextNode(cond , nodeIndex);
}


void CD::setNextNode(int nodeIndex ,int nextNodeIndex)
{
    nodes[nodeIndex].setNextNode(nextNodeIndex);
}


void CD::replaceNode(int NodeIndex,Node newNode)
{
    nodes[NodeIndex] = newNode;
    if(newNode.getNodeType() == Constants::ASSIGN_AFTER ||
       newNode.getNodeType() == Constants::ASSIGN_BEFORE||
       newNode.getNodeType() == Constants::SOLVE)
        lastVar = newNode.getVar1();
}

int CD::getSourceAction()
{
    return mSourceAction;
}

void CD::setSourceAction(int sourceAction)
{
    mSourceAction = sourceAction;
}

void CD::addPredefinedLTMVar(int var,int sol)  //add predefined variables to varSol
{
    varSol[var] = sol;
    afterPredefined++;
}

void CD::removePredefinedLTMVar(int var)
{
    varSol[var] = Constants::UNDEFINED_VAR;
    afterPredefined--;
}

int CD::getSolOfVar(int var)
{
    return varSol[var];
}
int CD::getType()
{
    return type;
}

set<int> CD::getPredefinedValues()
{
    int v1,v2,z,nodeType;
    set<int> predefinedValues;
    set<int> temp;
    if(type == Constants::POSITIVE_CD)
    {
        for(int i=0;i<nodes.size();i++)
        {
            nodeType = nodes[i].getNodeType();
            v1       = nodes[i].getVar1();
            v2       = nodes[i].getVar2();
            z        = nodes[i].getZEqualVar();
            if(nodeType == Constants::COND_VALUE)
            {
                if(v2<0)
                    predefinedValues.insert(-v2);
            }
            if(nodeType == Constants::PUSH)
            {
                if(v1<0)
                    predefinedValues.insert(-v1);
            }
            if(nodeType == Constants::Z_NODE)
            {
                if(z<0)
                    predefinedValues.insert(-z);
            }
        }
    }

    if(type == Constants::HYBRID_CD)
    {
        for(int i=0;i<CDsOfHCD.size();i++)
        {
            temp = CDsOfHCD[i].getPredefinedValues();
            predefinedValues.insert( temp.begin() , temp.end() );
        }
    }
    return predefinedValues;
}

void CD::setCycleNodeFlag(int NodeIdx,bool flag)
{
    nodes[NodeIdx].setfCycleNode(flag);
}

void CD::resetIsUsed()
{
    isUsed = false;
}

void CD::setIsUsed()
{
    isUsed = true;
}

bool CD::getIsUsed()
{
    return isUsed;
}
vector<int> CD::getGSP()
{
    return GSP;
}
void CD::popGSP()
{
    GSP.pop_back();
}
void CD::printCD()
{
    Memo *memory = Memo::getInstance();
    bool fInCycle = false;
    //cout<<"------------------------\n";
    cout<<"\n";
    //if(type == Constants::HYBRID_CD)   cout<<"Hybrid CD\n\n";
    //if(type == Constants::POSITIVE_CD) cout<<"Positive CD\n\n";

    vector<vector<int> > startCycle(100);
    vector<vector<int> > endCycle(100);
    int cycleCounter = 1;
    for(int i=0;i<nodes.size();i++)
    {
        if(nodes[i].getfCycleNode())
        {
            endCycle[i].push_back(cycleCounter);
            startCycle[nodes[i].getNextNode()].push_back(cycleCounter);
            cycleCounter++;
        }
    }
    if(type == Constants::POSITIVE_CD)
    {
        for(int j=0;j<nodes.size();j++) //loop on nodes
        {
            for(int i=(startCycle[j].size()-1);i>=0;i--)
            {
                cout<<"\nCycle[ "<<startCycle[j][i]<<" ] start\n";
            }

            cout<<"["<<j<<"] ";
            if(nodes[j].getNodeType() == Constants::Z_NODE)
            {
                cout<<"Z_NODE : ";
                if(nodes[j].getZEqualVar() < 0) cout<<"\""<<memory->getKey(-1*nodes[j].getZEqualVar())<<"\"";
                else cout<<"("<<nodes[j].getZEqualVar()<<")";
            }
            if(nodes[j].getNodeType() == Constants::PUSH)
            {
                cout<<"PUSH : ";

                if(nodes[j].getVar1() < 0 )
                    cout<<"\""<<memory->getKey(-1*nodes[j].getVar1())<<"\"";
                else
                    cout<<"("<<nodes[j].getVar1()<<")";


                cout<<"   Next ";
                cout<<nodes[j].getNextNode();
            }

            if(nodes[j].getNodeType() == Constants::COND_TIME)
            {
                cout<<"COND_TIME : ";

                cout<<"("<<nodes[j].getVar1()<<")";
                cout<<" == ";
                cout<<"("<<nodes[j].getVar2()<<")";

                cout<<"   Next ";
                cout<<nodes[j].getNextNode(true)<<" ";
                cout<<nodes[j].getNextNode(false);
            }

            if(nodes[j].getNodeType() == Constants::SOLVE)
            {
                cout<<"SOLVE : ";
                cout<<"("<<nodes[j].getVar1()<<")";

                cout<<"   Next ";
                cout<<nodes[j].getNextNode(true)<<" ";
                cout<<nodes[j].getNextNode(false);
            }

            if(nodes[j].getNodeType() == Constants::COND_VALUE)
            {
                cout<<"COND_VALUE : ";
                cout<<"("<<nodes[j].getVar1()<<")";
                cout<<" == ";
                if(nodes[j].getVar2() < 0 )
                    cout<<"\""<<memory->getKey(-1*nodes[j].getVar2())<<"\"";
                else
                    cout<<"("<<nodes[j].getVar2()<<")";

                cout<<"   Next ";
                cout<<nodes[j].getNextNode(true)<<" ";
                cout<<nodes[j].getNextNode(false);
            }
            if(nodes[j].getNodeType() == Constants::ASSIGN_AFTER)
            {
                cout<<"ASSIGN_AFTER : ";

                if(nodes[j].getVar2() == 0)
                cout<<"("<<nodes[j].getVar1()<<") = (s)"<<" + 1";
                else
                cout<<"("<<nodes[j].getVar1()<<") = ("<<nodes[j].getVar2()<<")"<<" + 1";

                cout<<"   Next "<<nodes[j].getNextNode();
            }
            if(nodes[j].getNodeType() == Constants::ASSIGN_BEFORE)
            {
                cout<<"ASSIGN_BEFORE : ";

                if(nodes[j].getVar2() == 1)
                cout<<"("<<nodes[j].getVar1()<<") = (e)"<<" - 1";
                else
                cout<<"("<<nodes[j].getVar1()<<") = ("<<nodes[j].getVar2()<<")"<<" - 1";

                cout<<"   Next "<<nodes[j].getNextNode();
            }
            if(nodes[j].getNodeType() == Constants::SOURCE)
            {
                cout<<"SOURCE : ";
                cout<<" Next "<<nodes[j].getNextNode();

            }
            if(nodes[j].getNodeType() == Constants::SINK)
            {
                cout<<"SINK : ";
            }

            for(int i=0;i<endCycle[j].size();i++)
            {
                cout<<"\nCycle[ "<<endCycle[j][i]<<" ] end\n";
            }

            cout<<"\n";
        }

        /*
        cout<<"\nGSP : ";
        for(int i=0;i<GSP.size();i++)
        {
            cout<<memory->getKey(GSP[i])<<" ";
        }
        cout<<"\n";
        */

        //cout<<"------------------------\n";
        cout<<"\n";
    }


    if(type == Constants::HYBRID_CD)
    {
        cout<<"---- [0] PCD ----\n";
        CDsOfHCD[0].printCD();
        for(int i=1;i<CDsOfHCD.size();i++)  //loop on NCDs
        {
            cout<<"---- ["<<i<<"] NCD ----\n";
            CDsOfHCD[i].printCD();
        }
    }

}
void CD::printVarSol()
{
    cout<<"\n==================================\n";
    for(int i=0;i<varSol.size();i++)
    {
        if(varSol[i] == Constants::UNDEFINED_VAR) continue;
        if(i == 0)
            cout<<"varSol[ s ] = "<<varSol[i]<<"\n";
        else if(i == 1)
            cout<<"varSol[ e ] = "<<varSol[i]<<"\n";
        else if(i == 2)
            cout<<"varSol[ z ] = "<<varSol[i]<<"\n";
        else
            cout<<"varSol[ "<<i<<" ] = "<<varSol[i]<<"\n";
    }
    cout<<"==================================\n";
}
