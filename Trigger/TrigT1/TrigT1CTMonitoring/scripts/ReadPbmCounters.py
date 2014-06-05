#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys,getopt,os, math
from PyCool import cool,coral
from ROOT import TH1F, TH1D,TFile, TProfile, TObject, TGraph
import CoolHelper

def decodeBlob(blob, index = 0):
    # one block in the blob is 3*3564 long; 3564 is the number of BCIDs,
    # for each BCID, we have a 24-bit counter, which is stored as 3
    # separate bytes...
    startIndex = index * (3*3564)
    stopIndex = (index+1) * (3*3564)
    returnList = []
    byteNum = int(0)
    counter = int(0)
    for id in range(startIndex,stopIndex):
        byte = blob[id]
        counter |= (byte << (byteNum%3)*8)
        if (byteNum%3) == 2:
            returnList.append(counter)
            counter = int(0)
        byteNum += 1
    return returnList

def usage():
    print "%s" % sys.argv[0]
    print "  --help                         this printout"
    print "  --run <runNo>                  run number"
    print "  --lb <lb1>-<lb2>               LB range"
    print "  --trig                         flag to read trigger counters, too"
    print "  --dtLb                         store all deadtimes per LB, else only combined high/low priorities will be stored per LB"

try:
    longopts=['run=','help','lb=','trig','dtLb']
    opts,args=getopt.getopt(sys.argv[1:],'',longopts)
except getopt.GetoptError,e:
    usage()
    sys.exit(2)

lbList=[]
run=int(-1)
doTrig = False
doDtLb = False
for o,a in opts:
    if o in ('-h','--help'):
        usage()
        sys.exit(0)
    if o in ('-t','--trig'):
        doTrig = True
    if o in ('-d','--dtLb'):
        doDtLb = True
    if o == '--run':
        run = int(a)
    if o == '--lb':
            list1 = str(a).split('-')
            if len(list1) > 1 : lbList += range(int(list1[0]),int(list1[1])+1)
            else : lbList.append(int(list1[0]))

if (run == -1 or len(lbList) == 0) : 
    usage()
    sys.exit(2)

# get database service and open database
dbSvc=cool.DatabaseSvcFactory.databaseService()
dbstring="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=COMP200;user=ATLAS_COOL_READER;password=COOLRED4PRO"

try:
    db=dbSvc.openDatabase(dbstring,False)
except Exception,e:
    print e
    sys.exit(-1)

foldername='/TRIGGER/LUMI/PerBcidDeadtime'
if not db.existsFolder(foldername) :
    print "Folder",foldername,"not found"
    db.closeDatabase()
    sys.exit(-1)
    
pr0 = TProfile("LowPriority","Low priority deadtime fraction;BCID;Veto fraction",3564,-0.5,3563.5,0.,1.0,"s")
pr1 = TProfile("HighPriority","High priority deadtime fraction;BCID;Veto fraction",3564,-0.5,3563.5,0.,1.0,"s")
pr2 = TProfile("DtDaq","DAQ deadtime fraction;BCID;Veto fraction",3564,-0.5,3563.5,0.,1.0,"s")
pr3 = TProfile("DtSimple","Simple deadtime fraction;BCID;Veto fraction",3564,-0.5,3563.5,0.,1.0,"s")
pr4 = TProfile("DtHighPriority","High priority deadtime fraction;BCID;Veto fraction",3564,-0.5,3563.5,0.,1.0,"s")
pr5 = TProfile("L1A","L1A rate;BCID;Rate ( Hz )",3564,-0.5,3563.5,"s")
if doTrig:
    pr6 =  TH1F("TAP0","TAP0 counts;BCID;# Triggers",3564,-0.5,3563.5)
    pr7 =  TH1F("TAP1","TAP1 counts;BCID;# Triggers",3564,-0.5,3563.5)
    pr8 =  TH1F("TAP2","TAP2 counts;BCID;# Triggers",3564,-0.5,3563.5)
    pr9 =  TH1F("TAP3","TAP3 counts;BCID;# Triggers",3564,-0.5,3563.5)
    pr10 = TH1F("TAP4","TAP4 counts;BCID;# Triggers",3564,-0.5,3563.5)
    pr11 = TH1F("TAP5","TAP5 counts;BCID;# Triggers",3564,-0.5,3563.5)
    pr12 = TH1F("TAV0","TAV0 counts;BCID;# Triggers",3564,-0.5,3563.5)
    pr13 = TH1F("TAV1","TAV1 counts;BCID;# Triggers",3564,-0.5,3563.5)
    pr14 = TH1F("TAV2","TAV2 counts;BCID;# Triggers",3564,-0.5,3563.5)
    pr15 = TH1F("TAV3","TAV3 counts;BCID;# Triggers",3564,-0.5,3563.5)
    pr16 = TH1F("TAV4","TAV4 counts;BCID;# Triggers",3564,-0.5,3563.5)
    pr17 = TH1F("TAV5","TAV5 counts;BCID;# Triggers",3564,-0.5,3563.5)

# now get physics bunch group
bgKeys = CoolHelper.readBunchGroupKeys(run)
bgNames, bgItemMap = CoolHelper.readBunchGroupDescription(run)
bgFullContent = CoolHelper.readBunchGroupContent(run)
bcidList = CoolHelper.extractBcids(bgItemMap['L1_EM10'], bgFullContent)
bb = TH1F("PhysicsBunchgroup","Physics bunch group;BCID;a.u.",3564,-0.5,3563.5)
testLb = 0
for lb in bcidList.keys():
    if lb > testLb:
        testLb = lb
for bcid in bcidList[testLb]:
    bb.Fill(bcid,1)

lbHistList = []

folder=db.getFolder(foldername)
try:
    itr=folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())
    lbNum = 1
    while itr.goToNext() :
        obj=itr.currentRef()
        payload=obj.payload()
        lb=(obj.since() & 0xffff)
        turnCount = payload["TurnCounter"]
        blobLow = payload["LowPriority"]
        blobHigh = payload["HighPriority"]
        if lb in lbList:
            print "LB count",lbNum,"/",len(lbList)
            lbNum += 1
            hName = "HighPriority_lb"
            hName += str(int(lb))
            lbHist = TH1F(hName,"High priority deadtime fraction;BCID;Veto fraction",3564,-0.5,3563.5)
            lbHistList.append(lbHist)
            bcidDeadLow = decodeBlob(blobLow)
            bcidDeadHigh = decodeBlob(blobHigh)
            for bcid in range(3564):
                pr0.Fill(bcid, float(bcidDeadLow[bcid]) / float(turnCount) )
                pr1.Fill(bcid, float(bcidDeadHigh[bcid]) / float(turnCount) )
                lbHist.Fill(bcid,float(bcidDeadHigh[bcid]) / float(turnCount))
    itr.close()
except Exception,e:
    print "Reading data from",foldername,"failed:",e

db.closeDatabase()

####################################
# < --
# now read further trigger counters
dbstring="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=MONP200;user=ATLAS_COOL_READER;password=COOLRED4PRO"
#dbstring = "TRIG_PIT/MONP200"
try:
    db=dbSvc.openDatabase(dbstring,False)
except Exception,e:
    print e
    sys.exit(-1)

foldername='/TRIGGER/LVL1/PerBcidCounters'
if not db.existsFolder(foldername) :
    print "Folder",foldername,"not found"
    db.closeDatabase()
    sys.exit(-1)

folder=db.getFolder(foldername)
try: 
    itr=folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())
    lbNum = 1
    while itr.goToNext() :
        obj=itr.currentRef()
        payload=obj.payload()
        lb=(obj.since() & 0xffff)
        turnCount = payload["TurnCounter"]
        blobL1A = payload["L1A"]
        blob1 = payload["DT2"]
        blob2 = payload["DT3"]
        blob3 = payload["DT4"]
        if doTrig:
            blob4 = payload["TrigA0"]
            blob5 = payload["TrigA1"]
            blob6 = payload["TrigA2"]
            blob7 = payload["TrigB0"]
            blob8 = payload["TrigB1"]
            blob9 = payload["TrigB2"]

        if lb in lbList:
            print "LB count",lbNum,"/",len(lbList)
            lbNum += 1
            if doDtLb:
                hName = "DtDaq_lb"
                hName += str(int(lb))
                hTitle = "DAQ deadtime fraction LB "
                hTitle += str(int(lb))
                hTitle += ";BCID;Veto fraction"
                lbHistDT2 = TH1F(hName,hTitle,3564,-0.5,3563.5)
                lbHistList.append(lbHistDT2)
                hName = "DtSimple_lb"
                hName += str(int(lb))
                hTitle = "Simple deadtime fraction LB "
                hTitle += str(int(lb))
                hTitle += ";BCID;Veto fraction"
                lbHistDT3 = TH1F(hName,hTitle,3564,-0.5,3563.5)
                lbHistList.append(lbHistDT3)
                hName = "DtHighPriority_lb"
                hName += str(int(lb))
                hTitle = "High priority deadtime fraction LB "
                hTitle += str(int(lb))
                hTitle += ";BCID;Veto fraction"
                lbHistDT4 = TH1F(hName,hTitle,3564,-0.5,3563.5)
                lbHistList.append(lbHistDT4)

            bcidL1A = decodeBlob(blobL1A)
            bcidDT2 = decodeBlob(blob1)
            bcidDT3 = decodeBlob(blob2)
            bcidDT4 = decodeBlob(blob3)
            if doTrig:
                bcidTrigTAP0 = decodeBlob(blob4,0)
                bcidTrigTAP1 = decodeBlob(blob4,1)
                bcidTrigTAP2 = decodeBlob(blob5,0)
                bcidTrigTAP3 = decodeBlob(blob5,1)
                bcidTrigTAP4 = decodeBlob(blob6,0)
                bcidTrigTAP5 = decodeBlob(blob6,1)
                bcidTrigTAV0 = decodeBlob(blob7,0)
                bcidTrigTAV1 = decodeBlob(blob7,1)
                bcidTrigTAV2 = decodeBlob(blob8,0)
                bcidTrigTAV3 = decodeBlob(blob8,1)
                bcidTrigTAV4 = decodeBlob(blob9,0)
                bcidTrigTAV5 = decodeBlob(blob9,1)
            for bcid in range(3564):
                pr2.Fill(bcid,float(bcidDT2[bcid]) / float(turnCount))
                pr3.Fill(bcid,float(bcidDT3[bcid]) / float(turnCount))
                pr4.Fill(bcid,float(bcidDT4[bcid]) / float(turnCount))
                pr5.Fill(bcid,float(bcidL1A[bcid]) / float(turnCount) * 11246.)

                if doDtLb:
                    lbHistDT2.Fill(bcid,float(bcidDT2[bcid]) / float(turnCount))
                    lbHistDT3.Fill(bcid,float(bcidDT3[bcid]) / float(turnCount))
                    lbHistDT4.Fill(bcid,float(bcidDT4[bcid]) / float(turnCount))

                if doTrig:
                    pr6.Fill(bcid,bcidTrigTAP0[bcid])
                    pr7.Fill(bcid,bcidTrigTAP1[bcid])
                    pr8.Fill(bcid,bcidTrigTAP2[bcid])
                    pr9.Fill(bcid,bcidTrigTAP3[bcid])
                    pr10.Fill(bcid,bcidTrigTAP4[bcid])
                    pr11.Fill(bcid,bcidTrigTAP5[bcid])
                    pr12.Fill(bcid,bcidTrigTAV0[bcid])
                    pr13.Fill(bcid,bcidTrigTAV1[bcid])
                    pr14.Fill(bcid,bcidTrigTAV2[bcid])
                    pr15.Fill(bcid,bcidTrigTAV3[bcid])
                    pr16.Fill(bcid,bcidTrigTAV4[bcid])
                    pr17.Fill(bcid,bcidTrigTAV5[bcid])

    itr.close()
except Exception,e:
    print "Reading data from",foldername,"failed:",e
db.closeDatabase()

fileName = "PerBcidMon_Run"
fileName += str(run)
fileName += ".root"
tfile = TFile(fileName,"recreate")
pr0.Write()
pr1.Write()
pr2.Write()
pr3.Write()
pr4.Write()
pr5.Write()
if doTrig:
    pr6.Write()
    pr7.Write()
    pr8.Write()
    pr9.Write()
    pr10.Write()
    pr11.Write()
    pr12.Write()
    pr13.Write()
    pr14.Write()
    pr15.Write()
    pr16.Write()
    pr17.Write()
subdir = tfile.mkdir("LbHists")
subdir.cd()
for h in lbHistList:
    h.Write()

tfile.cd()
bb.Write()
tfile.Close()

#            bcidDeadLow = []
#            bcidDeadHigh = []
#            print "Found lb",lb,turnCount,"turns",len(blobHigh),"bytes in blob"
#            byteNum = int(0)
#            counter = int(0)
#            for byte in blobLow:
#                counter |= (byte << (byteNum%3)*8)
#                if (byteNum%3) == 2:
#                    bcidDeadLow.append(counter)
##                    bcidDeadLow.append(float(counter) / float(turnCount))
#                    counter = int(0)
#                byteNum += 1

#            byteNum = int(0)
#            counter = int(0)
#            for byte in blobHigh:
#                counter |= (byte << (byteNum%3)*8)
#                if (byteNum%3) == 2:
#                    bcidDeadHigh.append(float(counter) / float(turnCount))
#                    counter = int(0)
#                byteNum += 1
