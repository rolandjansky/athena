#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys,getopt,os
from PyCool import cool,coral
from CoolConvUtilities import AtlCoolTool
from ROOT import TH1F, TFile, TProfile, TObject, TGraph, gROOT

def usage():
    print "%s" % sys.argv[0]
    print "  --help        this printout"
    print "  --run <runNo> run number"
    print "  --file <name> Input file"


try:
    longopts=['run=','file=','help']
    opts,args=getopt.getopt(sys.argv[1:],'',longopts)
except getopt.GetoptError,e:
    usage()
    sys.exit(2)

lbList={}
run=int(-1)
inFile=''
for o,a in opts:
    if o in ('-h','--help'):
        usage()
        sys.exit(0)
    if o == '--run':
        run = int(a)
    if o == '--file':
        inFile = str(a)

if (run == -1 or inFile == '') : 
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

lbDurations = TH1F("lbDurations",";LB Duration (s);Entries",10*40,100,140)

# set folders all run-event index
# AthenaAttributeList as type 0, CondAttributeList collection as type 1
# timestamp for folders bit 1

lblb_foldername='/TRIGGER/LUMI/LBLB'

if not db.existsFolder(lblb_foldername) :
    print "Folder",lblb_foldername,"not found"
    db.closeDatabase()
    sys.exit(-1)
         
folder=db.getFolder(lblb_foldername)
try:
    itr=folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())
    while itr.goToNext() :
        obj=itr.currentRef()
        payload=obj.payload()
        lb=int(obj.since() & 0xffff)
        # only accept lumi blocks at least 20 seconds long
        if (long(payload['EndTime']) - long(payload['StartTime'])) > 20e9 :
            lbList[lb] = [long(payload['StartTime']), long(payload['EndTime'])]
            lbDurations.Fill((long(payload['EndTime'])-long(payload['StartTime']))/1e9)
            startString = AtlCoolTool.time.ctime(payload['StartTime']/1E9)
            endString = AtlCoolTool.time.ctime(payload['EndTime']/1E9)

            print "LB",lb,"duration",(long(payload['EndTime'])-long(payload['StartTime']))/1e9,startString,endString
    itr.close()
except Exception,e:
    print "Reading data from",lvl1lbdata_foldername,"failed:",e

#print lbList

db.closeDatabase()

tagComm = TFile(inFile,'read')
gROOT.cd()
tree = tagComm.Get('POOLCollectionTree')

br1 = tree.GetBranch('LumiBlockN')
br2 = tree.GetBranch('EventTime')
br3 = tree.GetBranch('EventTimeNanoSec')

deltaTStart = TH1F("deltaTStart",";#DeltaT (ms);Events",1000, 0, 2000)
deltaTStop = TH1F("deltaTStop",";#DeltaT (ms);Events",1000, 0, 2000)

lbWiseHists = {}
for event in range(tree.GetEntries()):
#for event in range(500000):
    if not event%100000: print "Event",event
    br1.GetEntry(event)
    br2.GetEntry(event)
    br3.GetEntry(event)
    sec = br2.GetLeaf('EventTime').GetValue()
    nsec = br3.GetLeaf('EventTimeNanoSec').GetValue()
    eventTime = long(sec*1e9 + nsec)
    lumiBlock = int(br1.GetLeaf('LumiBlockN').GetValue())
    try:
        start = lbList[lumiBlock][0]
        end = lbList[lumiBlock][1]
        deltaTStart.Fill((eventTime-start)/1e6)
        deltaTStop.Fill((end-eventTime)/1e6)
        try:
            lbWiseHists[lumiBlock].Fill((eventTime-start)/1e6)            
        except KeyError:
            lbWiseHists[lumiBlock] = TH1F("deltaTStart_lb" + str(lumiBlock),";#DeltaT (ms);Events",1000, 0, 2000)
            lbWiseHists[lumiBlock].Fill((eventTime-start)/1e6)            
    except KeyError:
        pass

ofile = TFile("outfile.root","recreate")
deltaTStart.Write()
deltaTStop.Write()
lbDurations.Write()
for index in lbWiseHists: lbWiseHists[index].Write()
ofile.Close()
