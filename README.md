# Common-Description Learning (CDL)
This is a new framework to learn simples patterns from a few number of examples and use them to learn harder ones. the learned models
are perfectly interpretable and its depth depends on the question. By 'depth' I mean that whenever needed the model learns to break down the problem into simpler subproblems and solve them first.
The paper can be found at: arxiv..   
     
CDL is a library implements algorithms explaind in the paper and tested the framework on 32 small datasets
Directories included in the library
-----------------------------------
`CDL/`   - source code  
`Dataset/`   - dataset divided into three groups (Group_A: 25 small datasets, Group_B: 5 small datasets,Group_C: 2 small datasets)  
`results/`   - outputFile.txt: contain MGICDs learned in eac dataset and some statistics about training
             - 32 animation files that explain how MGICDs solve the test questions, they can be shown by calling `animate.py` but you                       need first to to change the directory of the animation file in the first line
             - any single CD in the outputFiles can be displayed by `visualizeCD`, but need first to copy it to CD.txt file 

Example: three one-digit numbers addition Group_B (dataset_2)
---------------------
![example1](GifFiles/Group_b_dataset_2.gif)
