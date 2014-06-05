#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# set the folder description for the trigger schema (not set by default)

import sys,getopt,os
from PyCool import cool,coral
import ROOT
from ROOT import TCanvas, TH1F, TFile

try:
    longopts=['cooldb=','r=','help']
    opts,args=getopt.getopt(sys.argv[1:],'',longopts)
except getopt.GetoptError,e:
    print e
    print "Wrong option"
    sys.exit(-1)

lb1=int(0)
lb2=int(0)
debug=bool(False)

for o,a in opts:
    if (o=='--r'): run=int(a)

print
print "------------------------------------"
print "Run",run
print "------------------------------------"
print

# get database service and open database
dbSvc=cool.DatabaseSvcFactory.databaseService()
dbstring="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=COMP200;user=ATLAS_COOL_READER;password=COOLRED4PRO"

try:
    db=dbSvc.openDatabase(dbstring,False)
    if (debug) : print "Opened database",dbstring
except Exception,e:
    print e
    sys.exit(-1)

# set folders all run-event index
# AthenaAttributeList as type 0, CondAttributeList collection as type 1
# timestamp for folders bit 1

# LVL1 trigger counters for each trigger item, before and after pre-scales,
# and after veto (deadtime, busy, etc.). For each counter there is an overflow flag. Indexed by run-LB
lvl1counters_foldername='/TRIGGER/LUMI/LVL1COUNTERS'
# VL1 menu items, indexed by run-LB.
lvl1menu_foldername='/TRIGGER/LVL1/Menu'
lvl1lbdata_foldername='/TRIGGER/LVL1/CTPCORELBDATA'
# Information on start-end time of luminoisty blocks, indexed by run-LB.
lblb_foldername='/TRIGGER/LUMI/LBLB'
lvl1lbcoreinput_foldername='/TRIGGER/LVL1/CTPCOREINPUTS'
if not db.existsFolder(lvl1counters_foldername) :
    print "Folder",lvl1counters_foldername,"not found"
    db.closeDatabase()
    sys.exit(-1)
if not db.existsFolder(lvl1menu_foldername) :
    print "Folder",lvl1menu_foldername,"not found"
    db.closeDatabase()
    sys.exit(-1)
if not db.existsFolder(lvl1lbdata_foldername) :
    print "Folder",lvl1lbdata_foldername,"not found"
    db.closeDatabase()
    sys.exit(-1)
if not db.existsFolder(lblb_foldername) :
    print "Folder",lblb_foldername,"not found"
    db.closeDatabase()
    sys.exit(-1)
if not db.existsFolder(lvl1lbcoreinput_foldername) :
    print "Folder",lvl1lbcoreinput_foldername,"notfound"
    db.closeDatabase()
    sys.exit(-1)


items=[""]*256
L1As=[]
L1ATurns=[]
TAVs=[]
TAPs=[]
TBPs=[]
startTimes=[]
endTimes=[]
PITs=[]
folder=db.getFolder(lvl1menu_foldername)
try:
    itr=folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())
    while itr.goToNext() :
        obj=itr.currentRef()
        itemNo=obj.channelId()
        payload=obj.payload()
        items[itemNo] = payload['ItemName']
    itr.close()
except Exception,e:
    print "Reading data from",lvl1menu_foldername,"failed:",e

if len(items) != 256 :
    print "Number of items should be 256, instead ", len(items), "items found"

 
folder=db.getFolder(lvl1lbdata_foldername)
try:
    itr=folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())
    while itr.goToNext() :
        obj=itr.currentRef()
        payload=obj.payload()
        L1As.append(payload['GlobalL1AcceptCounter'])
        L1ATurns.append(payload['GlobalL1AcceptTurnCounter'])
            
    itr.close()
except Exception,e:
    print "Reading data from",lvl1lbdata_foldername,"failed:",e

folder=db.getFolder(lblb_foldername)
try:
    itr=folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())
    while itr.goToNext() :
        obj=itr.currentRef()
        payload=obj.payload()
        startTimes.append(payload['StartTime'])
        endTimes.append(payload['EndTime'])
            
    itr.close()
except Exception,e:
    print "Reading data from",lvl1lbdata_foldername,"failed:",e

folder=db.getFolder(lvl1counters_foldername)
try:
    itr=folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())
    lb = 0
    oldItem = -1
    while itr.goToNext():
        obj=itr.currentRef()
        itemNo=obj.channelId()
        if (oldItem == -1 or itemNo == 0) :
            tavs=[]
            TAVs.append(tavs)
            tbps=[]
            TBPs.append(tbps)
            taps=[]
            TAPs.append(taps)            
        if (oldItem != itemNo) :
            lb = 0
        else :
            lb += 1
        payload=obj.payload()            
        TAVs[lb].append(int(payload['L1Accept']))
        TBPs[lb].append(int(payload['BeforePrescale']))
        TAPs[lb].append(int(payload['AfterPrescale']))
        oldItem = itemNo
        
    itr.close()
except Exception,e:
    print "Reading data from",lvl1lbdata_foldername,"failed:",e

folder=db.getFolder(lvl1lbcoreinput_foldername)
try:
    itr=folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())
    lb = 0
    oldItem = -1
    while itr.goToNext() :
        obj=itr.currentRef()
        itemNo=obj.channelId()
        if (oldItem == -1 or itemNo == 0) :
            pits=[]
            PITs.append(pits)
        if (oldItem != itemNo) :
            lb = 0
        else :
            lb += 1
        payload=obj.payload()
        PITs[lb].append(int(payload['PIT_TriggerInput']))
        oldItem = itemNo
        
    itr.close()


except Exception,e:
    print "Reading data from",lvl1lbcoreinput_foldername,"failed:",e

if (len(L1As) != len(endTimes)) :
    print "Problem: length of L1A vector (",len(L1As),") not equal length of time vector (",len(endTimes),")"


#----------------------------------------------------------------
# calculate the lumi block length and the L1A rate
#----------------------------------------------------------------
L1Arate=[]
length=[]
LB = 0
for i in range(0,len(L1As)) :
    length.append(1e-9*(endTimes[i] - startTimes[i]))
    rate1=float(L1As[i]) / float(L1ATurns[i]) * 11246.0
    L1Arate.append(float(L1As[i]) / length[i])
    LB += 1

#----------------------------------------------------------------
# calculate the rates per Lumi block and per item
#----------------------------------------------------------------
TAVrate=[]
TAPrate=[]
TBPrate=[]
for j in range(0,len(items)) :
    tavrate=[]
    taprate=[]
    tbprate=[]
    for i in range(0,len(L1As)) :
        tavrate.append(float(TAVs[i][j]) / length[i])
        taprate.append(float(TAPs[i][j]) / length[i])
        tbprate.append(float(TBPs[i][j]) / length[i])
    TAVrate.append(tavrate)
    TAPrate.append(taprate)
    TBPrate.append(tbprate)

#----------------------------------------------------------------
# calculate PITs rates per Lumi block
#----------------------------------------------------------------
PITrate=[]
for j in range(0,159) :
    pitrate=[]
    for i in range(0,len(L1As)) :
        pitrate.append(float(PITs[i][j]) / length[i])
    PITrate.append(pitrate)

#-----------------------------------------------------------------
# helper function to fill histograms  
#-----------------------------------------------------------------
def makeHist(name, title, Ytitle, file, vect, dir, nbins, xmin, xmax) :

    histFile = TFile(fileName,"UPDATE")
    htest = TH1F( name, title, nbins, xmin, xmax )
    for j in range(0, nbins) :
        htest.SetBinContent(j+1, vect[j])
    htest.SetXTitle("Luminosity Block")
    htest.SetYTitle(Ytitle)
    histFile.Cd(dir)    
    htest.Write()
    histFile.Close()
#-----------------------------------------------------------------
#-----------------------------------------------------------------

fileName = "RUN_" + str(run) + "_hist.root"
histFile = TFile(fileName,"RECREATE")
histFile.mkdir("TAV")
histFile.mkdir("TAP")
histFile.mkdir("TBP")
histFile.mkdir("PIT")
histFile.Close()
name ="h_LB_duration"
title = "luminosity block duration"
makeHist(str(name), str(title), "Time [s]",fileName, length, str(), int(LB), int(1), int(LB+1))
name ="h_L1A_rate"
title = "L1A rate vs luminosity block"
makeHist(str(name), str(title), "Rate [Hz]",fileName, L1Arate, str(), int(LB), int(1), int(LB+1))
for i in range(0, len(items)) :
    if (len(items[i]) == 0) :
        continue
        if (TBPrate[i] == 0) :
            print "item #",i,"has no item name but has a rate different than zero! skipping it"
    name = "h_tav_"+items[i]
    title = items[i] + " TAV rate vs luminosity block"
    makeHist(str(name), str(title), "Rate [Hz]", str(histFile), TAVrate[i], str("TAV"), int(LB), int(1), int(LB+1))
    name = "h_tap_"+items[i]
    title = items[i] + " TAP rate vs luminosity block"
    makeHist(str(name), str(title), "Rate [Hz]", str(histFile), TAPrate[i], str("TAP"), int(LB), int(1), int(LB+1))
    name = "h_tbp_"+items[i]
    title = items[i] + " TBP rate vs luminosity block"
    makeHist(str(name), str(title), "Rate [Hz]", str(histFile), TBPrate[i], str("TBP"), int(LB), int(1), int(LB+1))

for i in range(0, 159) :
    name = "h_pit"+str(i)
    title = "PIT item "+str(i)+" rate vs luminosity block"
    makeHist(str(name), str(title), "Rate [Hz]", str(histFile), PITrate[i], str("PIT"), int(LB), int(1), int(LB+1))

print "luminosity blocks: ", len(L1As)
db.closeDatabase()
