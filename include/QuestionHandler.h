#ifndef QUESTIONHANDLER_H
#define QUESTIONHANDLER_H

#include "Tester.h"
#include "CD.h"

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <utility>
#include <algorithm>

using namespace std;

class QuestionHandler
{
    public:
        QuestionHandler();
        virtual ~QuestionHandler();

        int solveQuestion(vector<CD> &MGICDs,vector<int> &LTM,vector<int> &LAS,vector<int> &LAE);   //solves the question based on all CDs including hybrid
        void readQuestion();
        void setQuestion(vector<int> action);

        vector<int>  getQuestion();
        vector<int>  getPossibleAnswers();

    protected:
    private:
    vector<int> question;
    vector< int > possibleAnswers;
};

#endif // QUESTIONHANDLER_H
