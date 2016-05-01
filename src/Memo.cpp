#include "Memo.h"
#include <assert.h>
static Memo *instance = NULL;

Memo::Memo()
{
    //ctor
}

Memo::~Memo()
{
    //dtor
}


Memo* Memo::getInstance(){
    if(instance == NULL)
        instance = new Memo;
    return instance;
}
//----------------------------------
vector<int> Memo::getAction(int index){
    return actions.at(index);
}

CD Memo::getCD(int index){
    return CDs.at(index);
}

int Memo::getValue(string key)
{
    assert(keysStrToInt.find(key) != keysStrToInt.end());
    return keysStrToInt[key];
}

string Memo::getKey(int value){
    assert(keysIntToStr.find(value) != keysIntToStr.end());
    assert(value > 0);        //key is index 1 based
    return keysIntToStr[value];
}

vector<int> Memo::getSentence(int index)
{
    return sentences[index];
}

short Memo::getCDAfromTable(int indexOfCD, int indexOfAction){
    return table_CDA[indexOfCD][indexOfAction];
}
int Memo::numOfCDs(){
    return CDs.size();
}

int Memo::numOfActions()
{
    return actions.size();
}

int Memo::numOfSentences()
{
    return sentences.size();
}

int Memo::numOfKeys()
{
    return keysStrToInt.size();
}
int Memo::numOfDatasets()
{
    return datasets.size();
}

//------------------------------------
void Memo::addCD(CD cd){
    CDs.push_back(cd);
}

void Memo::addKey(string NewItemValue){
    if(keysStrToInt.find(NewItemValue) == keysStrToInt.end()){
        keysIntToStr.insert(pair<int, string>(keysIntToStr.size()+1, NewItemValue));
        keysStrToInt.insert(pair<string, int>(NewItemValue, keysStrToInt.size()+1));
   }
}

void Memo::addRowInCDsVsActionsTable(vector<int> row){
    table_CDA.push_back(row);
}


void Memo::addSentence(string newSentence)
{
    //save sentence
    /*
    istringstream itemStream1(newSentence);
    string item;
    vector<int> sentence;
    while(!itemStream1.eof())
    {
        itemStream1>>item;
        addKey(item);
        sentence.push_back(getValue(item));
    }
    sentences.push_back(sentence);
    //saving relative actions
    istringstream itemStream2(newSentence);
    string action;

    itemStream2>>item;
    action = item;
    while(!itemStream2.eof())
    {
        itemStream2>>item;
        action = action+" "+item;
        addAction(action);
    }*/
}

void Memo::addAction(string newAction){
    istringstream itemStream(newAction);
    string item;
    vector<int> action;
    while(!itemStream.eof())
    {
        itemStream>>item;
        addKey(item);
        action.push_back(getValue(item));
    }
    actions.push_back(action);
}

void Memo::addDatasetQuestionsAnswers(vector<string > dataset , vector<string> questions ,vector <string> answers)
{
    int index = datasets.size();
    datasets.resize(datasets.size()+1);
    questionsOfDatasets.resize(datasets.size()+1);
    answersToQuestions.resize(datasets.size()+1);

    for(int i=0;i<dataset.size();i++)    //loop on actions in dataset
        datasets[index].push_back(convertAction(dataset[i]));
    for(int i=0;i<questions.size();i++)  //loop on questions in datdataset.size()aset
        questionsOfDatasets[index].push_back(convertAction(questions[i]));
    for(int i=0;i<answers.size();i++)    //loop on answers in dataset
    {
        addKey(answers[i]);
        answersToQuestions[index].push_back(getValue(answers[i]));
    }
}

vector<int> Memo::convertAction(string action)
{
    vector<int> actionKeys;
    string item;
    istringstream itemStream(action);

    while(1)
    {        
        itemStream>>item;
        if(itemStream.eof()) 
            break;
        addKey(item);
        actionKeys.push_back(getValue(item));
    }

    for(int i=0;i<actionKeys.size();i++)
    return actionKeys;
}

vector<vector<int> > Memo::getDataset(int index)
{
    return datasets[index];
}

vector<vector<int> > Memo::getQuestions(int index)
{
    return questionsOfDatasets[index];
}

vector<int> Memo::getAnswers(int index)
{
    return answersToQuestions[index];
}

void Memo::clearMemory()
{
    table_CDA.clear();
    CDs.clear();
    keysStrToInt.clear();
    keysIntToStr.clear();
    actions.clear();
    sentences.clear();

    datasets.clear();
    questionsOfDatasets.clear();
    answersToQuestions.clear();
}
//---------------------------------------------
