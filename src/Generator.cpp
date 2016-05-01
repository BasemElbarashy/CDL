#include "Generator.h"
#include <algorithm>
#include <assert.h>
#include <map>
#include <set>
#include <ctime>
#include "Utilities.h"

//print variable
#define PVV(x)  cout<<#x<<" = "<<x<<"\n";
//print vector content
#define PMV(x)  cout<<"\n===========\n"; {for(int PMVC=0;PMVC<x.size();PMVC++) PVV(x[PMVC]);} cout<<"===========\n";
//print vector content
//#define PMV(x,n)  cout<<"\n===========\n"; {for(int PMVC=0;PMVC<x.size(),PMVC<n;PMVC++) PVV(x[PMVC]);} cout<<"===========\n";


const int FIRST_Call = 0;
const int INF = 1000000;
const int VAR_RIGHT      = Constants::ASSIGN_AFTER;   //+1 ==> positive assignment
const int VAR_LEFT       = Constants::ASSIGN_BEFORE;  //-1 ==> negative assignment
const int VAR_RIGHT_LEFT = VAR_RIGHT + VAR_LEFT;      // 0
const int dummy=0;

bool isHighLevelCDsAllowed;
bool isAssignmentBeforeAllowed;
clock_t t;


Generator::Generator()
{
    nPCDGeneratorCalls   = 0;
    nMGICDGeneratorCalls = 0;
}

Generator::~Generator()
{
    //dtor
}

inline int valueOfVar(int &var,vector<int> &varSol,vector<int> &LTM)
{
    if(var < 0)
        return -var;
    else
    {
        if(varSol[var]<0)
            return -varSol[var];
        else
            return LTM[varSol[var]];
    }
}


vector<int> addPCDARows(vector<int> PCDRow ,vector<int> NCDRow)
{
    vector<int> sum;
    for(int i=0;i<PCDRow.size();i++)
    {
        if(NCDRow[i] == Constants::UNDEFINED_VAR)
            sum.push_back(PCDRow[i]);
        else
            sum.push_back(int(Constants::UNDEFINED_VAR));
    }
    return sum;
}

bool validate(vector<int> CDRow)  //valid CD: its characteristic vector has (no WA) and (one or more RA)
{
    bool fRAExist=false;
    for(int i =0;i<CDRow.size();i++)
    {
       if(CDRow[i] == Constants::WRONG_ANSWER)  return false;
       if(CDRow[i] == Constants::RIGHT_ANSWER)  fRAExist=true;
    }
    if(!fRAExist)  return false;
    return true;
}

int Generator::calculateGamma()
{
    //calculates Gamma (the estimate of nNodes in MGICD)
    //gamma used as the upper boundary of nNodes while generating MGICD to make it faster
    int i,j,k,l,m;
    int score[Constants::MAX_NUM_VCDS];
    int nNodes=0,nOnes=0,ICDsnNodes=0;
    bitset<Constants::MAX_NUM_PCDS> isValid;
    bitset<Constants::MAX_NUM_ACTIONS>  solvedActions;
    bitset<Constants::MAX_NUM_ACTIONS>  zeros;
    bitset<Constants::MAX_NUM_ACTIONS>  temp;
    bitset<Constants::MAX_NUM_VCDS> selectedVCDs;
    bitset<Constants::MAX_NUM_PCDS> selectedPCDs;

    for(i=0;i<PCDs.size();i++)
        isValid[i] = validate(PCDA[i]);

    while(solvedActions.count() != nActions)
    {
        for(i=0,l=0;i<VCDs.size();i++)  //score VCDs to find the best candidate VCD
        {
            if((VCDA[i]|solvedActions) == solvedActions) continue;
            if((VCDA[i]&solvedActions) != zeros) continue;
            if(selectedVCDs[i])                          continue;
            nNodes=nOnes=0;

            for(j=0;j<VCDs[i].size();j++)
                if(!selectedPCDs[VCDs[i][j]])
                    nNodes+= PCDs[VCDs[i][j]].getNumOfNodes();

            temp = VCDA[i];
            for(j=1;j<VCDs[i].size();j++)
                for(k=0;k<nActions;k++)
                    if(isValid[j] && PCDA[j][k] == Constants::RIGHT_ANSWER)
                        if(!solvedActions[k])
                            temp[k] = 1;
            nOnes=temp.count();

            score[i] = (1000*nOnes)/(nNodes+1);
            if(score[i]>l)   {l=score[i]; m=i;}
        }


        selectedVCDs[m] = 1;

        for(i=0;i<VCDs[m].size();i++)
        {
            if(!selectedPCDs[VCDs[m][i]])
                ICDsnNodes+= PCDs[VCDs[m][i]].getNumOfNodes();
            selectedPCDs[VCDs[m][i]] = 1;

        }
        solvedActions = solvedActions | VCDA[m];
    }
    return ICDsnNodes;
}

void Generator::generatePCDsOfAction(int& actionToLearnIdx,int nextNodeIndex,int callNo, bool isNormalMode,int firstVarAssign,int firstVarCond,vector<CD> &MGICDs)
{
    ///==================================== declarations ======================================///
    static int  var; 
    static int  afterPredefinedIdx;
    static vector<int> varSol(Constants::MAX_NUM_VARIABLES);
    static int  varDirection[Constants::MAX_NUM_VARIABLES];
    static int  varBasicDirection[Constants::MAX_NUM_VARIABLES];
    static int  varScope[Constants::MAX_NUM_VARIABLES];     //(action no) where var defined on
    static bool isVarConstant[Constants::MAX_NUM_VARIABLES];
    static CD   tmpCD;
    static int CDAnswer;
    static Node ZNodeEqualConst,ZNodeEqualVar;
    static int cycleCounter;
    static int conditionCounter;
    static int assigmentsCounter;
    static int negativeCondCounter;
    static int pushCounter;
    static int solveCounter;
    static int pushVarCounter;
    static int PredefinedVarUsedCounter;
    static bool fVariableCD;
    static int maxNumCycle;
    static int cycleMaxCall;
    static int outScopeMaxCall;
    static int maxNumLTMUsedActions;
    static int maxNumValueConditions ;
    static int maxNumAssignments;
    static int maxNumNegativeCond;
    static int maxNumSolve;
    static int maxNumPush;
    static vector<int> LTMPlusCurrentX;
    static vector<int> LASPlusCurrentX;
    static vector<int> LAEPlusCurrentX;
    static vector<vector<int> > XsValues;
    static set<int>::iterator setIt;
    static vector <int>  X;   
    static int  Y;            
    static int  currentScope;
    static bitset<Constants::MAX_NUM_ACTIONS> fActionUsed;
    static bool fOverBuild;
    static int varAvail[Constants::MAX_NUM_VARIABLES];
    static bool CSH[Constants::MAX_NUM_VARIABLES]; //call state history ==> CSH[callNo] = isNormalMode
    static bool varTrueConditioned[Constants::MAX_NUM_VARIABLES];
    static int valueFrequencyInAllXs[Constants::MAX_KEY_LENGTH];
    static bool isValueRepeated[Constants::MAX_KEY_LENGTH];
    static int lastVar;
    static vector<int> GSP;
    static int GSPRelatedAction;
    int i,j,k,tmp1,tmp2; //counters,...
    //PVV(callNo)
    //if(isHighLevelCDsAllowed && actionToLearnIdx == 6)
    //cout<<'.';
    nPCDGeneratorCalls++;
    CSH[callNo] = isNormalMode;
    //if(!(nPCDGeneratorCalls%1000)) PVV(nPCDGeneratorCalls)
    ///============================ First Call ==================================================///
    if(callNo == FIRST_Call)
    {
        afterPredefinedIdx = 2;  // three variables : start,end,Z
        firstVarCond = afterPredefinedIdx;   //all variables can be used in conditional node except predefined variables:S,E,Z
        var         = 3;
        varAvail[0] = Constants::START_VAR;
        varAvail[1] = Constants::END_VAR;
        firstVarAssign = 0;
        lastVar = 1;

        GSP.clear();
        X.clear();
        X = Xs[actionToLearnIdx];
        Y = Ys[actionToLearnIdx];

        LTMPlusCurrentX.clear();  LTMPlusCurrentX = LTMPlusX[actionToLearnIdx];
        LASPlusCurrentX.clear();  LASPlusCurrentX = LASPlusX[actionToLearnIdx];
        LAEPlusCurrentX.clear();  LAEPlusCurrentX = LAEPlusX[actionToLearnIdx];

        XsValues.clear();
        set<int> tmpContainer;
        XsValues.resize(Constants::MAX_NUM_ACTIONS);
        for(i=0;i<LASPlusCurrentX.size();i++)
        {
            tmpContainer.clear();
            for(j=(LASPlusCurrentX[i]+1);j<LAEPlusCurrentX[i];j++)
            {
                if(tmpContainer.find(LTMPlusCurrentX[j]) == tmpContainer.end())
                {
                    XsValues[i].push_back(LTMPlusCurrentX[j]);
                    tmpContainer.insert(LTMPlusCurrentX[j]);
                }
            }
        }

        for(i=0;i<Constants::MAX_KEY_LENGTH;i++)
            valueFrequencyInAllXs[i] = isValueRepeated[i] = 0;

        for(i=0;i<LASPlusCurrentX.size();i++)
        {
            for(j=(LASPlusCurrentX[i]+1);j<LAEPlusCurrentX[i];j++)
            {
                valueFrequencyInAllXs[LTMPlusCurrentX[j]]++;
                if(valueFrequencyInAllXs[LTMPlusCurrentX[j]]>1)
                    isValueRepeated[LTMPlusCurrentX[j]] = 1;
            }
        }

        cycleCounter = conditionCounter = assigmentsCounter = negativeCondCounter = 0;
        pushCounter  = solveCounter = pushVarCounter = PredefinedVarUsedCounter = 0;

        currentScope                   = nActions;
        varScope[Constants::START_VAR] = varScope[Constants::END_VAR] = currentScope;

        for(i=0;i<Constants::MAX_NUM_VARIABLES;i++)
        {
            varSol[i] = Constants::UNDEFINED_VAR;
            varTrueConditioned[i] = false;
        }
        varSol[Constants::START_VAR] = LASPlusCurrentX.back();
        varSol[Constants::END_VAR]   = LAEPlusCurrentX.back();

        varDirection[Constants::START_VAR] = varBasicDirection[Constants::START_VAR] = VAR_RIGHT;
        varDirection[Constants::END_VAR]   = varBasicDirection[Constants::END_VAR] = VAR_LEFT;

        fActionUsed.reset();

        tmpCD.setSourceAction(actionToLearnIdx);
        tmpCD.setType(Constants::POSITIVE_CD);

        
        for(i=3;i<tmpCD.getNumOfNodes();i++)
            tmpCD.removeLastNode();

        //=========================== control parameters
        maxNumNegativeCond    = 2;
        fVariableCD           = true;
        cycleMaxCall          = 11;        //cycles are defined only when (callNo<cycleMaxCall)
        outScopeMaxCall       = 11;        //out of scope variables are used only when  (calNo<outScopeMaxCall)
        //===========================
        maxNumCycle           = Constants::HyperParameters[actionToLearnIdx][1];
        maxNumLTMUsedActions  = Constants::HyperParameters[actionToLearnIdx][2];///
        maxNumValueConditions = Constants::HyperParameters[actionToLearnIdx][3];///
        maxNumAssignments     = Constants::HyperParameters[actionToLearnIdx][4];

        maxNumPush            = Constants::HyperParameters[actionToLearnIdx][6];
        maxNumSolve           = Constants::HyperParameters[actionToLearnIdx][7];
        isAssignmentBeforeAllowed = Constants::HyperParameters[actionToLearnIdx][8];
        //===========================
    }
    // if(tmpCD.getNode(nextNodeIndex-1).getfCycleNode() && cycleCounter>1)
    // {PVV(var);   tmpCD.printCD();   getch();}

    /*
    if(isHighLevelCDsAllowed && pushCounter>1 && solveCounter>0 & actionToLearnIdx == 14)
    {

            PVV(actionToLearnIdx)
            PVV(callNo)
            tmpCD.printCD();
            PMV(GSP)
            getch();
    }//*/
    ///============================ Test & Save =================================================///
    if(isNormalMode)
    {
        //=================== test and save [constant CD]
        ZNodeEqualConst.setZ(-1*Y);
        tmpCD.replaceNode(Constants::Z_INDEX,ZNodeEqualConst);
        CDAnswer = Y;
         //cout<<"\nupdate1\n"<<tmpCD.getNode(nextNodeIndex-1).getfCycleNode();
        if((tmpCD.getNode(nextNodeIndex-1).getfCycleNode()) || solveCounter>0)
        {
            //cout<<"\nupdate2\n";
            CDAnswer == Constants::UNDEFINED_VAR;
            CDAnswer = tmpCD.evaluateCD(LTMPlusCurrentX,LASPlusCurrentX,LAEPlusCurrentX,MGICDs,false,Constants::FIRST_LEVEL_CD,false); //test CD on source action
            for(i=afterPredefinedIdx+1;i<var;i++)
            {

                varSol[i] = tmpCD.getSolOfVar(i);
                if(varSol[i] == Constants::UNDEFINED_VAR)   return;
            }
        }
        //PVV(varSol[8])
        if(CDAnswer == Constants::UNDEFINED_VAR)    return;
        if(CDAnswer == Y)
        {
            fOverBuild = testAndSaveCD(tmpCD,MGICDs,actionToLearnIdx,(PredefinedVarUsedCounter==0)&&(afterPredefinedIdx!=2));
        }
        //=================== test and save [variable CD]
        if(CDAnswer != Constants::UNDEFINED_VAR && fVariableCD)
        {
            for(i=afterPredefinedIdx;i<=lastVar;i++)
            {
                if(valueOfVar(varAvail[i],varSol,LTMPlusCurrentX) != Y)  continue;  //depends on previous call to evaluateCD()
                ZNodeEqualVar.setZ( varAvail[i] );
                tmpCD.replaceNode(Constants::Z_INDEX,ZNodeEqualVar);
                CDAnswer = tmpCD.evaluateCD(LTMPlusCurrentX,LASPlusCurrentX,LAEPlusCurrentX,MGICDs,false,Constants::FIRST_LEVEL_CD,false); //test CD on source action

                if(CDAnswer == Y)
                {
                    testAndSaveCD(tmpCD,MGICDs,actionToLearnIdx,(PredefinedVarUsedCounter==0)&&(afterPredefinedIdx!=2));
                }
            }
        }
        //======================== stop conditions
        if(!fOverBuild)          return;
        //if((var-3) == 6)         return;
        //========================
    }

    /*if(isHighLevelCDsAllowed && actionToLearnIdx==6)
    tmpCD.printCD();

    if(callNo!=0 )
    {
        //cout<<"=========================\n";
        for(i=3;i<var;i++)
        {
            if(varSol[i]<0)
            {
                getch();
                cout<<"varSol Of "<<i<<" "<<(varSol[i])<<"\n";
                tmpCD.printCD();
                PVV(afterPredefinedIdx)
                PVV(var)
            }
        }
        //cout<<"=========================";
        //getch();
    }//*/
    ///==========================================================================================///
    ///=================================[ Build Over CD ]========================================///
    ///==========================================================================================///
    ///==================[Predefined variables: Starts and ends of LTM actions]==================///
    if((callNo - FIRST_Call) < maxNumLTMUsedActions)
    if(((callNo - FIRST_Call) == fActionUsed.count()) && isNormalMode) //to guarantee that the predefined variables are defined before all nodes
    for(i=(callNo - FIRST_Call);i<nActions;i++)
    {
        if(i==actionToLearnIdx || fActionUsed[i]) continue;
        /**/if(i!=0)                                  continue;   //
        tmp1=0;
        for(k=0;k<Xs[i].size();k++)
        {
            if(Xs[i][k] == Y) tmp1++;
            for(j=0;j<X.size();j++)
                if(X[j] == Xs[i][k]) tmp1++;
        }
        if(tmp1==0) continue;

        var+=2;
        lastVar+=2;
        afterPredefinedIdx+=2;

        tmpCD.addPredefinedLTMVar(var-1,LAS[i]);
        varSol[var-1] = LAS[i];
        varScope[var-1] = i;
        varDirection[var-1] = VAR_RIGHT;
        varBasicDirection[var-1] = VAR_RIGHT;
        isVarConstant[var-1] = false;

        tmpCD.addPredefinedLTMVar(var-2,LAE[i]);
        varSol[var-2] = LAE[i];
        varScope[var-2] = i;
        varDirection[var-2] = VAR_LEFT;
        varBasicDirection[var-2] = VAR_LEFT;
        isVarConstant[var-2] = false;

        fActionUsed[i] = true;
        varAvail[lastVar]   = var-1;
        varAvail[lastVar-1] = var-2;
        generatePCDsOfAction(actionToLearnIdx,nextNodeIndex,callNo+1,isNormalMode,firstVarAssign,afterPredefinedIdx,MGICDs);
        fActionUsed[i] = false;

        tmpCD.removePredefinedLTMVar(var-1);
        tmpCD.removePredefinedLTMVar(var-2);
        afterPredefinedIdx-=2;
        lastVar-=2;
        var-=2;
    }
    ///============================[ value conditional Node ]====================================///
    Node tmpNode;
    tmpCD.addNode(tmpNode);
    bool comparisonResult;
    static bool isGoToNormal;
    int  nCycleConditions;

    nCycleConditions = 0;
    if(!isNormalMode)
    {
        nCycleConditions++;
        if(!CSH[callNo-1]) nCycleConditions++;
    }

    if(conditionCounter < maxNumValueConditions && nCycleConditions<=1)
    {
        conditionCounter++;
        for(k=afterPredefinedIdx;k<=lastVar;k++)
        {
            if(nCycleConditions==1)
                tmp1 = tmpCD.getNode(nextNodeIndex-1).getVar1();
            else
                tmp1 =  varAvail[k];
            //if(isVarConstant[tmp1])  continue;

            if(nCycleConditions==1 && tmpCD.getNode(nextNodeIndex-1).getVar2()<0)
                i = -tmpCD.getNode(nextNodeIndex-1).getVar2();
            else
                i = (LASPlusCurrentX[varScope[tmp1]]+1);
            //*/
            //if(varTrueConditioned[tmp1])                                  continue;
            if(varScope[tmp1]!=currentScope && callNo>=outScopeMaxCall )  continue;
            //if(!(actionToLearnIdx == 6 && isHighLevelCDsAllowed))
            //------------------  (var1 == constant)
            for(j=0;j<XsValues[varScope[tmp1]].size();j++)
            {
                if(nCycleConditions==1)
                if(XsValues[varScope[tmp1]][j]>=i)                        continue;

                comparisonResult = ( valueOfVar(tmp1,varSol,LTMPlusCurrentX) == XsValues[varScope[tmp1]][j] );
                isGoToNormal     = (k>=firstVarCond) & comparisonResult & isNormalMode;
                if(!comparisonResult && !isValueRepeated[XsValues[varScope[tmp1]][j]] )   continue;
                if(!isGoToNormal)
                    if(negativeCondCounter>=maxNumNegativeCond ||  cycleCounter>=maxNumCycle)
                        continue;
                tmpNode.setCondition(Constants::COND_VALUE,tmp1,-1*(XsValues[varScope[tmp1]][j]),Constants::SINK_INDEX,Constants::Z_INDEX,comparisonResult);
                tmpCD.replaceNode(nextNodeIndex,tmpNode);

                varTrueConditioned[tmp1] = comparisonResult;
                negativeCondCounter+= !comparisonResult;
                generatePCDsOfAction(actionToLearnIdx,nextNodeIndex+1,callNo+1,isGoToNormal,firstVarAssign,firstVarCond+1,MGICDs);
                negativeCondCounter-= !comparisonResult;
                varTrueConditioned[tmp1] = false;
            }//*/
            //------------------  (var1 == var2)
            for(j=afterPredefinedIdx;j<=lastVar;j++)
            {
                tmp2 = varAvail[j];
                //if(isVarConstant[tmp2])  continue;
                //if(varTrueConditioned[tmp2]) continue;

                if(varScope[tmp2] != currentScope && callNo>=outScopeMaxCall) continue;
                if(tmp1!=tmp2)
                {
                    comparisonResult = (valueOfVar(tmp1,varSol,LTMPlusCurrentX) == valueOfVar(tmp2,varSol,LTMPlusCurrentX));
                    isGoToNormal     = (k>=firstVarCond) & comparisonResult & isNormalMode;
                    if(!isGoToNormal)
                        if(negativeCondCounter>=maxNumNegativeCond ||  cycleCounter>=maxNumCycle)
                            continue;
                    tmpNode.setCondition(Constants::COND_VALUE,tmp1,tmp2,Constants::SINK_INDEX,Constants::Z_INDEX,comparisonResult);
                    tmpCD.replaceNode(nextNodeIndex,tmpNode);

                    varTrueConditioned[tmp1] = comparisonResult;
                    negativeCondCounter+= !comparisonResult;
                    generatePCDsOfAction(actionToLearnIdx,nextNodeIndex+1,callNo+1,isGoToNormal,firstVarAssign,firstVarCond+1,MGICDs);
                    negativeCondCounter-= !comparisonResult;
                    varTrueConditioned[tmp1] = false;
                }
            }//*/
            if(nCycleConditions==1) break;
        }
        conditionCounter--;
    }//*/

    ///=================================[ Assignment Node ]======================================///
    if(assigmentsCounter < maxNumAssignments && isNormalMode)
    {
        var++;  //index of new variable => (var-1)
        lastVar++;
        varAvail[lastVar] = var-1;
        assigmentsCounter++;
        varTrueConditioned[var-1] = false;
        isVarConstant[var-1] = false;
        for(i = firstVarAssign;i<lastVar;i++)
        {
            j = varAvail[i];
            if(isVarConstant[j])  continue;
            if(varScope[j] != currentScope && callNo>=outScopeMaxCall)  continue;
            if(LTMPlusCurrentX[varSol[j]+varDirection[j]]!=Constants::LTM_SEPARATOR)  //assignment before
            {
                k = varDirection[j];
                if(j == 3 || j == 4)    PredefinedVarUsedCounter++;
                if(varDirection[j] != VAR_LEFT)
                {
                    tmpNode.setAssign(VAR_RIGHT,(var-1),j,Constants::Z_INDEX);
                    varSol[(var-1)]       = varSol[j] + VAR_RIGHT;
                    varScope[(var-1)]     = varScope[j];
                    varDirection[(var-1)] = VAR_RIGHT;
                    varBasicDirection[(var-1)] = VAR_RIGHT;

                    tmpCD.replaceNode(nextNodeIndex,tmpNode);
                    generatePCDsOfAction(actionToLearnIdx,nextNodeIndex+1,callNo+1,true,i+1,lastVar,MGICDs);
                }
                if(varDirection[j] != VAR_RIGHT && isAssignmentBeforeAllowed)
                {
                    tmpNode.setAssign(VAR_LEFT,(var-1),j,Constants::Z_INDEX);
                    varSol[(var-1)]       = varSol[j] + VAR_LEFT;
                    varScope[(var-1)]     = varScope[j];
                    varDirection[(var-1)] = VAR_LEFT;
                    varBasicDirection[(var-1)] = VAR_LEFT;

                    tmpCD.replaceNode(nextNodeIndex,tmpNode);
                    if(k == VAR_RIGHT_LEFT)  varDirection[j] = VAR_RIGHT;
                    generatePCDsOfAction(actionToLearnIdx,nextNodeIndex+1,callNo+1,true,i+(k!=VAR_RIGHT_LEFT),lastVar,MGICDs);
                    if(k == VAR_RIGHT_LEFT)  varDirection[j] = VAR_RIGHT_LEFT;
                }//*/
                if(j == 3 || j == 4)    PredefinedVarUsedCounter--;
            }
        }
        assigmentsCounter--;
        var--;
        lastVar--;
    }
    ///==================================[ Push Node  ]===========================================///
    static vector<int>::iterator it;

    if(isHighLevelCDsAllowed && isNormalMode && pushCounter<maxNumPush)
    {
        pushCounter++;
        if(GSP.size() == 0) tmp1 = 0;                else tmp1 = GSPRelatedAction;
        if(GSP.size() == 0) tmp2 = actionToLearnIdx; else tmp2 = GSPRelatedAction+1;

        for(j=tmp1;j<tmp2;j++) //j:action we want to generate //Xs[j][GSP.size()]: value we want to push
        {
            if(GSP.size() > (Xs[j].size()-1)) continue;
            GSPRelatedAction = j;
            //------------------   pushing [variable]
            pushVarCounter++;
            for(i=afterPredefinedIdx;i<=lastVar;i++)
            {
                if(varScope[varAvail[i]] != currentScope && callNo>=outScopeMaxCall)    continue;
                if(Xs[j][GSP.size()] != valueOfVar(varAvail[i],varSol,LTMPlusCurrentX)) continue;
                tmpNode.setPush(varAvail[i],Constants::Z_INDEX);
                tmpCD.replaceNode(nextNodeIndex,tmpNode);

                GSP.push_back(valueOfVar(varAvail[i],varSol,LTMPlusCurrentX));
                generatePCDsOfAction(actionToLearnIdx,nextNodeIndex+1,callNo+1,true,firstVarAssign,firstVarCond,MGICDs);
                GSP.pop_back();
            }
            pushVarCounter--;
            //------------------   pushing [constant]

            if(callNo == FIRST_Call || tmpCD.getNode(nextNodeIndex-1).getNodeType() == Constants::PUSH
               || tmpCD.getNode(nextNodeIndex-1).getNodeType() == Constants::SOLVE)
            {
                tmpNode.setPush(-1*Xs[j][GSP.size()],Constants::Z_INDEX);
                tmpCD.replaceNode(nextNodeIndex,tmpNode);
                i=Xs[j][GSP.size()];
                GSP.push_back(i);
                generatePCDsOfAction(actionToLearnIdx,nextNodeIndex+1,callNo+1,true,firstVarAssign,firstVarCond,MGICDs);
                GSP.pop_back();
            }
            //*/
        }

        //------------------
        pushCounter--;
    }
    ///==================================[ Solve Node ]===========================================///
    if(pushVarCounter!=0)
    if(tmpCD.getNode(nextNodeIndex-1).getNodeType() == Constants::PUSH || tmpCD.getNode(nextNodeIndex-1).getfCycleNode())
    if(isHighLevelCDsAllowed && isNormalMode && (solveCounter < maxNumSolve) && (GSP.size()!=0))
    {
        vector<int> tmpGSP(GSP.size());
        var++;  //index of new variable => (var-1)
        lastVar++;
        varAvail[lastVar] = var-1;
        solveCounter++;
        varTrueConditioned[var-1] = false;
        isVarConstant[var-1] = true;
        varScope[var-1] = actionToLearnIdx;

        tmpNode.setSolve(var-1,Constants::Z_INDEX,Constants::SINK_INDEX);
        tmpCD.replaceNode(nextNodeIndex,tmpNode);

        tmpGSP = GSP;
        GSP.clear();

        tmp1 = pushVarCounter;
        pushVarCounter = 0;
        generatePCDsOfAction(actionToLearnIdx,nextNodeIndex+1,callNo+1,true,firstVarAssign,lastVar,MGICDs);
        pushVarCounter = tmp1;

        GSP = tmpGSP;
        solveCounter--;
        var--;
        lastVar--;
    }

    tmpCD.removeLastNode();
    ///===================================[ cycles ]=============================================///
    //this part adds path to the CD connecting two conditionals and contains assignment node(s)

    vector<int> tmpVarSol;
    if(callNo<cycleMaxCall)
    if(assigmentsCounter < maxNumAssignments)
    if(cycleCounter < maxNumCycle && tmpCD.getNode(nextNodeIndex-1).getNodeType() == Constants::COND_VALUE)
    {
        //nextNodeIndex-1 ==> path head
        static Node nodeUpdate1, nodeUpdate2;
        bool tmpVarTrueConditioned;
        int x,y,xTmpDir,yTmpDir;

        x = tmpCD.getNode(nextNodeIndex-1).getVar1();
        k=negativeCondCounter;
        varAvail[lastVar+1] = x;
        tmpVarTrueConditioned = varTrueConditioned[x];

        varTrueConditioned[x] = !tmpVarTrueConditioned;
        cycleCounter ++;
        tmpVarSol = varSol;
        for(i=nextNodeIndex-1;i>max(nextNodeIndex-4,3);i--) //loop on possible (path tail)
        {
            if(isVarConstant[x])  break;
            if(tmpCD.getNode(i).getfCycleNode())
            {
                while(tmpCD.getNode(i).getfCycleNode())
                    i = tmpCD.getNode(i).getNextNode();
            }
            else if(i==(nextNodeIndex-3))
                continue;

            if(tmpCD.getNode(i).getNodeType() != Constants::COND_VALUE &&
               tmpCD.getNode(i).getNodeType() != Constants::PUSH        )        break;
            if((nCycleConditions == 2) && (i==(nextNodeIndex-1)))             continue;
            //================= add path with one update node
            nodeUpdate1.setAssign(varBasicDirection[x],x,x,i);

            xTmpDir = varDirection[x];
            varDirection[x] = VAR_RIGHT_LEFT;
            tmpCD.addUpdateNode(nodeUpdate1);
            tmp1 = varSol[x];
            negativeCondCounter = 0;

            tmpCD.setCycleNodeFlag(nextNodeIndex,true);

            lastVar++; assigmentsCounter++;

            generatePCDsOfAction(actionToLearnIdx,nextNodeIndex+1,callNo+1,true,lastVar,lastVar,MGICDs);

            tmpCD.setCycleNodeFlag(nextNodeIndex,false);
            //================= add path with two update nodes
            if(assigmentsCounter < maxNumAssignments)
            {
                assigmentsCounter++; lastVar++;

                for(y=afterPredefinedIdx+1;y<var;y++)  //afterPredefinedIdx+1 ==> neglect predefined variables & z
                {
                    if(y==x)   continue;
                    if(isVarConstant[y])  continue;
                    nodeUpdate2.setAssign(varBasicDirection[y],y,y,i);
                    varAvail[lastVar] = y;
                    yTmpDir = varDirection[y];
                    varDirection[y] = VAR_RIGHT_LEFT;
                    tmpCD.addUpdateNode(nodeUpdate2);
                    tmp2 = varSol[y];
                    tmpCD.setCycleNodeFlag(nextNodeIndex+1,true);
                    generatePCDsOfAction(actionToLearnIdx,nextNodeIndex+2,callNo+1,true,lastVar-1,lastVar-1,MGICDs);
                    varSol[x]=tmp1;
                    varSol[y]=tmp2;
                    tmpCD.setCycleNodeFlag(nextNodeIndex+1,false);
                    tmpCD.removeUpdateNode();
                    varDirection[y] = yTmpDir;
                    varSol = tmpVarSol;
                }//*/
                assigmentsCounter--; lastVar--;
            }//*/
            //====================
            assigmentsCounter--;
            lastVar--;
            negativeCondCounter = k;
            varSol[x]=tmp1;
            tmpCD.removeUpdateNode();
            varDirection[x] = xTmpDir;
            varSol = tmpVarSol;
        }
        cycleCounter --;
        //if(tmpCD.getNode(nextNodeIndex-1).getNextNode(true) == Constants::SINK_INDEX)
        varTrueConditioned[x] = tmpVarTrueConditioned;
    }
    //*/
    //-------------------------------------
}
void Generator::prepareActions(const vector< vector<int> > &actions)
{
    int i,j,k;
    ///------------------------------- prepare Actions
    for(i=0,nActions=0;i<actions.size();i++)
    {
        if(i==0)   {LAS.push_back(0);    LTM.push_back(int(Constants::LTM_SEPARATOR));}
        else        LAS.push_back(LTM.size()-1);

        for(j=0;j<actions[i].size();j++)
        {
            LTM.push_back(actions[i][j]);
        }
        LTM.push_back(int(Constants::LTM_SEPARATOR));
        LAE.push_back(LTM.size()-1);
        nActions++;

        Xs.push_back(actions[i]);
        Ys.push_back(actions[i].back());
        Xs[i].pop_back();

    }

    for(i=0;i<nActions;i++)
    {
        LTMPlusX.push_back(LTM);
        LASPlusX.push_back(LAS);
        LAEPlusX.push_back(LAE);

        LASPlusX[i].push_back(LAE.back());
        LAEPlusX[i].push_back(LAE.back() + Xs[i].size() + 1);
        for(j=0;j<Xs[i].size();j++)
            LTMPlusX[i].push_back(Xs[i][j]);
        LTMPlusX[i].push_back(int(Constants::LTM_SEPARATOR));
    }
}

void Generator::learnActions(const vector< vector<int> > &actions)
{
    int i,j,k,l,CDLevel;
    //isHighLevelCDsAllowed
    vector<CD> previousMGICDs,tmpPreviousMGICDs;
    prepareActions(actions);
    int nLevels = Constants::HyperParameters[0][5];
    for(CDLevel=1;CDLevel<=nLevels;CDLevel++)
    {
        nPCDGeneratorCalls = nMGICDGeneratorCalls = 0;
        if(CDLevel==1)
            isHighLevelCDsAllowed = false;
        else
            isHighLevelCDsAllowed = true;
        ///------------------------------- generate PCDs
        t = clock();
        for(i=0;i<nActions;i++)
        {
            //----------------------------------- solution of dependency problem
            tmpPreviousMGICDs.clear();
            if(isHighLevelCDsAllowed)
            for(j=0;j<i;j++)
                //if(previousMGICDsVCDA[j][i] == false)
                    tmpPreviousMGICDs.push_back(previousMGICDs[j]);
            //-----------------------------------
            cout<<i<<" ";
            generatePCDsOfAction(i,3,FIRST_Call,true,dummy,dummy,tmpPreviousMGICDs);
        }
        cout<<"PCDs => ";
        t = clock() - t;
        PCDsGenerationTime = (((float)t)/CLOCKS_PER_SEC);
        for(i=0,nVPCDs=0;i<PCDs.size();i++)
            if(validate(PCDA[i]))  nVPCDs++;
        ///------------------------------- generate VCDs (VPCDs & VHCDs)
        vector<int> tempHCD ;
        vector<int> accumaltedCDARow;
        int NCDStartIndex, PCDIndex;
        t = clock();
        generateVCDsOfActions(FIRST_Call ,tempHCD ,accumaltedCDARow,NCDStartIndex,PCDIndex,0);
        cout<<"VCDs "<<VCDs.size()<<" => ";
        t = clock() - t;
        VCDsGenerationTime = (((float)t)/CLOCKS_PER_SEC);

        //cout<<"\n"<<VCDA.size()<<"\n";
        /*for(i=0;i<VCDA.size();i++)
        {
            cout<<VCDA[i]<<"\n";
        }*/
        ///------------------------------- generate MGICD (final model)
        bitset<Constants::MAX_NUM_ACTIONS> solvedActions;
        gamma = calculateGamma();
        cout<<"Gamma "<<gamma<<" ==> ";
        //cout<<gamma;
        t = clock();
        if(CDLevel==nLevels)
            generateMGICDsOfActions(FIRST_Call,gamma,false);
        else
            generateMGICDsOfActions(FIRST_Call,gamma,true);
        t = clock() - t;
        cout<<"MGICDs\n";
        MGICDsGenerationTime = (((float)t)/CLOCKS_PER_SEC);
        ///------------------------------
        previousMGICDs = MGICDs;
        previousMGICDsVCDA = MGICDsVCDA;

        cout<<"\nCharacteristic vectors of VCDs in the MGICDs:\n";
        for(i=0;i<MGICDsVCDA.size();i++)
        {
            for(j=0;j<nActions;j++)
                cout<<MGICDsVCDA[i][j];
            cout<<"\n";
        }
        //*/
        if(CDLevel!=nLevels)
            clearSomeVariables();
        ///------------------------------
    }
}


//generate VCDs using PCDs
void Generator::generateVCDsOfActions(int call_no ,vector<int> tempHCD , vector<int> accumaltedCDARow,int NCDStartIndex,int PCDIndex,int numOfWA)
{
    if(call_no > Constants::HyperParameters[0][0]) return;
    bitset<Constants::MAX_NUM_ACTIONS> tmpset;
    static int nWA,nRA;
    int i,j,k;
    static bitset<Constants::MAX_NUM_PCDS> highLevelCDs;
    //------------------------------------------- add positive CD
    if(call_no == FIRST_Call)
    {
        /*for(int i=0;i<PCDs.size();i++)
        {
            cout<<"("<<i<<") ";
            for(int j=0;j<nActions;j++)   cout<<PCDA[i][j];
            cout<<"\n";
        }*/
        for(i=0;i<PCDs.size();i++)
            if(PCDs[i].isHighLevel())
                highLevelCDs[i] = true;
            else
                highLevelCDs[i] = false;

        vector<int> tmpVPCD (1);
        for(i=0;i<PCDs.size();i++)
        {
            if(validate(PCDA[i]))  //if PCD is valid then save it
            {
                tmpVPCD[0] = i;
                VCDs.push_back(tmpVPCD);

                for(j = 0;j<nActions;j++)
                    if(PCDA[i][j] == Constants::RIGHT_ANSWER)  tmpset[j] = 1;
                    else                                       tmpset[j] = 0;
                VCDA.push_back(tmpset);
            }
            else                         //if CD is invalid then overbuild it
            {
                nWA = 0;
                tempHCD.push_back(i);
                for(j=0;j<PCDA[i].size();j++)
                    if(PCDA[i][j] == Constants::WRONG_ANSWER) nWA++;
                generateVCDsOfActions(call_no+1,tempHCD,PCDA[i],0,i,nWA);
                tempHCD.pop_back();

            }
        }
    }
    //------------------------------------------- add negative CDs
    vector<int> newRow;
    if(call_no!=FIRST_Call)
    {
        bool fAllNCDsUseful;
        for(i=NCDStartIndex;i<PCDs.size();i++)
        {
            nWA=nRA=0;
            if(i == PCDIndex)   continue;
            //if(highLevelCDs[i]) continue;
            if(highLevelCDs[PCDIndex]) continue;
            //--------------------------- test new HCD
            newRow = addPCDARows(accumaltedCDARow,PCDA[i]);
            tempHCD.push_back(i);

            for(j=0;j<newRow.size();j++)
                if     (newRow[j] == Constants::WRONG_ANSWER) nWA++;
                else if(newRow[j] == Constants::RIGHT_ANSWER) nRA++;
            //---------- to check if one of the previous NCDs add nothing to HCD
            fAllNCDsUseful=true;
            for(j=1;j<(tempHCD.size()-1);j++) //loop on NCDs except the last one
            {
                //PCDA[i][k] Vs PCDA[tempHCD[j]][k]
                fAllNCDsUseful=false;
                for(k=0;k<nActions;k++)
                {
                    if(PCDA[tempHCD[0]][k] == Constants::WRONG_ANSWER)
                    {
                        //if this NCD is useful in negating action (k)
                        if(PCDA[tempHCD[j]][k] != Constants::RIGHT_ANSWER &&
                            PCDA[i][k]==Constants::RIGHT_ANSWER)
                        fAllNCDsUseful=true;
                    }
                }
                if(fAllNCDsUseful==false)  break;
            }
            //----------
            if(nRA>=1 && nWA==0 && fAllNCDsUseful==true)  //if HCD is valid then save it
            {
                for(j = 0;j<nActions;j++)
                    if(newRow[j] == Constants::RIGHT_ANSWER)
                        tmpset[j] = 1;
                    else
                        tmpset[j] = 0;
                //---------------
                if(tempHCD.size() <= tmpset.count())
                {
                    VCDs.push_back(tempHCD);
                    VCDA.push_back(tmpset);
                }
            }
            else if(nRA>=1 && nWA<numOfWA && fAllNCDsUseful==true)
            {
                    generateVCDsOfActions(call_no+1,tempHCD,newRow,i+1,PCDIndex,nWA);
            }
            tempHCD.pop_back();
        }
    }
    if(call_no == FIRST_Call)
    {
        bitset<Constants::MAX_NUM_ACTIONS> solvedActions(nActions);
        for(i=0;i<nActions;i++)
            solvedActions[i] = false;
        for(j=0;j<VCDs.size();j++)
            for(i=0;i<nActions;i++)
                if(VCDA[j][i]) solvedActions[i] = true;

        if(solvedActions.count() != nActions)
        {
            cout<<"\n###########################################\n";
            cout<<"VCDs can't solve all Sequences";
            cout<<"\n###########################################\n";
        }
    }
}

//VCDs ==> MGICD
void Generator::generateMGICDsOfActions(int call_no, int gamma,bool isSingleOption,bitset<Constants::MAX_NUM_ACTIONS> solvedActions)
{
    static vector<int> bestICDs;    
    static int minimumNodes = INF;  //this function choose the ICD with minumum number of nodes
    static int nNodesOfICD = 0;     //number of nodes of PCDs in ICDs ,each PCD is counted once
    static bitset<Constants::MAX_NUM_PCDS> PCDsUsed;
    static bitset<Constants::MAX_NUM_PCDS> PCDsUsedAsPComponent;
    const  bitset<Constants::MAX_NUM_ACTIONS> ZEROS_ACTIONS;
    static const int nNodesIdx = 0;
    static const int maxSharedNodesIdx = 1;
    static int VCDsProp[Constants::MAX_NUM_VCDS][2]; //to save two properties for each CD:nNodes , max shared nodes
    static vector<vector<int> > VCDGroups(nActions);
    static vector<int> VCDsUsed;
    int i,j,k;
    nMGICDGeneratorCalls++;
    if(nNodesOfICD > gamma )    return;
    /*
    for(i=0;i<nActions;i++)
        cout<<solvedActions[i];
    cout<<"    ";
    for(i=0;i<VCDsUsed.size();i++)
        cout<<VCDsUsed[i]<<" ";
    cout<<"\n";
    getch();
    */
    assert(nActions    < Constants::MAX_NUM_ACTIONS);
    assert(PCDs.size() < Constants::MAX_NUM_PCDS);
    assert(VCDs.size() < Constants::MAX_NUM_VCDS);
    //cout<<"call :"<<call_no<<"   \n";

    if(call_no == FIRST_Call)
    {
        nMGICDGeneratorCalls=0;
        bestICDs.clear();
        minimumNodes = INF;
        nNodesOfICD = 0;
        PCDsUsed.reset();  
        VCDsUsed.clear();
        VCDGroups.clear();
        VCDGroups.resize(nActions);
        PCDsUsedAsPComponent.reset();
        //---------------------
        bitset<Constants::MAX_NUM_PCDS> PCDsUsedOnce;
        bitset<Constants::MAX_NUM_PCDS> PCDsUsedTwiceOrMore;
        if(isSingleOption)  //one VCD for each sequence
        {
            MGICDs.clear();
            nNodesInMGICDs = -10;
            for(j=0;j<nActions;j++)
            {
                for(i=0;i<VCDs.size();i++)
                {
                    if(VCDA[i][j] && VCDA[i].count()==1)
                       bestICDs.push_back(i);
                }
            }

            for(i=0;i<bestICDs.size();i++)
            {
                CD HCD;

                //cout<<VCDA[bestICDs[i]]<<"\n";
                MGICDsVCDA.push_back(VCDA[bestICDs[i]]);
                if(VCDs[bestICDs[i]].size()==1)   //add VPCD to MGICD
                    MGICDs.push_back(PCDs[ VCDs[ bestICDs[i] ][0] ]);
                else                              //add VHCD to MGICD
                {
                    for(j=0;j<VCDs[bestICDs[i]].size();j++)
                        HCD.addCDToHCD(PCDs[VCDs[bestICDs[i]][j]]);
                    MGICDs.push_back(HCD);
                }
            }
            return;
        }



        /*
        for(i=0;i<VCDs.size();i++)
        {
            for(j=0;j<nActions;j++) cout<<VCDA[i][j];
            cout<<"   ";
            for(j=0;j<VCDs[i].size();j++) cout<<VCDs[i][j]<<" ";
            cout<<"\n";
        }
        getch();//*/
        ///------------------------------------------
        //calculates nNodes for each VCD
        /*for(i=0;i<VCDs.size();i++)
        {
            for(k=0,j=0;j<VCDs[i].size();j++) //loop on PCDs in VCD i
            {
                k+=PCDs[VCDs[i][j]].getNumOfNodes();
                assert(k!=0);
                if(PCDsUsedOnce[VCDs[i][j]])
                    PCDsUsedTwiceOrMore[VCDs[i][j]] = 1;
                else
                    PCDsUsedOnce[VCDs[i][j]] = 1;
            }
            assert(k!=0);
            VCDsProp[i][nNodesIdx] = k;
        }
        ///------------------------------------------
        //calculates max shared nodes for each VCD
        for(i=0;i<VCDs.size();i++)  //step 1
        {
            for(k=0,j=0;j<VCDs[i].size();j++) //loop on PCDs in VCD i
            {
                if(PCDsUsedTwiceOrMore[VCDs[i][j]])
                    k+=PCDs[VCDs[i][j]].getNumOfNodes();
            }
            VCDsProp[i][maxSharedNodesIdx] = k;
        }*/
        ///------------------------------------------
        bitset<Constants::MAX_NUM_VCDS> VCDsUsedInPreviousGroup;
        for(i=0;i<VCDs.size();i++)
        {
            for(j=0;j<nActions;j++)
            {
                ///this condition !VCDsUsedInPreviousGroup[i] is right:
                ///assuming no intersections between VCDA vectors of VCDs in MGICD
                if(VCDA[i][j] && !VCDsUsedInPreviousGroup[i])
                {
                    VCDsUsedInPreviousGroup[i] = 1;
                    VCDGroups[j].push_back(i);
                }

            }
        }
        ///------------------------------------------
      
    }

    //-----------------------------------( save ICD )
    if(call_no == nActions  && solvedActions.count() == nActions)
    {
        if(nNodesOfICD <= minimumNodes)
        {
            minimumNodes = nNodesOfICD;
            bestICDs = VCDsUsed;
        }
    }
    //-----------------------------------( add VCD to ICD )
    else
    {
        bitset<Constants::MAX_NUM_PCDS> tmpPCDsUsed;
        tmpPCDsUsed = PCDsUsed;

        if(!solvedActions[call_no])
            for(i=0;i<VCDGroups[call_no].size();i++)
            {
                if(PCDsUsedAsPComponent[VCDs[VCDGroups[call_no][i]][0]]) continue;
                ///assuming no intersections between VCDA vectors of VCDs in MGICD
                if((solvedActions & VCDA[VCDGroups[call_no][i]]) != ZEROS_ACTIONS) continue;
                if((solvedActions | VCDA[VCDGroups[call_no][i]]) == solvedActions) continue;


                VCDsUsed.push_back(VCDGroups[call_no][i]);
                k=0;
                for(j=0;j<VCDs[VCDGroups[call_no][i]].size();j++)
                    if(!PCDsUsed[VCDs[VCDGroups[call_no][i]][j]])
                    {
                        k += PCDs[VCDs[VCDGroups[call_no][i]][j]].getNumOfNodes();
                        PCDsUsed[VCDs[VCDGroups[call_no][i]][j]] = 1;
                    }
                nNodesOfICD+=k;
                PCDsUsedAsPComponent[VCDs[VCDGroups[call_no][i]][0]] = 1;

                generateMGICDsOfActions(call_no+1,gamma,isSingleOption,(solvedActions|VCDA[VCDGroups[call_no][i]]));

                PCDsUsedAsPComponent[VCDs[VCDGroups[call_no][i]][0]] = 0;
                VCDsUsed.pop_back();
                PCDsUsed = tmpPCDsUsed;
                nNodesOfICD-=k;
            }
        else
            generateMGICDsOfActions(call_no+1,gamma,isSingleOption,solvedActions);
    }//*/
    //------------------------------ ( saving MGICD : ICDs with least number of nodes )
    if(call_no == FIRST_Call)
    {
        MGICDs.clear();
        nNodesInMGICDs = minimumNodes;
        for(i=0;i<bestICDs.size();i++)
        {
            CD HCD;

            //cout<<VCDA[bestICDs[i]]<<"\n";
            MGICDsVCDA.push_back(VCDA[bestICDs[i]]);
            if(VCDs[bestICDs[i]].size()==1)   //adding VPCD to MGICD
                MGICDs.push_back(PCDs[ VCDs[ bestICDs[i] ][0] ]);
            else                              //adding VHCD to MGICD
            {
                for(j=0;j<VCDs[bestICDs[i]].size();j++)
                    HCD.addCDToHCD(PCDs[VCDs[bestICDs[i]][j]]);
                MGICDs.push_back(HCD);
            }
        }
    }
}




vector<CD> Generator::getMGICDs()
{
    return MGICDs;
}

int Generator::getnNodesInMGICDs()
{
    return nNodesInMGICDs;
}

vector<CD> Generator::getPCDs()
{
    return PCDs;
}

vector< vector<int> > Generator::getPCDA()
{
    return PCDA;
}

inline bool Generator::testAndSaveCD(CD &newCD,vector<CD> &MGICDs,int &actionToLearnIdx,bool isPredefinedUnused)
{
    vector<int> CDA(nActions,0);
    static int ansOfCD;
    static int nRightAnswers,nWrongAnswers;
    static int i,j,k;
    static bool fUnusedNodeExist;
    static bool fSave;
    static bool fOverbuild;    //true ==> the function retruns true to stop overbuilding
    fSave = true;   fOverbuild = true;

    nRightAnswers =0, nWrongAnswers =0;
    newCD.resetIsUsedFlags();
    //----------------------------------------------- calculate characteristic vector
    //----------
    for(j=0;j<MGICDs.size();j++)
        MGICDs[j].resetIsUsed();
    //----------

    for(i=0;i<nActions;i++)
    {
        ansOfCD = newCD.evaluateCD(LTMPlusX[i],LASPlusX[i],LAEPlusX[i],MGICDs,false,Constants::FIRST_LEVEL_CD,false);
        if(ansOfCD == Constants::UNDEFINED_VAR)
        {
            CDA[i] = (Constants::UNDEFINED_VAR);
        }
        else if(ansOfCD == Ys[i])
        {
            CDA[i] = (Constants::RIGHT_ANSWER);    nRightAnswers++;
        }
        else
        {
            CDA[i] = (Constants::WRONG_ANSWER);    nWrongAnswers++;
        }
    }
    if(isPredefinedUnused)
        fSave = false;

    if(MGICDs.size()!=0)
    {
        for(k=0;k<actionToLearnIdx;k++)
        {
            if(MGICDs[k].getIsUsed() && previousMGICDsVCDA[k][k] == true && CDA[k] == Constants::RIGHT_ANSWER)
                {
                    fSave = false;
                }
        }
    }//*/
    //----------
    /*if(CDA[14] == Constants::RIGHT_ANSWER && CDA[13] == Constants::RIGHT_ANSWER &&
       CDA[12] == Constants::RIGHT_ANSWER && CDA[11] == Constants::RIGHT_ANSWER)
    {
        for(i=0;i<15;i++)
            cout<<CDA[i]<<" ";
        cout<<"\n";
        newCD.printCD();
        getch();
    }*/

    if(nRightAnswers == 1 && nWrongAnswers == 0)    fOverbuild = false;
    //----------------------------------------------- detect unused nodes in CD
    //last conditional node can be useless until getting in cycle
    for(i=0,fUnusedNodeExist=true;i<(newCD.getNumOfNodes()-1);i++)
    {
        fUnusedNodeExist = fUnusedNodeExist * newCD.getNode(i).IsNodeUsed();
    }

    if(newCD.getNode((newCD.getNumOfNodes()-1)).IsNodeUsed() == false)
        fSave = false;
    if(fUnusedNodeExist==false) //if one node has never been used while testing all actions
    {
        fSave = false;
        fOverbuild = false;
    }
    //----------------------------------------------- save PCD
    /*if( newCD.getNumOfNodes() == 14 && newCD.isHighLevel() &&nRightAnswers >4)
    {
        PVV(nRightAnswers)
        PVV(nWrongAnswers)
        newCD.printCD();
        getch();
    }
    if(nRightAnswers == 9)
    {
        PVV(nWrongAnswers)
        newCD.printCD();
        PVV(fSave)
    }*/
    if(fSave)
    {
        ///the next condition doesn't allow to save more than one CD with same permutation of CDA vector
        if((permutations.find(CDA) == permutations.end())) //permutation doesn't exist
        {
            PCDA.push_back(CDA);
            PCDs.push_back(newCD);
            permutations.insert(make_pair(CDA,make_pair(PCDs.size()-1,newCD.getNumOfNodes())));
        }
        else if(newCD.getNumOfNodes() < permutations[CDA].second) //if permutation exist with more number of nodes
        {
            //this conditional is extremely important to get the more generalized CDs
            permutations[CDA].second = newCD.getNumOfNodes();
            PCDs[permutations[CDA].first] = newCD;
        }
    }
    //-----------------------------------------------
    return fOverbuild;
}

void Generator::clearSomeVariables()
{
    MGICDsVCDA.clear();
    MGICDs.clear();           //final model
    PCDs.clear();
    PCDA.clear();
    VCDs.clear();            
    VCDA.clear();
    permutations.clear(); //(permutaion ,(idx in PCDs,NumOfNodes) )
}

int Generator::getNumOfPCDs()
{
    return PCDs.size();
}


int Generator::getNumOfVCDs()
{
    return VCDs.size();
}


vector<vector<int> > Generator::getVCDs()
{
    return VCDs;
}


vector<bitset<Constants::MAX_NUM_ACTIONS> > Generator::getVCDA()
{
    return VCDA;
}

vector<int> Generator::getLTM()
{
    return LTM;
}
vector<int> Generator::getLAS()
{
    return LAS;
}
vector<int> Generator::getLAE()
{
    return LAE;
}

int Generator::getnPCDGeneratorCalls()
{
    return nPCDGeneratorCalls;
}
int Generator::getnMGICDGeneratorCalls()
{
    return nMGICDGeneratorCalls;
}
int Generator::getnVPCDs()
{
    return nVPCDs;
}
int Generator::getgamma()
{
    return gamma;
}
float Generator::getPCDsGenerationTime()
{
    return PCDsGenerationTime;     //in seconds
}
float Generator::getVCDsGenerationTime()
{
    return VCDsGenerationTime;     //in seconds
}
float Generator::getMGICDsGenerationTime()
{
    return MGICDsGenerationTime;   //in seconds
}
