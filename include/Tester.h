#ifndef TESTER_H
#define TESTER_H

#include <vector>
#include "CD.h"

#include <iostream>
#include "Utilities.h"
#include "QuestionHandler.h"
#include "Generator.h"
#include "Memo.h"


using namespace std;

class Tester
{
    public:
        Tester();
        int learnAndTest(int datasetIndex); //try one dataset
        int learnAndTest();                 //try all datasets
        virtual ~Tester();
    protected:
    private:
};

#endif // TESTER_H
