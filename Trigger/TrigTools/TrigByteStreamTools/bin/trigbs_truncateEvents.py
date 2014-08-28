#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#tool to corrupt fragment in data file

import sys
import array

def readEvent(file):
    event = array.array('I')
    event.read(file, 4)
    if event[0]!=0x1234cccc:
        inFile.seek(-16,1)
        return None
#    for val in event:
#        print '%08x' % val

    event.read(file, event[3]/4)
    return event

def corruptEvent(event,badList):
    newEvent=array.array('I')
    headerSize=event[4+2]
    robIdx=4+headerSize
    #copy header - we will adjust size at the end
    newEvent.fromlist(event.tolist()[0:robIdx])
    reducedWords=0
    while robIdx<len(event):
        rob=event[robIdx:robIdx+event[robIdx+1]]
        #corrupt rob
        id=rob[4]
        if (id >= badList[0]) and (id<=badList[1]):
            print 'Found rob to be truncated: %08x - org length %d' % (id,len(rob))
            if (len(rob)>50): #don't truncate basically empty fragments
                newrob=array.array('I',rob[0:len(rob)-20])
                reducedWords+=20
                newrob[1]-=20
                newrob[6]|=(1<<27)|8 #mark as truncated
                #make sure trailer is messed up
                newrob[-1]=0xfe77efdd
                newrob[-2]=0xfe77efdd
                newrob[-3]=0xfe77efdd
                rob=newrob
        else:
            print 'good rob %08x' % (id)
#        newEvent.fromlist(rob.tolist())
        newEvent.extend(rob)
        robIdx+=event[robIdx+1]
    newEvent[3]-=4*reducedWords
    newEvent[5]-=reducedWords
    return newEvent

input_file=sys.argv[1]
output_file=sys.argv[2]

inFile = open(input_file, mode='rb')
outFile= open(output_file, mode='wb')

#first copy headers - should not have any size in it
cnt=0
while True:
    binvalues = array.array('I')
    binvalues.fromfile(inFile, 1)
    if binvalues[-1]==0x1234cccc:
        inFile.seek(-4,1)
        break
    binvalues.tofile(outFile)

while True:
    event=readEvent(inFile)
    if not event: break
    #corrupt event
#    corrupt=corruptEvent(event,range(0x00670001,0x0067000c))
#all of ID
    corrupt=corruptEvent(event,[0x00100000,0x00350000])
    corrupt.tofile(outFile)

#copy tail record
binvalues = array.array('I')
binvalues.fromfile(inFile, 10)
binvalues.tofile(outFile)

    
