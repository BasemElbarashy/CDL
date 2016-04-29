# Common-Description Learning (CDL)
This is a new framework to learn simples patterns from a few number of examples and use them to learn harder ones. the learned models are perfectly interpretable and its depth depends on the question. What is meant by depth here is that
whenever needed, the model learns to break down the problem into simpler subproblems
and solves them using previously learned models
The paper can be found at: arxiv..   
     
CDL is written in C++ and tested on 32 small datasets
Directories
-----------------------------------
`src/`       - source files

`dataset/`   - dataset divided into three groups (Group_A: 25 small datasets, Group_B: 5 small datasets,Group_C: 2 small datasets)   

`experiments/`   
* 3 outputFiles: contain MGICDs learned in each group of datasets and some statistics about training
* 32 animation files that explain how MGICD solve the test questions, they can be shown by calling `animate.py` but need first to to change the directory of the animation file in the first line. you can also change the animation speed by changing the value of nodeTime
* Most of CDs in the outputFiles can be displayed by `visualizeCD.py`, but it need first to be copied in CD.dat



Installation
---------------------
```
make
./runner
```
Usage
---------------------
You can edit `Driver.cpp` to learn datasets in Group_A, Group_B or Group_C. For example, to learn the first dataset in  Group_A write this in main():
```cpp
    IOH.loadDatasetsAndQuestionsFromFile("dataset/Group_A.dat");
    Constants::redirectStdoutToOutputFile = true;  //redirect stdout to OutputFile.txt
                                                   //The results will be saved in OutputFile.txt
    setHyperParameters(1,2,1,2,7,0,0,1);    //set hyperparamters for the 25 dataset in file Group_A.dat
    TE.learnAndTest(0);                      //learn & test the first Dataset in file Group_A.dat
```
The results and learned models will be save in `OutputFile.txt`.
To learn all the datasets in Group_A: 
```cpp
    IOH.loadDatasetsAndQuestionsFromFile("dataset/Group_A.dat");
    Constants::redirectStdoutToOutputFile = true;  //redirect stdout to OutputFile.txt
                                                   //The results will be saved in OutputFile.txt
    setHyperParameters(1,2,1,2,7,0,0,1);    //set hyperparamters for the 25 dataset in file Group_A.dat
    TE.learnAndTest();                      //learn & test all Datasets in file Group_A.dat
```
You can also write a new dataset and test but it must be formatted like other datasets


Example : three one-digit numbers addition Group_B (dataset_2)
---------------------
![example1](GifFiles/Group_B_dataset_2.gif)

<br><br><br>

Example: reverse task Group_A (dataset_19)
---------------------
![example2](GifFiles/Group_A_dataset_19.gif)

<br><br><br>

Example: Group_B (dataset_3)
---------------------
![example2](GifFiles/Group_B_dataset_3.gif)

<br><br><br>

Example: Group_A (dataset_24)
---------------------
![example2](GifFiles/Group_A_dataset_24.gif)

License
---------------------
This project is licensed under the terms of the MIT license
