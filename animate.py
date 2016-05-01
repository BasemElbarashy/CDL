#=====================================================================
# select the animation file u want to run
stepsFile = open ( 'experiments/Group_A_results/animationFiles/animation_dataset_20.dat' , 'r'); #

#=====================================================================
from tkinter import *
root = Tk()
root.title("Animation of how CD solves the test problems")
cw = root.winfo_screenwidth()
ch = root.winfo_screenheight()

C = Canvas(root, width=cw, height=ch, background="white")
C.grid(row=0, column=0)
predefinedColor = '#999444666';
nodeTime = 200; #1000
nProblems = 1;
#=====================================================================
ASSIG_BEFORE = -1
ASSIG_AFTER = 1
COND_TIME = 0
COND_VALUE = 2
SINK = 3
SOURCE = 4
Z_NODE = 5
PUSH = 6
SOLVE = 7
problemPositionX = 150
problemPositionY = 630

NodeType     = C.create_text(75 ,problemPositionY+25, text = '', fill='black',font=('verdana', 20));
QuestionText = C.create_text(200,problemPositionY-20, text = "", fill='brown',font=('verdana', 14));
#=====================================================================
predefined      = [None]*100;
actions         = [None]*100;
question        = [None]*100;
fPositivePart   = [None]*100;

for i in range(0,99):
    question[i]  = [None]*100;
    
actionIds       = [None]*100;
questionIds     = [None]*100;
for i in range(0,99):
    questionIds[i]  = [None]*100;

currentQuestion =  0;
predefinedIds   = [None]*100;
#=====================================================================
def waitFor(timeInMs):
    C.update()
    C.after(round(timeInMs))
#=====================================================================
def push(GSP,no):
    C.itemconfigure(NodeType,text = 'PUSH');
    s = problemPositionX        #x aixs
    e = problemPositionY - no*70   #y axis
    length = 40;
    width  = 30;
    sStep = length/10 + length;
    eStep = width/2   + width;
    fontSize = 7;

    GSPText = C.create_text(310,e-20, text = "Generated subproblem (Level "+str(no)+"):", fill='brown',font=('verdana', 14));
    for j in questionIds[no]:
       C.delete(j);
    
    ids  = [None] * (len(GSP)+2);
    text = [None] * (len(GSP)+2);
    i=0;                ids[i] = C.create_rectangle(s+sStep*i, e ,s+sStep*i+length ,e+width ,width = 4.0,fill=predefinedColor);
    i=len(GSP) + 1;     ids[i] = C.create_rectangle(s+sStep*i, e ,s+sStep*i+length ,e+width ,width = 4.0,fill=predefinedColor);
    
    i = 0;              text[i] = C.create_text(s+sStep*i+length/2, e + width/2, text = '', fill='black',font=('verdana', fontSize));
    i = len(GSP)+1;     text[i] = C.create_text(s+sStep*i+length/2, e + width/2, text = '', fill='black',font=('verdana', fontSize));
    for i in range(1,len(GSP)+1):
        ids[i] = C.create_rectangle(s+sStep*i, e ,s+sStep*i+length ,e +width,width = 2.0,fill='#FFFBBBFFF');
        text[i] = C.create_text(s+sStep*i+length/2, e + width/2, text = GSP[i-1], fill='black',font=('verdana', fontSize));

    waitFor(nodeTime)
    
    return ids+text+[GSPText]

    
#=====================================================================
def solve(moveTo):
    C.itemconfigure(NodeType,text = 'Solve');
    waitFor(nodeTime);
#=====================================================================    
def defineZVar(value):
    C.itemconfigure(NodeType,text = 'Output');
    lastItemRectId = questionIds[currentQuestion][len(question[currentQuestion])+1];
    lastItemTextId = questionIds[currentQuestion][(len(question[currentQuestion])+2)*2-1];
    
    C.itemconfigure(lastItemRectId, fill='white',width = 10)
    a = C.coords(lastItemRectId);
    C.coords(lastItemRectId,a[0],a[1],2*a[2]-a[0],a[3]) # change coordinates
    a = C.coords(lastItemRectId);
    
    b = C.coords(lastItemTextId);
    C.coords(lastItemTextId,(a[2]+a[0])/2,b[1]) # change coordinates    
    C.itemconfigure(lastItemTextId, text=value,fill='red',font = "10")
    
    waitFor(nodeTime*2);
    if(currentQuestion > 0):
        for j in questionIds[currentQuestion]:
            C.delete(j);
    

#=====================================================================    
def condTimeWithVar(action1,item1,action2,item2,state):
    C.itemconfigure(NodeType,text = 'Compare\nPositions');
    if action1<0:
        id1 = questionIds[currentQuestion][item1];
    else:
        id1 = actionIds[action1][item1];

    if action2<0:
        id2 = questionIds[currentQuestion][item2];
    else:
        id2 = actionIds[action2][item2];


    a = C.coords(id1);
    b = C.coords(id2);
    c1 = C.itemcget(id1, 'fill')
    c2 = C.itemcget(id2, 'fill')
    for i in range(1,3):
        waitFor(nodeTime/4)  
        id = C.create_line(a[0], a[1],b[0], b[1],width = 4.0)
        if (state == 1):
            C.itemconfigure(id1, fill='#00ff00')
            C.itemconfigure(id2, fill='#00ff00')
        else:
            C.itemconfigure(id1, fill='#ff0000')
            C.itemconfigure(id2, fill='#ff0000')    
        waitFor(nodeTime/2)
        C.delete(id)
        C.itemconfigure(id1, fill=c1)
        C.itemconfigure(id2, fill=c2)       
#=====================================================================    
def condValueWithVar(action1,item1,action2,item2,state):
    C.itemconfigure(NodeType,text = 'Compare\nValues');
    if action1<0:
        id1 = questionIds[currentQuestion][item1];
    else:
        id1 = actionIds[action1][item1];

    if action2<0:
        id2 = questionIds[currentQuestion][item2];
    else:
        id2 = actionIds[action2][item2];


    a = C.coords(id1);
    b = C.coords(id2);
    c1 = C.itemcget(id1, 'fill')
    c2 = C.itemcget(id2, 'fill')
    for i in range(1,3):
        waitFor(nodeTime/4)  
        id = C.create_line(a[0], a[1],b[0], b[1],width = 4.0)
        if (state == 1):
            C.itemconfigure(id1, fill='#00ff00')
            C.itemconfigure(id2, fill='#00ff00')
        else:
            C.itemconfigure(id1, fill='#ff0000')
            C.itemconfigure(id2, fill='#ff0000')    
        waitFor(nodeTime/2)
        C.delete(id)
        C.itemconfigure(id1, fill=c1)
        C.itemconfigure(id2, fill=c2)       
#=====================================================================    
def condValueWithConst(action1,item1,value,state):
    C.itemconfigure(NodeType,text = 'Compare\nValues');
    if action1<0:
        id1 = questionIds[currentQuestion][item1];
    else:
        id1 = actionIds[action1][item1];

    idx = predefined.index(value);
    id2 = predefinedIds[idx];

    a = C.coords(id1);
    b = C.coords(id2);
    c1 = C.itemcget(id1, 'fill')
    c2 = C.itemcget(id2, 'fill')
    for i in range(1,3):
        waitFor(nodeTime/4)  
        id = C.create_line(a[0], a[1],b[0], b[1],width = 4.0)
        if (state == 1):
            C.itemconfigure(id1, fill='#00ff00')
            C.itemconfigure(id2, fill='#00ff00')
        else:
            C.itemconfigure(id1, fill='#ff0000')
            C.itemconfigure(id2, fill='#ff0000')    
        waitFor(nodeTime/2)
        C.delete(id)
        C.itemconfigure(id1, fill=c1)
        C.itemconfigure(id2, fill=c2)       
#=====================================================================
def assign(action,item1,item2,state):
    C.itemconfigure(NodeType,text = 'Assign');    
    if(action<0):        
        a = C.coords(questionIds[currentQuestion][item1]);
        b = C.coords(questionIds[currentQuestion][item2]);
        prevColor1 = C.itemcget(questionIds[currentQuestion][item1], 'fill')
        prevColor2 = C.itemcget(questionIds[currentQuestion][item2], 'fill')
        
        id = C.create_line((a[0]+a[2])/2, (a[1]+a[3])/2,(b[0]+b[2])/2, (b[1]+b[3])/2,width = 5.0,fill='#008800',arrow='last')
        C.itemconfigure(questionIds[currentQuestion][item2], fill='yellow')
        waitFor(nodeTime)
        C.delete(id)
        
        if (state == 1):
            C.itemconfigure(questionIds[currentQuestion][item1], fill=prevColor2)
            
    else:
        a = C.coords(actionIds[action][item1]);
        b = C.coords(actionIds[action][item2]);
        prevColor1 = C.itemcget(actionIds[action][item1], 'fill')
        prevColor2 = C.itemcget(actionIds[action][item2], 'fill')

        C.itemconfigure(actionIds[action][0], fill=predefinedColor)
        C.itemconfigure(actionIds[action][len(actions[action])+1], fill=predefinedColor)
        
        id = C.create_line((a[0]+a[2])/2, (a[1]+a[3])/2,(b[0]+b[2])/2, (b[1]+b[3])/2,width = 5.0,fill='#008800',arrow='last')
        C.itemconfigure(actionIds[action][item2], fill='yellow')
        waitFor(nodeTime)
        C.delete(id)
        
        if (state == 1):
            C.itemconfigure(actionIds[action][item1], fill=prevColor2)
    
#=====================================================================
def predefinedItems(action):
    s = round(cw/1.1);
    e = round(ch/10);
    length = 50;
    width  = 30;
    sStep = length/10 + length;
    eStep = width/4 + width;
    C.create_text(s-30,e-width/2, text = "Colors", fill='brown',font=('verdana', 14));

    for j in predefinedIds:
            C.delete(j);
    #---------------
    i=0;                 C.create_rectangle(s+sStep*i, e ,s+sStep*i+length ,e+width ,width = 4.0,fill=predefinedColor);    
    i=1;                 C.create_rectangle(s, e+eStep*i ,s+length ,e+width+eStep*i ,width = 4.0,fill='#ffff00');
    C.create_text(s-60,e+width/2, text = "Predefined", fill='black',font=('verdana', 10));   
    C.create_text(s-60,e+width*3/2+width/4, text = "CD Variables", fill='black',font=('verdana', 10));   
    e = e+150;
    C.create_text(s-50,e+50, text = "Predefined Values", fill='brown',font=('verdana', 14));   
    #---------------
    ids = [None] * 100;
    text = [None] * 100;
    for i in range(2,len(action)+2):
        ids[i-2] = C.create_rectangle(s, e+eStep*i ,s+length ,e+width+eStep*i,width = 2.0,fill=predefinedColor);
        if len(action[i-2])< 6 :
            text[i] = C.create_text(s+length/2, e+eStep*i + width/2, text = action[i-2], fill='black',font=('verdana', 8));
        else:
            text[i] = C.create_text(s+length/2, e+eStep*i + width/2, text = action[i-2], fill='black',font=('verdana', 7));

    return ids + text
#=====================================================================
def drawQuestion(action,no):
    s = problemPositionX; #x aixs
    e = problemPositionY; #y axis
    length = round(cw/30);
    width  = round(cw/40);
    sStep = length/10 + length;
    eStep = width/4 + width;
    fontSize = 8;
    if(len(action)>19):
        length = 30;
        width  = 30;
        sStep = length/10 + length;
        eStep = width/6 + width;
        fontSize = 7;
        
    
    C.itemconfigure(QuestionText, text="Question " + no);
    
    for j in questionIds[currentQuestion]:
        C.delete(j);    
    
    ids = [None] * (len(action)+2);
    text = [None] * (len(action)+2);
    i=0;                ids[i] = C.create_rectangle(s+sStep*i, e ,s+sStep*i+length ,e+width ,width = 4.0,fill=predefinedColor);
    i=len(action) + 1;  ids[i] = C.create_rectangle(s+sStep*i, e ,s+sStep*i+length ,e+width ,width = 4.0,fill=predefinedColor);
    
    i = 0;              text[i] = C.create_text(s+sStep*i+length/2, e + width/2, text = '', fill='black',font=('verdana', fontSize));
    i = len(action)+1;  text[i] = C.create_text(s+sStep*i+length/2, e + width/2, text = '', fill='black',font=('verdana', fontSize));
    for i in range(1,len(action)+1):
        ids[i] = C.create_rectangle(s+sStep*i, e ,s+sStep*i+length ,e +width,width = 2.0,fill='#FFFBBBFFF');
        text[i] = C.create_text(s+sStep*i+length/2, e + width/2, text = action[i-1], fill='black',font=('verdana', fontSize));
    
    return ids+text
#=====================================================================
def drawAction(actionNo,action):
    s = 10; #x aixs
    e = 30; #y axis
    length = 50;
    width = 20;
    sStep = length/10 + length;
    eStep = 2*width/nProblems + width;
    if (actionNo == 0):
        text = C.create_text(s+100,e-20, text = "Training Sequences:", fill='brown',font=('verdana', 14));
    
    ids  = [None] * (len(action)+2);
    text = [None] * (len(action)+2);
    i=0;                ids[i] = C.create_rectangle(s+sStep*i, e + actionNo * eStep,s+sStep*i+length ,e + actionNo * eStep+width,width = 4.0,fill='#555555fff');
    i=len(action) + 1;  ids[i] = C.create_rectangle(s+sStep*i, e + actionNo * eStep,s+sStep*i+length ,e + actionNo * eStep+width,width = 4.0,fill='#555555fff');
    
    for i in range(1,len(action)+1):
        ids[i] = C.create_rectangle(s+sStep*i, e + actionNo * eStep,s+sStep*i+length ,e + actionNo * eStep+width,width = 2.0,fill='#BBBBBBfff');
        if len(action[i-1])< 6 :
            text[i] = C.create_text(s+sStep*i+length/2, e + actionNo * eStep +width/2, text = action[i-1], fill='black',font=('verdana', 8));
        else:
            text[i] = C.create_text(s+sStep*i+length/2, e + actionNo * eStep +width/2, text = action[i-1], fill='black',font=('verdana', 7));
    
    return ids + text
#=====================================================================
#--------------------------------------- read animation file
lines = [ line.split() for line in stepsFile];
#--------------------------------------- draw training data
nxtLine = 0;
nProblems = int(lines[0][0]);

nxtLine = int(lines[0][0])+1;

fPositivePart[0] = 1;
CDPart   = C.create_text(round(cw/1.3),problemPositionY-180, text = 'CDPart',   fill='black',font=('verdana', 20));
CDStatus = C.create_text(round(cw/1.3),problemPositionY-150, text = '', fill='black',font=('verdana', 20));
#--------------------------------------- loop on steps CD took to solve the training sequences
while (nxtLine<len(lines)): 
    
    if lines[nxtLine][0] == 'Question':
        for i in range(0,int(lines[0][0])):
            actionIds[i] = drawAction(i,lines[i+1]);
            actions[i] = lines[i+1];
        
        nxtLine = nxtLine+1;
        questionIds[0] = drawQuestion(lines[nxtLine],lines[nxtLine-1][1]);
        question[0] = lines[nxtLine];
        nxtLine = nxtLine+1;
        waitFor(6*nodeTime)
        prefinedValues = lines[nxtLine];
        predefinedIds = predefinedItems(prefinedValues);
        
        predefined = lines[nxtLine];
        
        for i in range(0,int(lines[0][0])):
            actionIds[i] = drawAction(i,lines[i+1]);
        
        waitFor(1000)
    elif lines[nxtLine][0] == 'NegativePart':
        fPositivePart[currentQuestion] = 0;
        C.itemconfigure(CDPart,text = 'Negative Part');
    elif lines[nxtLine][0] == 'PositivePart':
        fPositivePart[currentQuestion] = 1;
        C.itemconfigure(CDPart,text = 'Positive Part');

    elif lines[nxtLine][0] == 'Succeeded':
        if(fPositivePart[currentQuestion]):
            C.itemconfigure(CDStatus,text = 'Succeeded');
        else:
            C.itemconfigure(CDStatus,text = 'Failed');
        waitFor(1000);
        C.itemconfigure(CDStatus,text = '');
    elif lines[nxtLine][0] == 'Failed':
        if(fPositivePart[currentQuestion]):
            C.itemconfigure(CDStatus,text = 'Succeeded');
        else:
            C.itemconfigure(CDStatus,text = 'Failed');
        waitFor(1000);
        C.itemconfigure(CDStatus,text = '');
    else:
        if(int(lines[nxtLine][0]) == ASSIG_BEFORE) or (int(lines[nxtLine][0]) == ASSIG_AFTER):
            assign(int(lines[nxtLine][1]),int(lines[nxtLine][4]),int(lines[nxtLine][2]),int(lines[nxtLine][5]))
        if(int(lines[nxtLine][0]) == Z_NODE) and (fPositivePart[currentQuestion] == 1):
            defineZVar(lines[nxtLine][1]);
        if(int(lines[nxtLine][0]) == COND_VALUE) and (len(lines[nxtLine])==5):
            condValueWithConst(int(lines[nxtLine][1]),int(lines[nxtLine][2]),lines[nxtLine][3],int(lines[nxtLine][4]));
        if(int(lines[nxtLine][0]) == COND_VALUE) and (len(lines[nxtLine])==6):
            condValueWithVar(int(lines[nxtLine][1]),int(lines[nxtLine][2]),int(lines[nxtLine][3]),int(lines[nxtLine][4]),int(lines[nxtLine][5]));
        if(int(lines[nxtLine][0]) == COND_TIME):
            condTimeWithVar(int(lines[nxtLine][1]),int(lines[nxtLine][2]),int(lines[nxtLine][3]),int(lines[nxtLine][4]),int(lines[nxtLine][5]));
        
        if(int(lines[nxtLine][0]) == PUSH):
            questionIds[currentQuestion+1] = push(lines[nxtLine][1:],currentQuestion+1)
            question[currentQuestion+1]    = lines[nxtLine][1:];
        if(int(lines[nxtLine][0]) == SOLVE):
            for i in range(0,int(lines[0][0])):
                actionIds[i] = drawAction(i,lines[i+1]);
                actions[i] = lines[i+1];

            isGoToGSP = (int(lines[nxtLine][1])>currentQuestion)
            currentQuestion = int(lines[nxtLine][1]);
            solve(int(lines[nxtLine][1]))
            if(isGoToGSP):
                nxtLine = nxtLine+1;                
                for i in range(0,len(lines[nxtLine])):
                    prefinedValues.append(lines[nxtLine][i]);
                predefinedIds = predefinedItems(prefinedValues);
            else:
                if(fPositivePart[currentQuestion]):
                    C.itemconfigure(CDPart,text = 'Positive Part');
                else:
                    C.itemconfigure(CDPart,text = 'Negative Part');
                        
    nxtLine = nxtLine +1;

#=====================================================================
root.mainloop()
#=====================================================================
