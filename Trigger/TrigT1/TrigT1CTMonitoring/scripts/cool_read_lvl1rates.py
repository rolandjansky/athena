#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# TODO: check for overflows of counters!
# TODO: option to print out counters instead of rates

import sys,getopt,os, math
from PyCool import cool,coral
from CoolConvUtilities import AtlCoolTool
from ROOT import TH1F, TH1D,TFile, TProfile, TObject, TGraph

def usage():
    print "%s" % sys.argv[0]
    print "  --help                                this printout"
    print "  --run <runNo>                         run number"
    print "  --item <name>                         item name - 'ALL' for all items"
    print "  --itemList <i1>,<i2>,...              comma separated list of item names"
    print "  --ctpid <id>                          use ctpid of item instead of name - '-1' for all items"
    print "  --ctpidRange <i1>-<i2>,<i3>-<i4>      range of ctpids "
    print "  --enabledItems                        print all items enabled at L1"
    print "  --printNumbers                        print number of triggers"
    print "  --printRates                          print trigger rates"
    print "  --printSummary                        just print summary per item"
    print "  --fillHist                            fill histograms of rate vs. LB"

    print "  example for making a plot: "
    print "   cool_read_lvl1rates.py --run 152166 --fillHist"
    print "   cool_plot_lvl1rates.py -i RUN_152166_hist.root --counters MBTS_1$,L1_LUCID_A$,L1_EM3$,ZDC_A$ --TAV --outbase TAV"

try:
    longopts=['run=','ctpid=','item=','help','printNumbers','printRates','itemList=','ctpidRange=','fillHist','printSummary','enabledItems']
    opts,args=getopt.getopt(sys.argv[1:],'',longopts)
except getopt.GetoptError,e:
    usage()
    sys.exit(2)

lb1=int(0)
lb2=int(0)
debug=bool(False)
ctpid=int(-2)
ctpidRange=[]
itemName="ALL"
itemList=[]
run=int(-1)
printRates=False
printNumbers=False
fillHist=False
printSummary=False
orbitFreq = 40.0790e6 / 3564.
enabledItems = False

for o,a in opts:
    if o in ('-h','--help'):
        usage()
        sys.exit(0)
    if o == '--run':
        run = int(a)
    if o == '--item':
        itemName = str(a)
    if o == '--itemList':
        itemName = ''
        itemList = str(a).split(',')
    if o == '--ctpid':                 
        itemName = ''
        ctpid = int(a)
    if o == '--ctpidRange':
        itemName = ''
        list1 = str(a).split(',')
        for i in list1:
            list2 = i.split('-')
            ctpidRange += range(int(list2[0]),int(list2[1])+1)
    if o == '--printNumbers':
        printNumbers = True
    if o == '--printRates':
        printRates = True
    if o == '--printSummary':
        printSummary = True
    if o == '--fillHist':
        fillHist = True
    if o == '--enabledItems':
        enabledItems = True

if not printSummary and not printRates and not printNumbers:
    printSummary = True

if (run == -1) : 
    usage()
    sys.exit(2)

if printNumbers and printRates :
    print "Both arguments 'printNumbers' and 'printRates' given, print only rates"
    printNumbers = False

    print
    print "------------------------------------"
    print "Run",run
    print "------------------------------------"
    print

# get database service and open database
dbSvc=cool.DatabaseSvcFactory.databaseService()
dbstring="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=COMP200;user=ATLAS_COOL_READER;password=COOLRED4PRO"
#dbstring="sqlite://;schema=/afs/cern.ch/user/b/berge/trash/ctp.LBData.1190647872.sqlitefile.db;dbname=L1CTCOOL"

try:
    db=dbSvc.openDatabase(dbstring,False)
    if (debug) : print "Opened database",dbstring
except Exception,e:
    print e
    sys.exit(-1)

# set folders all run-event index
# AthenaAttributeList as type 0, CondAttributeList collection as type 1
# timestamp for folders bit 1

lvl1counters_foldername='/TRIGGER/LUMI/LVL1COUNTERS'
lvl1menu_foldername='/TRIGGER/LVL1/Menu'
lvl1lbdata_foldername='/TRIGGER/LVL1/CTPCORELBDATA'
lblb_foldername='/TRIGGER/LUMI/LBLB'
l1ps_fname = '/TRIGGER/LVL1/Prescales'

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
if not db.existsFolder(l1ps_fname) :
    print "Folder",l1ps_fname,"not found"
    db.closeDatabase()
    sys.exit(-1)
         
items=[""]*256
L1As=[]
L1ATurns=[]
TAVs=[]
TAPs=[]
TBPs=[]
TAVsOl=[]
TAPsOl=[]
TBPsOl=[]
TAVTurns=[]
TAPTurns=[]
TBPTurns=[]
LBs=[]
startTimes=[]
endTimes=[]
l1ps_dict={}

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
 
folder=db.getFolder(lvl1lbdata_foldername)
try:
    itr=folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())
    while itr.goToNext() :
        obj=itr.currentRef()
        payload=obj.payload()
        L1As.append(payload['GlobalL1AcceptCounter'])
        L1ATurns.append(payload['GlobalL1AcceptTurnCounter'])
        TAVTurns.append(payload['L1AcceptTurnCounter'])
        TAPTurns.append(payload['AfterPrescaleTurnCounter'])
        TBPTurns.append(payload['BeforePrescaleTurnCounter'])
            
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







#starttimes=ROOT.TH1D("LB_start","LB_start",len(LBs),LBs[0]-0.5,LBs[-1]+0.5)
#stoptimes=ROOT.TH1D("LB_stop","LB_stop",len(LBs),LBs[0]-0.5,LBs[-1]+0.5)
#dttimes=ROOT.TH1D("LB_dt","LB_dt",len(LBs),LBs[0]-0.5,LBs[-1]+0.5)
#centertimes=ROOT.TH1D("LB_center","LB_center",len(LBs),LBs[0]-0.5,LBs[-1]+0.5)








folder=db.getFolder(l1ps_fname)
try:
    itr=folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())
    lb = 0
    while itr.goToNext():
        obj=itr.currentRef()
        lb=(obj.since() & 0xffff)
        itemNo=obj.channelId()
        payload=obj.payload()
        ps=payload['Lvl1Prescale']
        try :
            l1ps_dict[lb][itemNo] = ps            
        except KeyError:
            l1ps_dict[lb] = 256*[-1]
            l1ps_dict[lb][itemNo] = ps
except Exception,e:
    print "Reading data from",l1ps_fname,"failed:",e

folder=db.getFolder(lvl1counters_foldername)
try:
    itr=folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())
    lb = 0
    counter = 0
    oldItem = -1
    while itr.goToNext():
        obj=itr.currentRef()
        lb=(obj.since() & 0xffff)
        itemNo=obj.channelId()
        if (oldItem == -1 or itemNo == 0) :
            tavs=[]
            TAVs.append(tavs)
            tbps=[]
            TBPs.append(tbps)
            taps=[]
            TAPs.append(taps)            

            tavsol=[]
            TAVsOl.append(tavsol)
            tbpsol=[]
            TBPsOl.append(tbpsol)
            tapsol=[]
            TAPsOl.append(tapsol)            

            LBs.append(lb)

        if (oldItem != itemNo) :
            counter = 0
        else :
            counter += 1

        payload=obj.payload()            
        TAVs[counter].append(int(payload['L1Accept']))
        TBPs[counter].append(int(payload['BeforePrescale']))
        TAPs[counter].append(int(payload['AfterPrescale']))
        TAVsOl[counter].append(bool(payload['L1AcceptOverflow']))
        TBPsOl[counter].append(bool(payload['BeforePrescaleOverflow']))
        TAPsOl[counter].append(bool(payload['AfterPrescaleOverflow']))

        'L1AcceptOverflow'
        oldItem = itemNo
        
    itr.close()
except Exception,e:
    print "Reading data from",lvl1lbdata_foldername,"failed:",e


if (len(L1As) != len(endTimes)) :
    print "Problem: length of L1A vector (",len(L1As),") not equal length of time vector (",len(endTimes),")"

if (fillHist) :
    fileName = "RUN_" + str(run) + "_hist.root"
    histFile = TFile(fileName,"RECREATE")
    histFile.mkdir("TAV")
    histFile.mkdir("TAP")
    histFile.mkdir("TBP")

    def makeHist(label,name,LB_list) :
        histFile.cd(label)
        hist = TH1F( label + "_" + name, name + " " + label + " rate;LB number;Rate ( Hz )", len(LB_list), LB_list[0]-0.5, LB_list[-1]+0.5 )
        graph = TGraph(len(LB_list))
        graph.SetName(label + "_" + name + "_TimeGraph")
        return (hist, graph)

    histFile.cd()
    hTotalTbp = TProfile( "AverageTbpRatePerItem", "TBP Rates per Item, averaged over the whole run; CTP ID;Rate ( Hz )", 256, -0.5, 255.5)
    hTotalTap = TProfile( "AverageTapRatePerItem", "TAP Rates per Item, averaged over the whole run; CTP ID;Rate ( Hz )", 256, -0.5, 255.5)
    hTotalTav = TProfile( "AverageTavRatePerItem", "TAV Rates per Item, averaged over the whole run; CTP ID;Rate ( Hz )", 256, -0.5, 255.5)
    hL1a = TH1F( "GlobalL1ARate", "Global L1A rate;LB number;Rate ( Hz )", len(LBs), LBs[0]-0.5, LBs[-1]+0.5 )
    for i in range(0,len(L1As)) :
        rate2=float(L1As[i]) / float(L1ATurns[i]) * orbitFreq
        err_rate2 = math.sqrt(L1As[i])  / float(L1ATurns[i]) * orbitFreq
        hL1a.SetBinContent(LBs[i],rate2)
        hL1a.SetBinError(LBs[i],err_rate2)

if( printSummary ) :
    print "-----------------------------------------------------------------------------------------------------------------------------------------"
    if printNumbers :
        print '%30s %12s %20s %20s %20s %20s' % ("Item","ctpid", "L1A[#]","TAV[#]", "TAP[#]", "TBP[#]" )            
    else :
        print '%30s %12s %20s %20s %20s %20s' % ("Item","ctpid", "L1A[Hz]","TAV[Hz]", "TAP[Hz]", "TBP[Hz]" )            
    print "-----------------------------------------------------------------------------------------------------------------------------------------"

for j in range(0,len(items)) :
    doContinue = 0
    if ( itemName == "ALL" ) : doContinue += 1
    elif (itemName != '' and items[j] == itemName) : doContinue += 1
    elif ( ctpid==-1 ) : doContinue += 1
    elif ( ctpid>-1 and j == ctpid ) : doContinue += 1
    elif ( items[j] in itemList ) : doContinue += 1
    elif ( j in ctpidRange ) : doContinue += 1
    if (enabledItems) :
        doContinue = 0
        for key in l1ps_dict.keys():
            if(l1ps_dict[key][j] > 0) : doContinue += 1

    if( doContinue == 0 ) : continue

    if (fillHist) :
        # create Hists
        histName = "ctpid_" + str(j)
        if items[j] != "" : histName += "_Item_" + items[j]
        else : histName = str(j)
        hTav, gTav = makeHist("TAV",histName,LBs)
        hTap, gTap = makeHist("TAP",histName,LBs)
        hTbp, gTbp = makeHist("TBP",histName,LBs)

    if( not printSummary and (printNumbers or printRates) ) :
        print ""
        print "Item",items[j]," ( ctpid",j,")"
        print "-----------------------------------------------------------------------------------------------------------------------------------------"
        if printNumbers :
            print '%5.5s %20s %25s %18s %10s %13s %12s %12s %12s' % ("LB No","Start time", "End time", "Length[s]", "L1A[#]","L1A[Hz]","TAV[#]", "TAP[#]", "TBP[#]" )            
        else :
            print '%5.5s %20s %25s %18s %10s %13s %12s %12s %12s' % ("LB No","Start time", "End time", "Length[s]", "L1A[#]","L1A[Hz]","TAV[Hz]", "TAP[Hz]", "TBP[Hz]" )
        print "-----------------------------------------------------------------------------------------------------------------------------------------"

    totalTBP=int(0)
    totalTAP=int(0)
    totalTAV=int(0)
    totalLength=0.0
    totalTurns=0
    totalL1As=0
    for i in range(0,len(L1As)) :
        lbSec = 1e-9*(endTimes[i] + startTimes[i]) / 2
        length = 1e-9*(endTimes[i] - startTimes[i])
        start = AtlCoolTool.time.ctime(startTimes[i]/1E9)
        end = AtlCoolTool.time.ctime(endTimes[i]/1E9)

# do this only for the TBP
# import math
# rate3=-math.log10(1.-(float(TAVs[i][j]) / float(TAVTurns[i]))) * orbitFreq / 60E-27

        rate2=float(L1As[i]) / float(L1ATurns[i]) * orbitFreq
        rate3=float(TAVs[i][j]) / float(TAVTurns[i]) * orbitFreq
        rate4=float(TAPs[i][j]) / float(TAPTurns[i]) * orbitFreq
        rate5=float(TBPs[i][j]) / float(TBPTurns[i]) * orbitFreq
        err_rate3=math.sqrt(float(TAVs[i][j])) / float(TAVTurns[i]) * orbitFreq
        err_rate4=math.sqrt(float(TAPs[i][j])) / float(TAPTurns[i]) * orbitFreq
        err_rate5=math.sqrt(float(TBPs[i][j])) / float(TBPTurns[i]) * orbitFreq
        totalTBP += TBPs[i][j]
        totalTAP += TAPs[i][j]
        totalTAV += TAVs[i][j]
        totalL1As += L1As[i]
        totalLength += length
        totalTurns += float(TAVTurns[i])
        if TBPsOl[i][j] :
            over="<-- Overflow!"
        else :
            over=""

        if (fillHist) :
            hTav.SetBinContent(LBs[i],rate3)
            hTav.SetBinError(LBs[i],err_rate3)
            hTap.SetBinContent(LBs[i],rate4)
            hTap.SetBinError(LBs[i],err_rate4)
            hTbp.SetBinContent(LBs[i],rate5)
            hTbp.SetBinError(LBs[i],err_rate5)
            #hTav.Fill(LBs[i],rate3)
            #hTap.Fill(LBs[i],rate4)
            #hTbp.Fill(LBs[i],rate5)
            gTav.SetPoint(i,lbSec,rate3)
            gTap.SetPoint(i,lbSec,rate4)
            gTbp.SetPoint(i,lbSec,rate5)

            hTotalTbp.Fill(j,rate5)
            hTotalTap.Fill(j,rate4)
            hTotalTav.Fill(j,rate3)

        enabledAndPrint = (not enabledItems)
        if (enabledItems) :
            for pp in l1ps_dict.keys():
                if(pp <= LBs[i]): lb = pp
            if(l1ps_dict[lb][j] > 0) :
                enabledAndPrint = True
        if((enabledItems and enabledAndPrint) or (not enabledItems)) :
            if not printSummary and printRates :
                print '%4d %27.26s %27.27s %9.2f %10d %13.2f %12.2f %12.2f %12.2f %13s' % (LBs[i], start, end, length, L1As[i], rate2, rate3, rate4, rate5,over)
            elif not printSummary and printNumbers :
                print '%4d %27.26s %27.27s %9.2f %10d %13.2f %12.0f %12.0f %12.0f %13s' % (LBs[i], start, end, length, L1As[i], rate2, TAVs[i][j], TAPs[i][j], TBPs[i][j],over)

    if( not printSummary and (printNumbers or printRates) ) :
        print "-----------------------------------------------------------------------------------------------------------------------------------------"

    if printSummary:
        if printNumbers :
            print '%30s %12d %20d %20d %20d %20d' % (items[j],j,totalL1As,totalTAV,totalTAP,totalTBP )
        else :
            print '%30s %12d %20.2f %20.2f %20.2f %20.2f' % (items[j],j,float(totalL1As)/totalTurns*orbitFreq,float(totalTAV)/totalTurns*orbitFreq,float(totalTAP)/totalTurns*orbitFreq,float(totalTBP)/totalTurns*orbitFreq )
    
    # here make sure histograms are written properly

    if (fillHist) :
        histFile.cd("TAV")
        gTav.Write("",TObject.kOverwrite)
        histFile.cd("TAP")
        gTap.Write("",TObject.kOverwrite)
        histFile.cd("TBP")
        gTbp.Write("",TObject.kOverwrite)
        histFile.Write("",TObject.kOverwrite)

db.closeDatabase()


import numpy
def rootarray(inp):
    return numpy.array(inp, dtype=numpy.double)


if (fillHist) : 
    histFile.cd("")
    alltimes=startTimes+[endTimes[-1]]
    # scale to seconds:
    alltimes=map(lambda x:x/1e9,alltimes)
    times_lb=TH1D("LumiBlocks","exact lb at time",len(LBs),rootarray(alltimes))
    for t,b in zip(alltimes,LBs):
        times_lb.Fill(t+0.1,b)
    times_lb.GetXaxis().SetTimeDisplay(1);
    times_lb.GetXaxis().SetTimeFormat("%H:%M");        
    times_lb.Write()


    
    RunNR=TH1D("RunNR","RunNR",1,0,1);
    RunNR.Fill(0.5,run)
    RunNR.Write()
    histFile.Close()


