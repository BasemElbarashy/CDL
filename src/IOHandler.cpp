#include "IOHandler.h"
#include <assert.h>
bool IOHandler::fActionsLoaded = false;
Memo *memory = Memo::getInstance();

IOHandler::IOHandler()
{
    IOHandler::fActionsLoaded = false;
}

IOHandler::~IOHandler()
{

}
//------------------------------
void IOHandler::loadActionsFromFile(string FilePath)
{
    ifstream inFile(FilePath.c_str());
    string action;

    assert(fActionsLoaded != true);
    fActionsLoaded = true;

    while (getline(inFile, action))
        memory->addAction(action);
    inFile.close();
}

void IOHandler::loadSentencesFromFile(string FilePath)
{
    ifstream inFile(FilePath.c_str());
    string sentence;

    assert(fActionsLoaded != true);
    fActionsLoaded = true;

    while (getline(inFile, sentence))
        memory->addSentence(sentence);
    inFile.close();
}

void IOHandler::loadCDsFromFile(string FilePath)
{
    /*
    int nCDs,nNodes,entry;
    vector<int> nodeParameters;
    Node tempNode;

    ifstream inFile(FilePath.c_str());

    inFile>>nCDs;
    for(int i = 0 ;i<nCDs;i++)  //read CDs
    {
        CD tempCD;
        inFile>>nNodes;
        for(int j=0;j<nNodes;j++) //reading nodes
        {
            for(int k=0;k<Constants::NUM_NODE_PARAMETERS;k++)
            {
                inFile>>entry;
                nodeParameters.push_back(entry);
            }
            tempNode.setNodeParameters(nodeParameters);
            nodeParameters.clear();
            if(j<3)    tempCD.replaceNode(j,tempNode);
            else       tempCD.addNode(tempNode);
        }
        memory->addCD(tempCD);
    }
    inFile.close();
    */
}

void IOHandler::loadCDsVsActionsFromFile(string FilePath)
{
    /*
    int nCDs,nactions,entry;
    vector<int> row;

    ifstream inFile(FilePath.c_str());
    inFile>>nCDs;
    inFile>>nactions;

    for(int i=0;i<nCDs;i++)
    {
        for(int j=0;j<nactions;j++)
        {
            inFile>>entry;
            row.push_back(entry);
        }
        memory->addRowInCDsVsActionsTable(row);
        row.clear();
    }

    inFile.close();*/
}

void IOHandler::loadKeysFromFile(string FilePath)
{
    int keyLength,i,mappedValue;
    string key_value;
    ifstream inFile(FilePath.c_str());
    inFile>>keyLength;
    for(i=0;i<keyLength;i++)
    {
        inFile>>key_value;
        inFile>>mappedValue;
        memory->addKey(key_value);
    }

    inFile.close();
}

void IOHandler::loadDatasetsAndQuestionsFromFile(string FilePath)
{
    int nDatasets,datasetSize,questionsSize;
    string action;
    string question;
    char ch;
    string str;
    int index;
    vector<string> dataset;
    vector<string> questions;
    vector <string> answers;

    ifstream inFile(FilePath.c_str());
    while(1)
    {
        inFile>>ch;
        inFile>>index;
        inFile>>ch;
        //read dataset
        inFile>>datasetSize;

        getline(inFile, action); //to avoid "\n"
        for(int j=0;j<datasetSize;j++){
            getline(inFile, action);
            dataset.push_back(action);
        }

        //read questions and answers
        inFile>>questionsSize;
        if(!getline(inFile, action)) break; //to avoid "\n"

        for(int j=0;j<questionsSize;j++){
            question = "";
            getline(inFile, action);
            istringstream itemStream(action);
            string item;
            while(!itemStream.eof())
            {
                itemStream>>item;
                if(question == "") question  = item;
                else               question  = question +" "+item;
            }
            for(int k=(question.size()-1);;k--)
            {
                if(question[k] == ' ')     {question.erase(k,1); break;}
                question.erase(k,1);
            }
            answers.push_back(item);
            questions.push_back(question);
        }
        //save

        memory->addDatasetQuestionsAnswers(dataset,questions,answers);
        dataset.clear();
        questions.clear();
        answers.clear();
    }
    inFile.close();


}

//------------------------------
void IOHandler::saveActionsToFile(string FilePath)
{
    int nActions = memory->numOfActions();
    vector<int> action;

    ofstream outFile(FilePath.c_str());

    for(int i=0;i<nActions;i++)
    {
        action = memory->getAction(i);
        for(int j=0;j<action.size();j++)  //loop on items
        {
            if(j!=0) outFile<<" ";
            outFile<<memory->getKey(action[j]);
        }
        outFile<<"\n";
    }
    outFile.close();
}

void IOHandler::saveSentencesToFile(string FilePath)
{
    int nsentences = memory->numOfSentences();
    vector<int> sentence;

    ofstream outFile(FilePath.c_str());
    for(int i=0;i<nsentences;i++)
    {
        sentence = memory->getSentence(i);
        for(int j=0;j<sentence.size();j++)  //loop on items
        {
            if(j!=0) outFile<<" ";
            outFile<<memory->getKey(sentence[j]);
        }
        outFile<<"\n";
    }
    outFile.close();
}

void IOHandler::saveCDsToFile(string FilePath)
{
    int nCDs = memory->numOfCDs();
    int nNodes,entry;
    vector<int> nodeParameters;
    Node tempNode;

    ofstream outFile(FilePath.c_str());
    outFile<<nCDs<<"\n";
    for(int i = 0 ;i<nCDs;i++)  // CDs
    {
        nNodes = memory->getCD(i).getNumOfNodes();
        outFile<<nNodes<<"\n";
        for(int j=0;j<nNodes;j++) // nodes
        {
            nodeParameters = memory->getCD(i).getNode(j).getNodeParameters();
            for(int k=0;k<Constants::NUM_NODE_PARAMETERS;k++)
            {
                if(k!=0) outFile<<" ";
                outFile<<nodeParameters[k];
            }
            outFile<<"\n";
        }
    }
    outFile.close();
}

void IOHandler::saveCDsVsActionsToFile(string FilePath)
{
    ofstream outFile(FilePath.c_str());
    int nActions = memory->numOfActions();
    int nCDs = memory->numOfCDs();

    outFile<<nCDs;
    outFile<<" ";
    outFile<<nActions;
    outFile<<"\n";
    for(int i=0;i<nCDs;i++)
    {
        for(int j=0;j<nActions;j++)
        {
            if(j!=0) outFile<<" ";
            outFile<<memory->getCDAfromTable(i,j);
        }
        outFile<<"\n";
    }
    outFile.close();
}

void IOHandler::addLineToAnimation(vector<int> data, string FilePath)
{
    ofstream outFile(FilePath.c_str(),std::ofstream::out | std::ofstream::app);

    for(int i=0;i<data.size();i++)
    {
        if(i) outFile<<' ';
        outFile<<data[i];
    }
    outFile<<"\n";
    outFile.close();
}

void IOHandler::addToAnimation(string data, string FilePath)
{
    ofstream outFile(FilePath.c_str(),std::ofstream::out | std::ofstream::app);
    outFile<<data;
    outFile.close();
}

void IOHandler::addToAnimation(int data, string FilePath)
{
    ofstream outFile(FilePath.c_str(),std::ofstream::out | std::ofstream::app);
    outFile<<data;
    outFile.close();
}

void IOHandler::removeAnimationFile(string FilePath)
{
    ofstream outFile(FilePath.c_str(),std::ofstream::out | std::ofstream::trunc);
    outFile.close();
}
void IOHandler::saveKeysToFile(string FilePath)
{
    string key_value;
    int keyLength = memory->numOfKeys();
    ofstream outFile(FilePath.c_str());
    outFile<<keyLength;
    outFile<<"\n";
    for(int i=0;i<keyLength;i++)
    {
        outFile<<memory->getKey(i);
        outFile<<" ";
        outFile<<i;
        outFile<<"\n";
    }
    outFile.close();
}

void IOHandler::removeFiles()
{
    fActionsLoaded = false;
    remove( "actions.dat" );
    remove( "keys.dat" );
    remove( "CDs.dat" );
    remove( "CDVsActionsTable.dat" );
    remove( "sentences.dat" );
}
