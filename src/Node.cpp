#include "Node.h"
#include "IOHandler.h"
#include "QuestionHandler.h"
#include "Memo.h"
QuestionHandler QH;

//print variable
#define PVV(x)  cout<<#x<<" = "<<x<<"\n";
//print vector content
#define PMV(x)  cout<<"\n===========\n"; {for(int i=0;i<x.size();i++) PVV(x[i]);} cout<<"===========\n";
inline int valueOfVar(int &var,vector<int> &varSol,vector<int> &LTM)
{
    if(var < 0)
        return -var;
    else
        if(varSol[var]<0)
            return -varSol[var];
        else
            return LTM[varSol[var]];
}

//-----------------------------------------
Node::Node(){
    nodeType = 0;
    variable1 = 0;
    variable2 = 0;
    ZSol = 0;
    next = 0;
    nextTrue = 0;
    nextFalse = 0;

    isCycleLastUpdate = false;
    nextTrueUsed=false;
    nextFalseUsed=false;
}

Node::~Node(){
    //dtor
}
//-----------------------------------------
void Node::writeVarAnimation(int varSol,const vector<int> &nodeLTM,const vector<int> &LASPlusX,const vector<int> &LAEPlusX)
{
    IOHandler IOH;
    int i;
    if(varSol>=0)
    {
        for(i = 0;i<LASPlusX.size();i++)
        {
            if(nodeType == Constants::ASSIGN_AFTER)
            {
                if(varSol >= LASPlusX[i] && varSol< LAEPlusX[i])
                    break;
            }
            else if(nodeType == Constants::ASSIGN_BEFORE)
            {
                if(varSol > LASPlusX[i] && varSol<= LAEPlusX[i])
                    break;
            }
            else
            {
                if(varSol > LASPlusX[i] && varSol< LAEPlusX[i])
                    break;
            }
        }
        if(i == (LASPlusX.size()-1)) //defined on question
        {
            IOH.addToAnimation(-1);
            IOH.addToAnimation(" ");
            IOH.addToAnimation(varSol-LASPlusX[i]);
            IOH.addToAnimation(" ");
        }
        else
        {
            IOH.addToAnimation(i);
            IOH.addToAnimation(" ");
            IOH.addToAnimation(varSol-LASPlusX[i]);
            IOH.addToAnimation(" ");
        }
    }
    else
    {

    }

}

void Node::resetIsUsedFlags()
{
    nextTrueUsed=false;
    nextFalseUsed=false;
}

void Node::setSource(int nextNode)
{
    nodeType = Constants::SOURCE;
    next = nextNode;
}

void Node::setSink()
{
    nodeType = Constants::SINK;
}

void Node::setZ(int ZEqualVar,int nextNode)
{
    nodeType = Constants::Z_NODE;
    ZSol = ZEqualVar;
    next = nextNode;
}

void Node::setCondition(short type,int var1,int var2,int nextNodeTrue,int nextNodeFalse,bool flipOutEdges)
{
    nodeType  = type;
    variable1 = var1;
    variable2 = var2;
    if(flipOutEdges)
    {
        nextTrue  = nextNodeFalse;
        nextFalse = nextNodeTrue;
    }
    else
    {
        nextTrue  = nextNodeTrue;
        nextFalse = nextNodeFalse;
    }
}

void Node::setAssign(short type,int var1,int var2,int nextNode)
{
    nodeType  = type;
    variable1 = var1;
    variable2 = var2;
    next      = nextNode;
}

void Node::setPush(int var1,int nextNode)
{
    nodeType  = Constants::PUSH;
    variable1 = var1;
    next      = nextNode;
}

void Node::setSolve(int var1,int nextNodeTrue,int nextNodeFalse)
{
    nodeType  = Constants::SOLVE;
    variable1 = var1;

    nextTrue  = nextNodeTrue;
    nextFalse = nextNodeFalse;
}


int Node::evaluateNode(vector<int> &varSol ,vector<int> &nodeLTM ,vector<int> &LASPlusX,vector<int> &LAEPlusX ,vector<int> &GSP,vector<CD> &MGICDs,int levelOfCD,bool isTesting,bool fAnimation)
{
    static int tmpSol;
    //----------------------------------------------------
        /*  if(isTesting)
            {
            cout<<"-------------------\n";
            for(int i=0;i<7;i++){
                cout<<"solOfvar"<<i<<" = "<<varSol[i]-varSol[0];
                cout<<"  ==>  LTM[]= "<<nodeLTM[varSol[i]]<<"\n";}
            cout<<"-------------------\n";

            getch();

            }*/

    if(nodeType == Constants::PUSH)
    {
        GSP.push_back(valueOfVar(variable1,varSol,nodeLTM));

        if(fAnimation)
        {
            Memo *memory = Memo::getInstance();
            IOHandler IOH;

            /**/IOH.addToAnimation(nodeType);

            for(int i=0;i<GSP.size();i++)
            {
                /**/IOH.addToAnimation(" ");
                /**/IOH.addToAnimation(memory->getKey(GSP[i]));
            }

            //**/writeVarAnimation(varSol[variable1],nodeLTM,LASPlusX,LAEPlusX);
            //**/IOH.addToAnimation(memory->getKey(-variable2));   IOH.addToAnimation(" ");
            //**/IOH.addToAnimation(nodeLTM[varSol[variable1]] == (-1*variable2));
            /**/IOH.addToAnimation("\n");
        }

        return next;
    }

    else if(nodeType == Constants::SOLVE)
    {
        if(fAnimation)
        {
            IOHandler IOH;

            /**/IOH.addToAnimation(nodeType);
            /**/IOH.addToAnimation(" ");
            /**/IOH.addToAnimation(levelOfCD+1);
            /**/IOH.addToAnimation("\n");
        }

        if(isTesting)
        {
            Memo *memory = Memo::getInstance();
            /*cout<<"\nGSP : ";
            for(int i=0;i<GSP.size();i++)
            {
                cout<<memory->getKey(GSP[i])<<" ";
            }*/
            /*
            cout<<"-------------------\n";
            for(int i=0;i<7;i++){
                cout<<"solOfvar"<<i<<" = "<<varSol[i]-varSol[0];
                cout<<"  ==>  LTM[]= "<<nodeLTM[varSol[i]]<<"\n";}
            cout<<"-------------------\n";*/

        }
        //================== solve
        if(!isTesting)
        if(levelOfCD == Constants::FIRST_LEVEL_CD )
        {
            set<int> possibleAnswers;
            int ans,questionMatchedVCD;
            possibleAnswers.clear();
            vector<CD> tmpMGICDs = MGICDs;
            for(int i=0;i<MGICDs.size();i++)  //loop on VCDs
            {
                ans = MGICDs[i].evaluateCD(nodeLTM,LASPlusX,LAEPlusX,GSP,tmpMGICDs,false,levelOfCD+1,isTesting);
                if(ans != Constants::UNDEFINED_VAR)
                {
                    questionMatchedVCD = i;
                    possibleAnswers.insert(ans);
                }
            }
            if(possibleAnswers.size() == 1)
            {
                MGICDs[questionMatchedVCD].setIsUsed();
                varSol[variable1] = -(*possibleAnswers.begin());
            }
        }

        if(isTesting)
        if(levelOfCD<4)
        {
            set<int> possibleAnswers;
            int ans,questionMatchedVCD;
            possibleAnswers.clear();
            vector<CD> tmpMGICDs = MGICDs;
            for(int i=0;i<MGICDs.size();i++)  //loop on VCDs
            {
                ans = MGICDs[i].evaluateCD(nodeLTM,LASPlusX,LAEPlusX,GSP,tmpMGICDs,false,levelOfCD+1,isTesting);
                if(ans != Constants::UNDEFINED_VAR)
                {
                    questionMatchedVCD = i;
                    possibleAnswers.insert(ans);
                }
            }
            if(possibleAnswers.size() == 1)
            {
                if(fAnimation)
                {
                    IOHandler IOH;
                    Memo *memory = Memo::getInstance();

                    set<int> predefinedValues = MGICDs[questionMatchedVCD].getPredefinedValues();
                    for (set<int>::iterator it=predefinedValues.begin(); it!=predefinedValues.end(); ++it)
                    {
                        IOH.addToAnimation(memory->getKey(*it));
                        IOH.addToAnimation(" ");
                    }
                    IOH.addToAnimation("\n");
                    if(MGICDs[questionMatchedVCD].getType() == Constants::POSITIVE_CD)
                        IOH.addToAnimation("PositivePart\n");
                    ans = MGICDs[questionMatchedVCD].evaluateCD(nodeLTM,LASPlusX,LAEPlusX,GSP,tmpMGICDs,fAnimation,levelOfCD+1,isTesting);

                }

                MGICDs[questionMatchedVCD].setIsUsed();
                varSol[variable1] = -(*possibleAnswers.begin());
            }

        }

        //================== push
        //GSP.push_back(varSol[variable1]);
        //================== clear
        /*
        Memo *memory = Memo::getInstance();
        cout<<"\nGSP : ";
        for(int i=0;i<GSP.size();i++)
        {
            cout<<memory->getKey(GSP[i])<<" ";
        }
        cout<<"\n"<<memory->getKey()<<"\n";


        //getch();
        //*/

        GSP.clear();
        //==================
        if(fAnimation)
        {
            IOHandler IOH;
            if(varSol[variable1] == Constants::UNDEFINED_VAR)
                /**/IOH.addToAnimation("\n");
            /**/IOH.addToAnimation(nodeType);
            /**/IOH.addToAnimation(" ");
            /**/IOH.addToAnimation(levelOfCD);
            //**/writeVarAnimation(varSol[variable1],nodeLTM,LASPlusX,LAEPlusX);
            //**/IOH.addToAnimation(memory->getKey(-variable2));   IOH.addToAnimation(" ");
            //**/IOH.addToAnimation(nodeLTM[varSol[variable1]] == (-1*variable2));
            /**/IOH.addToAnimation("\n");
        }

        if(varSol[variable1] != Constants::UNDEFINED_VAR)
            return nextTrue;
        else
            return nextFalse;
    }
    else if(nodeType == Constants::ASSIGN_AFTER)
    {
        tmpSol = varSol[variable2];
        varSol[variable1] = varSol[variable2] + 1;
        if(varSol[variable1]>=nodeLTM.size() || varSol[variable1] <0)      return Constants::SINK_INDEX;
        if(nodeLTM[varSol[variable1]] == Constants::LTM_SEPARATOR)         return Constants::SINK_INDEX;

        //--------------------------
        if(fAnimation)
        {
            IOHandler IOH;
            /**/IOH.addToAnimation(nodeType);
            /**/IOH.addToAnimation(" ");
            /**/writeVarAnimation(varSol[variable1],nodeLTM,LASPlusX,LAEPlusX);
            /**/writeVarAnimation(tmpSol,nodeLTM,LASPlusX,LAEPlusX);
            /**/IOH.addToAnimation((variable1 == variable2));
            /**/IOH.addToAnimation("\n");
        }
        //--------------------------
        return next;
    }

    else if(nodeType == Constants::ASSIGN_BEFORE)
    {
        tmpSol = varSol[variable2];
        varSol[variable1] = varSol[variable2] - 1;
        if(varSol[variable1]>=nodeLTM.size() || varSol[variable1] <0)      return Constants::SINK_INDEX;
        if(nodeLTM[varSol[variable1]] == Constants::LTM_SEPARATOR)         return Constants::SINK_INDEX;

        if(fAnimation)
        {
            IOHandler IOH;
            /**/IOH.addToAnimation(nodeType);
            /**/IOH.addToAnimation(" ");
            /**/writeVarAnimation(varSol[variable1],nodeLTM,LASPlusX,LAEPlusX);
            /**/writeVarAnimation(tmpSol,nodeLTM,LASPlusX,LAEPlusX);
            /**/IOH.addToAnimation((variable1 == variable2));
            /**/IOH.addToAnimation("\n");
        }
        return next;
    }

    else if(nodeType == Constants::COND_VALUE)
    {
        if(fAnimation)
        {
            Memo *memory = Memo::getInstance();
            IOHandler IOH;
            if(variable2<0)
            {
                /**/IOH.addToAnimation(nodeType);
                /**/IOH.addToAnimation(" ");
                /**/writeVarAnimation(varSol[variable1],nodeLTM,LASPlusX,LAEPlusX);
                /**/IOH.addToAnimation(memory->getKey(-variable2));   IOH.addToAnimation(" ");
                /**/IOH.addToAnimation(nodeLTM[varSol[variable1]] == (-1*variable2));
                /**/IOH.addToAnimation("\n");
            }
            else
            {
                /**/IOH.addToAnimation(nodeType);
                /**/IOH.addToAnimation(" ");
                /**/writeVarAnimation(varSol[variable1],nodeLTM,LASPlusX,LAEPlusX);
                /**/writeVarAnimation(varSol[variable2],nodeLTM,LASPlusX,LAEPlusX);
                /**/IOH.addToAnimation((nodeLTM[varSol[variable1]] == nodeLTM[varSol[variable2]]));
                /**/IOH.addToAnimation("\n");
            }
        }

        if(valueOfVar(variable1,varSol,nodeLTM) == valueOfVar(variable2,varSol,nodeLTM))
        {
            nextTrueUsed=true;
            return nextTrue;
        }
        else
        {
             nextFalseUsed = true;
             return nextFalse;
        }
    }
    else if(nodeType == Constants::Z_NODE)
    {
        if(fAnimation)
        {
            Memo *memory = Memo::getInstance();
            IOHandler IOH;
            /**/IOH.addToAnimation(nodeType);
            /**/IOH.addToAnimation(" ");
            /**/IOH.addToAnimation(memory->getKey(valueOfVar(ZSol,varSol,nodeLTM)));
            /**/IOH.addToAnimation("\n");
        }

        varSol[Constants::Z_VAR] = valueOfVar(ZSol,varSol,nodeLTM);
        return next;
    }

    else if(nodeType == Constants::SOURCE)
    {
        GSP.clear();
        return next;
    }
    //----------------------------------------------------
}

short Node::getNodeType(){
    return nodeType;
}

void Node::setVar1Var2(int var1,int var2)
{
    variable1 = var1;
    variable2 = var2;
}

void Node::setZEqualVar(int ZEqualVar)
{
    ZSol = ZEqualVar;
}

int Node::getNextNode()
{
    return next;
}

int Node::getNextNode(bool cond)
{
    if(cond) return nextTrue;
     else    return nextFalse;
}

int Node::getVar1()
{
    return variable1;
}

int Node::getVar2()
{
    return variable2;
}

void Node::setNextNode(bool cond,int nxt)
{
    if(cond) nextTrue = nxt;
    else nextFalse = nxt;
}

void Node::setNextNode(int nxt)
{
    next = nxt;
}


int Node::getZEqualVar()
{
    return ZSol;
}

bool Node::IsNodeUsed()
{
    if(nodeType == Constants::COND_VALUE)
        return nextTrueUsed&nextFalseUsed;
    else
        return true;
}

void Node::setNodeParameters(vector<int> parameters) 
{
    
}

vector<int> Node::getNodeParameters()
{
    vector<int> parameters;

    return parameters;
}


bool Node::getfCycleNode()
{
    return isCycleLastUpdate;
}

void Node::setfCycleNode(bool flag)
{
    isCycleLastUpdate = flag;
}
