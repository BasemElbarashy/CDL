#include "Tester.h"
#include "IOHandler.h"
#include <ctime>
#include <string.h>

vector<int> nNodesInMGICDs;
//print variable
#define PVV(x)  cout<<#x<<" = "<<x<<"\n";
//print vector
#define PMV(x)  cout<<"\n===========\n"; {for(int PMVC=0;PMVC<x.size();PMVC++) PVV(x[PMVC]);} cout<<"===========\n";
static bool isOutputFileOpened = false;
Tester::Tester()
{
        //ctor
}

Tester::~Tester()
{
    //dtor
}

int Tester::learnAndTest(int datasetIndex)
{
    Generator generator;
    QuestionHandler QH;
    IOHandler IOH;
    vector<CD> MGICDs;
    vector<int> LTM;
    vector<int> LAS;
    vector<int> LAE;
    vector< vector<int> > CDVSACTIONS;
    vector< vector<int> > permutaions;
    bitset<Constants::MAX_NUM_PCDS> PCDsUsed;
    int predictedAnswer;
    Memo *memory = Memo::getInstance();

    if(Constants::redirectStdoutToOutputFile && !isOutputFileOpened)
    {
        freopen ("OutputFile.txt","w",stdout);
        isOutputFileOpened = true;
    }

    cout<<"\n===============================================================\n";
    cout<<"[ Dataset no ( "<<datasetIndex<<" ) ]\n\n";

    string str1 =  "animationFiles/animation_dataset_" ;
    string str2 =  static_cast<ostringstream*>( &(ostringstream() << datasetIndex) )->str();
    string str3 =  ".dat" ;

    Constants::animationFilePath = str1+str2+str3;

    int nRightAnswers=0,nQuestions,accuracy,nActions,nCDs;
    vector<int> rightAnswers;
    vector<bitset<Constants::MAX_NUM_ACTIONS> > VCDA;
    vector<vector<int> > VCDs;
    int i,j,k;
    cout<<"Number of training sequences = "<<memory->getDataset(datasetIndex).size()<<"\n";
    
    generator.learnActions(memory->getDataset(datasetIndex));
    nNodesInMGICDs.push_back(generator.getnNodesInMGICDs());
    VCDA = generator.getVCDA();
    VCDs = generator.getVCDs();
    IOH.removeAnimationFile();
    MGICDs = generator.getMGICDs();
    LTM    = generator.getLTM();
    LAS    = generator.getLAS();
    LAE    = generator.getLAE();

    //------------------------------------------------- print statistics
    /*
    if(generator.getnPCDGeneratorCalls()<1e6)
        cout<<"\nNumber of PCDs Generator Calls = "<<generator.getnPCDGeneratorCalls()<<"   took "<<generator.getPCDsGenerationTime()<<"  seconds\n";
    else
        cout<<"\nNumber of PCDs Generator Calls = "<<float(generator.getnPCDGeneratorCalls())/1e6<<" million   took "<<generator.getPCDsGenerationTime()<<"  seconds\n";
    */
    printf("\nGeneration of PCDs   took %.2f minutes",  generator.getPCDsGenerationTime()/60);
    printf("\nGeneration of VCDs   took %.2f minutes",  generator.getVCDsGenerationTime()/60);
    printf("\nGeneration of MGICDs took %.2f minutes\n\n",generator.getMGICDsGenerationTime()/60);

    cout<<"Number of PCDs = "<<generator.getPCDs().size()<<" ( "<<generator.getnVPCDs()<<" valid"<<" )\n";
    cout<<"Number of VCDs = "<<generator.getVCDs().size()<<" ( ";
    for(i=1;i<=(Constants::HyperParameters[0][0]+1);i++)
    {
        for(j=0,k=0;j<VCDs.size();j++)
        {
            if(VCDs[j].size() == i) k++;
        }
        if(i==1)    cout<<k<<" VPCD";
        else        cout<<", "<<k<<" VHCD_"<<(i-1)<<"N";   //VHCD_2N ==> PCD+two negative CDs
    }
    cout<<" )\n";
    cout<<"Gamma = "<<generator.getgamma()<<'\n';
    //cout<<"Number of MGICD Generator Calls = "<<generator.getnMGICDGeneratorCalls()<<"   took "<<generator.getMGICDsGenerationTime()<<"  seconds\n";
    //------------------------------------------------- animation
    IOH.addToAnimation(LAS.size());
    IOH.addToAnimation("\n");
    for(int i=0;i<LTM.size();i++)
    {
        if(LTM[i] == Constants::LTM_SEPARATOR)
        {
            if(i)
                IOH.addToAnimation("\n");
        }
        else
        {
            IOH.addToAnimation(memory->getKey(LTM[i]));
            IOH.addToAnimation(" ");
        }
    }
    //--------------------------------------------------
    //cout<<"number of VCDs = "<<generator.getNumOfVCDs()<<"\n";
    cout<<"\n";
    nQuestions = memory->getQuestions(datasetIndex).size();
    nActions = memory->getDataset(datasetIndex).size();
    cout<<"Number of VCDs in MGICDs  = "<<MGICDs.size()<<"\n";
    cout<<"Number of Nodes in MGICDs = "<<generator.getnNodesInMGICDs()<<"\n\n";
    /*cout<<"VCDA     ==> compnents\n";
    for(int i=0;i<generator.getNumOfVCDs();i++)
    {
        cout<<"["<<i<<"] ";
        for(int j=0;j<nActions;j++) cout<<VCDA[i][j];
        cout<<"  ==> ";
        for(int j=0;j<VCDs[i].size();j++)  cout<<VCDs[i][j]<<" ";
        cout<<"\n";
    }//*/
    //printing MGICDs content

    cout<<"=======================================\nVCDs of MGICDs : \n\n";
    for(int i=0;i<MGICDs.size();i++)
    {
        cout<<"---------  [["<<i<<"]]";                                //<<"  ==> sourceAction = "<<MGICDs[i].getSourceAction()<<"\n";
        if(MGICDs[i].getType() == Constants::POSITIVE_CD)
            cout<<"  Poistive CD";
        else
            cout<<"  Hybrid CD";
        cout<<"  ---------\n";
        MGICDs[i].printCD();
    }
    cout<<"=======================================\n";
    //*/

    for(int i=0;i<nQuestions;i++)
    {
        //----------------------------------------------------------------- animation
        IOH.addToAnimation("Question ");
        IOH.addToAnimation(i);
        IOH.addToAnimation("\n");
        vector<int> question =  memory->getQuestions(datasetIndex)[i];
        for(int j=0;j<question.size();j++)
        {
            if(j) IOH.addToAnimation(" ");
            IOH.addToAnimation(memory->getKey(question[j]));
        }
        IOH.addToAnimation("\n");
        //-----------------------------------------------------------------
        QH.setQuestion(memory->getQuestions(datasetIndex)[i]);
        predictedAnswer = QH.solveQuestion(MGICDs,LTM,LAS,LAE);

        if(predictedAnswer!=Constants::UNDEFINED_VAR )
            if(predictedAnswer == memory->getAnswers(datasetIndex)[i])
                nRightAnswers++;

        //--------------------------- print questions and answers
        cout<<"Question "<<i<<" : predicted answer = ";
        if(predictedAnswer!=Constants::UNDEFINED_VAR )
            cout<<memory->getKey(predictedAnswer);
        else cout<<"Undefined";

        rightAnswers = QH.getPossibleAnswers();
        if(rightAnswers.size() == 0)   cout<<"   [ "<<"No Answer"<<" ]";
        //if(rightAnswers.size() == 1)   cout<<"   [ "<<"one Answer"<<" ]\n";
        if(rightAnswers.size() >  1)
        {
               cout<<"   [ "<<"multiple Answers:";
               for(int j=0;j<rightAnswers.size();j++) cout<<memory->getKey(rightAnswers[j])<<" ,";
               cout<<"]";
        }
        //-----------------------------
        cout<<"\n";
        //*/
    }
    accuracy = 100 *nRightAnswers/nQuestions;
    cout<<"\naccuracy = "<<accuracy<<"\n\n";

    return accuracy;
}


int Tester::learnAndTest()
{
    int overallAccuracy = 0;
    Memo *memory = Memo::getInstance();
    int datasetAccuracy[memory->numOfDatasets()];
    nNodesInMGICDs.clear();
    clock_t datasetLearningTime[memory->numOfDatasets()];

    if(Constants::redirectStdoutToOutputFile && !isOutputFileOpened)
    {
        freopen ("OutputFile.txt","w",stdout);
        isOutputFileOpened = true;
    }

    for(int i=0;i<memory->numOfDatasets();i++)
    {
        datasetLearningTime[i] = clock();
        datasetAccuracy[i] = learnAndTest(i);
        overallAccuracy +=datasetAccuracy[i];
        datasetLearningTime[i] = clock() - datasetLearningTime[i];
    }
    overallAccuracy/=memory->numOfDatasets();

    cout<<"\n===============================================================\n";
    for(int i=0;i<memory->numOfDatasets();i++)
    {
        cout<<"Dataset no ("<<i<<") ";
        cout<<"accuracy = "<<datasetAccuracy[i]<<"   nNodes = "<<nNodesInMGICDs[i];
        printf("   took %.2f minutes\n",((((float)datasetLearningTime[i])/CLOCKS_PER_SEC)/60));
    }
    cout<<"\nOverall Accuracy = "<<overallAccuracy;
    cout<<"\n===============================================================\n";
    return overallAccuracy;
}
