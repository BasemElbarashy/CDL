#=====================================================================
f = open ( 'CD.dat' , 'r');
#=====================================================================
from tkinter import *
import os

root = Tk()
root.title("visualize CD")

cw = root.winfo_screenwidth()
ch = root.winfo_screenheight()

C = Canvas(root, width=cw, height=ch, background="white")

C.grid(row=0, column=0)
NodeID          = [None]*100;
NodeCenter      = [None]*100;
NodeNext        = [None]*100;
shiftArrow = 0;
digitSubscript = [u'\u2080',u'\u2081',u'\u2082',u'\u2083',u'\u2084',u'\u2085',u'\u2086',u'\u2087',u'\u2088',u'\u2089']

NodeColor        = '#10b0a0'
normalArrowColor = '#0000CC'   
textColor        = '#000'

trueArrowColor   = '#009900'
falseArrowColor  = '#CC0000'

fillNodeColor1   = '#87f7f7'   
fillNodeColor2   = '#40e0d0'

radius = round(cw/23);
fontSize = int(radius/3);
#=====================================================================
def waitFor(timeInMs):
    C.update()
    C.after(timeInMs)

def transformVar(NodeContent):
    i = 0;
    
    while i<len(NodeContent)-2:
        if(NodeContent[i] == '(') and ((NodeContent[i+2] == ')')):
            left  = NodeContent[0:i];
            right = NodeContent[i+3:];            
            if(NodeContent[i+1] != 's') and (NodeContent[i+1] != 'e'):
                middle = 'X' + digitSubscript[int(NodeContent[i+1])-3];
                NodeContent = left+middle+right;
            else:
                if(NodeContent[i+1] == 's'):
                    middle= 'S';
                    NodeContent = left+middle+right;
                if(NodeContent[i+1] == 'e'):
                    middle= 'E';
                    NodeContent = left+middle+right;
        i = i+1;
        
    return NodeContent
#=====================================================================
def drawArrow(x0,y0,x1,y1,color): #centers of two Nodes
    s0=s1=s2=s3=0
    global shiftArrow
    if(y1>y0):
        midPointx = x1;
        midPointy = y0;
    if(y1<y0):
        if(x0>x1):
            midPointx = x1;
            midPointy = y0;
        else:
            midPointx = x0;
            midPointy = y1;        
    if(y1==y0):
        midPointx = (x0+x1)/2;        
        midPointy = y0;
    
        
    factor = [None]*2;
    factor[0] = -1;
    factor[1] = 1;
    
    s0 = radius * (midPointx > x0) - radius * (midPointx < x0)
    s1 = radius * (midPointy > y0) - radius * (midPointy < y0);

    s2 = radius * (midPointx > x1) - radius * (midPointx < x1);
    s3 = radius * (midPointy > y1) - radius * (midPointy < y1);

    if(color == trueArrowColor):
        s1        = s1 - 8;
        midPointy = midPointy - 8;
        s3        = s3 - 8;
    if y1<y0:
        midPointx = midPointx - shiftArrow;
        x1 = x1 - shiftArrow
        shiftArrow = shiftArrow + 8;
        

    id1 = C.create_line(x0+s0,y0+s1,midPointx,midPointy,width = radius/8,fill=color)
    id2 = C.create_line(midPointx,midPointy,x1+s2,y1+s3,width = radius/8,fill=color,arrow='last')
    
        
    
#=====================================================================
def drawNode(x0,y0,NodeType,NodeNo,NodeContent,nextTrue,nextFalse,fillColor,outlineColor):
    x0 = x0 - radius;
    y0 = y0 - radius;
    w = radius/12;
    NodefontSize = fontSize
    NodeContent = transformVar(NodeContent)
    
    if NodeType == 'Z_NODE': #or NodeType == 'SOURCE':
        fillColor = fillNodeColor2
        NodefontSize = int(NodefontSize * 1)
    if NodeType == 'SOURCE':
        fillColor = fillNodeColor2
        
    id  = C.create_oval(x0, y0, x0+2*radius, y0+2*radius, width = w,fill=fillColor,outline = outlineColor)
    txt = C.create_text(x0+radius,y0+radius , text = NodeContent, fill=textColor,font=('Times 20 italic bold',NodefontSize));
    return id+txt
#=====================================================================
lines = [ line.split() for line in f];
#---------------------------------------

#---------------------------------------
yOffset =radius;
nxtLine = 0;
pxt = [2.1*radius]*3; #used in shifting the CD
px = pxt[0];
py = 4*radius+yOffset;
step = 3*radius;
mode = 0; # 0 => main flow,1=>first cycle, 2 =>second cycle
#---------- SINK, Z, SOURCE
#NodeID[0] = drawNode(500,500,lines[0][1],0,'SINK',-1,-1,'#fff','#ccc')
NodeID[2] = drawNode(px,py,lines[2][1],2,'SOURCE',lines[2][4],lines[2][4],fillNodeColor1,NodeColor)
memory    = [None];
NodeCenter[0] = [500,500];
NodeCenter[2] = [px,py];
NodeNext[2]   = [3];

nodeIdx = 2;
for i in range(3,len(lines)):

    if(len(lines[i]) == 0):
        continue;
    if lines[i][0] == 'Memory:':
        memory = lines[i];
        continue
    
    if(lines[i][0] == 'Cycle['):
        if(lines[i][3] == 'start'):
            mode = mode+1;
        if(lines[i][3] == 'end'):
            mode = mode-1;
    if(lines[i][0] == 'Cycle['):
        continue;
    nodeType = lines[i][1];
    #-------------- update Node position
    # update x  # update y
    if mode == 1 and ((nodeType == 'ASSIGN_AFTER') or (nodeType == 'ASSIGN_BEFORE')):
        py = 8.8*radius+yOffset
        px = pxt[1] = pxt[1] -step;
    elif mode == 2 and ((nodeType == 'ASSIGN_AFTER') or (nodeType == 'ASSIGN_BEFORE')):
        py = 6.4*radius+yOffset
        px = pxt[2] = pxt[2] -step;
    else:
        py = 4*radius+yOffset;
        px = pxt[0] =pxt[1] =pxt[2] = pxt[0]+step;
        pxt[1] =pxt[2] = pxt[1] +1.5*step;
    '''
    if(i==15):
        py = 4*radius+yOffset;
        px = pxt[0] =pxt[1] =pxt[2] = pxt[0]+1*step;
        pxt[1] =pxt[2] = pxt[1] +1.5*step;
    '''
    #--------------
        
    for j in range (3,len(lines[i])):
        if(lines[i][j] == 'Next'):
            break;
    nodeIdx = nodeIdx+1;
    
    NodeCenter[nodeIdx] = [px,py]
    
    NodeContentList = lines[i][3:j]
    NodeContent =  ' '.join(NodeContentList)
    NodeContent = NodeContent.replace(' ', '')
    if(nodeType == 'PUSH'):
        NodeContent = 'PUSH'+' '+ NodeContent
    if(nodeType == 'SOLVE'):
        NodeContent = 'SOLVE'+' '+ NodeContent
    if (len(lines[i][j+1:]) == 1):
        NodeID[nodeIdx] = drawNode(px,py,lines[i][1],i,NodeContent,lines[i][j+1],lines[i][j+1],fillNodeColor1,NodeColor)
        NodeNext[nodeIdx]   = [int(lines[i][j+1])]
    else:
        NodeID[nodeIdx] = drawNode(px,py,lines[i][1],i,NodeContent,lines[i][j+1],lines[i][j+2],fillNodeColor1,NodeColor)
        NodeNext[nodeIdx]   = [int(lines[i][j+1]),int(lines[i][j+2])]
py = 4*radius+yOffset;

NodeID[1] = drawNode(pxt[0]+step,py,lines[1][1],1,'Z = '+ lines[1][3],-1,-1,fillNodeColor1,NodeColor)
NodeCenter[1] = [pxt[0]+step,py];


for i in range(2,nodeIdx+1):
    if len(NodeNext[i]) == 1:
        drawArrow(NodeCenter[i][0],NodeCenter[i][1],NodeCenter[NodeNext[i][0]][0],NodeCenter[NodeNext[i][0]][1],normalArrowColor)
    else:
        if NodeNext[i][0] == 0:
            drawArrow(NodeCenter[i][0],NodeCenter[i][1],NodeCenter[i][0]+step/2,NodeCenter[i][1]+step/2,trueArrowColor)
            C.create_line(NodeCenter[i][0]+step/2+10,NodeCenter[i][1]+step/2-radius,NodeCenter[i][0]+step/2-10,NodeCenter[i][1]+step/2-radius,width = radius/10,fill=trueArrowColor)            
        else:
            drawArrow(NodeCenter[i][0],NodeCenter[i][1],NodeCenter[NodeNext[i][0]][0],NodeCenter[NodeNext[i][0]][1],trueArrowColor)
        if NodeNext[i][1] == 0:
            drawArrow(NodeCenter[i][0],NodeCenter[i][1],NodeCenter[i][0]+step/2,NodeCenter[i][1]+step/2,falseArrowColor)
            C.create_line(NodeCenter[i][0]+step/2+10,NodeCenter[i][1]+step/2-radius,NodeCenter[i][0]+step/2-10,NodeCenter[i][1]+step/2-radius,width = radius/10,fill=falseArrowColor)
        else:
            drawArrow(NodeCenter[i][0],NodeCenter[i][1],NodeCenter[NodeNext[i][1]][0],NodeCenter[NodeNext[i][1]][1],falseArrowColor)        

#=====================================================================
s = 10;
e = 10;
length = 60;
width = 50;
sStep = width/3 + width;
wordFillColor = fillNodeColor1;
lenQuesion =4;

C.create_rectangle(s+sStep*1, e ,s+sStep*(lenQuesion-1)+length ,e+width ,width = 4.0,fill=wordFillColor);
C.create_text((s+sStep+s+sStep*(lenQuesion-1)+length)/2, e + width/2, text = '...Question...', fill='black',font=('Times 20 italic bold',fontSize));
    
i=0;
C.create_rectangle(s+sStep*i, e ,s+sStep*i+length ,e+width ,width = 4.0,fill=wordFillColor);
C.create_text(s+sStep*i+length/2, e + width/2, text = 'S', fill='black',font=('Times 20 italic bold',fontSize));
i=lenQuesion;
C.create_rectangle(s+sStep*i, e ,s+sStep*i+length ,e+width ,width = 4.0,fill=wordFillColor);
C.create_text(s+sStep*i+length/2, e + width/2, text = 'E' , fill='black',font=('Times 20 italic bold',fontSize));
#=====================================================================
            
if len(memory)>1:
    s = 10;
    e = 10+70;
    length = 60;
    width = 45;
    sStep = width/3 + width;
    wordFillColor = fillNodeColor1;
    for i in range(1,len(memory)):
        C.create_rectangle(s+sStep*i, e ,s+sStep*i+length ,e+width ,width = 4.0,fill=wordFillColor);
        C.create_text(s+sStep*i+length/2, e + width/2, text = memory[i], fill='black',font=('Times 20 italic bold',fontSize));

    i=0;
    C.create_rectangle(s+sStep*i, e ,s+sStep*i+length ,e+width ,width = 4.0,fill=wordFillColor);
    C.create_text(s+sStep*i+length/2, e + width/2, text = 'X' + digitSubscript[1], fill='black',font=('Times 20 italic bold',fontSize));
    i=len(memory);
    C.create_rectangle(s+sStep*i, e ,s+sStep*i+length ,e+width ,width = 4.0,fill=wordFillColor);
    C.create_text(s+sStep*i+length/2, e + width/2, text = 'X' + digitSubscript[0], fill='black',font=('Times 20 italic bold',fontSize));
#=====================================================================
root.mainloop()
#=====================================================================
