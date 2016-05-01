
#ifndef MEMO_H
#define MEMO_H

#include "CD.h"
#include "Utilities.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

class Memo
{
    public:
        static Memo* getInstance();

        vector<int> getAction(int index);     // index 0 based
        CD getCD(int index);
        int getValue(string key);  //map:(key value = string) ==> (mapped value = integer)
        string getKey(int value);
        vector<int> getSentence(int index);
        short getCDAfromTable(int indexOfCD, int indexOfAction);
        vector<vector<int> > getDataset(int index);
        vector<vector<int> > getQuestions(int index);
        vector<int> getAnswers(int index);
        int numOfCDs();
        int numOfActions();
        int numOfSentences();
        int numOfKeys();
        int numOfDatasets();


        void addCD(CD cd);
        void addKey(string NewItemValue);
        void addRowInCDsVsActionsTable(vector<int> row); 
        void addAction(string newAction);
        void addSentence(string newSentence);
        void addDatasetQuestionsAnswers(vector<string > dataset , vector<string> questions ,vector <string> answers);
        vector<int> convertAction(string action);
        void clearMemory();

        virtual ~Memo();
    protected:
    private:
        vector < vector<int> > table_CDA;
        vector < CD > CDs;
        map < string ,int > keysStrToInt;
        map < int,string >  keysIntToStr;
        vector< vector<int> > actions;
        vector< vector<int> > sentences;

        vector<vector<vector <int> > > datasets;
        vector<vector<vector <int> > > questionsOfDatasets;
        vector<vector<int> > answersToQuestions;

        Memo();
};

#endif // MEMO_H
