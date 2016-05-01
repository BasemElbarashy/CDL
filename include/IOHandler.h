#ifndef IOHANDLER_H
#define IOHANDLER_H

#include "CD.h"
#include <fstream>
#include "Memo.h"
#include <queue>
#include "Utilities.h"
#include <iostream>


class IOHandler
{
    public:
        IOHandler();

        void loadActionsFromFile(string FilePath = Constants::actionsFilePath);
        void loadSentencesFromFile(string FilePath = Constants::sentencesFilePath);
        void loadCDsFromFile(string FilePath = Constants::CDsFilePath);
        void loadCDsVsActionsFromFile(string FilePath = Constants::CDsVsActionsFilePath);
        void loadKeysFromFile(string FilePath = Constants::keysFilePath);
        void loadDatasetsAndQuestionsFromFile(string FilePath = Constants::datasetsAndQuestions);

        void saveActionsToFile(string FilePath = Constants::actionsFilePath);
        void saveSentencesToFile(string FilePath = Constants::sentencesFilePath);
        void saveCDsToFile(string FilePath = Constants::CDsFilePath);
        void saveCDsVsActionsToFile(string FilePath = Constants::CDsVsActionsFilePath);
        void saveKeysToFile(string FilePath = Constants::keysFilePath);

        void addLineToAnimation(vector<int> data, string FilePath = Constants::animationFilePath);
        void addToAnimation(string data, string FilePath = Constants::animationFilePath);
        void addToAnimation(int    data, string FilePath = Constants::animationFilePath);
        void removeAnimationFile(string FilePath = Constants::animationFilePath);

        void removeFiles();

        virtual ~IOHandler();
    protected:
    private:
    static bool fActionsLoaded;
};

#endif // IOHANDLER_H
