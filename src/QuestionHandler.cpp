#include "QuestionHandler.h"
#include "CD.h"
#include "IOHandler.h"


//print variable
#define PVV(x)  cout<<#x<<" = "<<x<<"\n";
//print vector content
#define PMV(x)  cout<<"\n===========\n"; {for(int PMVC=0;PMVC<x.size();PMVC++) PVV(x[PMVC]);} cout<<"===========\n";



QuestionHandler::QuestionHandler()
{
}

QuestionHandler::~QuestionHandler()
{
    //dtor
}

int QuestionHandler::solveQuestion(vector<CD> &MGICDs,vector<int> &LTM,vector<int> &LAS,vector<int> &LAE)
{
    int ans;
    int questionMatchedVCD;

    possibleAnswers.clear();

    for(int i=0;i<MGICDs.size();i++)  //loop on VCDs
    {
        vector<CD> tmpMGICDs = MGICDs;
        ans = MGICDs[i].evaluateCD(LTM,LAS,LAE,question,tmpMGICDs,false,Constants::FIRST_LEVEL_CD,true);

        if(ans != Constants::UNDEFINED_VAR)
        {
            questionMatchedVCD = i;
            if(find(possibleAnswers.begin(),possibleAnswers.end(),ans) == possibleAnswers.end())
                possibleAnswers.push_back(ans);
            cout<<"\nMatched VCD: "<<questionMatchedVCD<<"\n";
        }
    }
    //------------------------------ animation
    if(possibleAnswers.size() == 1)
    {
        IOHandler IOH;
        Memo *memory = Memo::getInstance();
        vector<CD> tmpMGICDs = MGICDs;

        set<int> predefinedValues = MGICDs[questionMatchedVCD].getPredefinedValues();
        for (set<int>::iterator it=predefinedValues.begin(); it!=predefinedValues.end(); ++it)
        {
            IOH.addToAnimation(memory->getKey(*it));
            IOH.addToAnimation(" ");
        }

        IOH.addToAnimation("\n");
        if(MGICDs[questionMatchedVCD].getType() == Constants::POSITIVE_CD)
            IOH.addToAnimation("PositivePart\n");
        MGICDs[questionMatchedVCD].evaluateCD(LTM,LAS,LAE,question,tmpMGICDs,true,Constants::FIRST_LEVEL_CD,true);
    }
    //------------------------------
    if(possibleAnswers.size() == 1)
        return possibleAnswers[0];
    else
        return Constants::UNDEFINED_VAR;
}

void QuestionHandler::readQuestion()
{
    cout<<"\nEnter the question :";
    question.clear();

    string line;
    std::getline(std::cin,line);
    std::stringstream stream(line);
    while(1) {
       int n;
       stream >> n;
       if(!stream)     break;
       question.push_back(n);
    }
}

void QuestionHandler::setQuestion(vector<int> action)
{
    question = action;
}

vector<int>  QuestionHandler::getQuestion()
{
    return question;
}

vector<int>  QuestionHandler::getPossibleAnswers()
{
    return possibleAnswers;
}
