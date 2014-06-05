#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys,getopt,os
from PyCool import cool,coral
from CoolConvUtilities import AtlCoolTool
from ROOT import TH1F, TFile, TProfile, TObject, TGraph

def usage():
    print "%s" % sys.argv[0]
    print "  --help                                this printout"
    print "  --run <runNo>                         run number"
    print "  --fillHist                            fill histograms with bunch groups"
    print "  --printBcids                          prit list of BCIDs per bunch group"


try:
    longopts=['run=','printBcids','fillHist','help']
    opts,args=getopt.getopt(sys.argv[1:],'',longopts)
except getopt.GetoptError,e:
    usage()
    sys.exit(2)

run=-1
fillHist=False
printBcids=True

for o,a in opts:
    if o in ('-h','--help'):
        usage()
        sys.exit(0)
    if o == '--run':
        run = int(a)
    if o == '--fillHist':
        fillHist = True
    if o == '--printBcids':
        printBcids = True

if (run == -1) : 
    usage()
    sys.exit(2)


# get database service and open database
dbSvc=cool.DatabaseSvcFactory.databaseService()
dbstring="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=COMP200;user=ATLAS_COOL_READER;password=COOLRED4PRO"
#dbstring="sqlite://;schema=/afs/cern.ch/user/b/berge/trash/ctp.LBData.1190647872.sqlitefile.db;dbname=L1CTCOOL"

try:
    db=dbSvc.openDatabase(dbstring,False)
except Exception,e:
    print e
    sys.exit(-1)

bgkey_fname='/TRIGGER/LVL1/BunchGroupKey'
bgcont_fname='/TRIGGER/LVL1/BunchGroupContent'
bgdesc_fname='/TRIGGER/LVL1/BunchGroupDescription'

if not db.existsFolder(bgkey_fname) :
    print "Folder",bgkey_fname,"not found"
    db.closeDatabase()
    sys.exit(-1)
if not db.existsFolder(bgcont_fname) :
    print "Folder",bgcont_fname,"not found"
    db.closeDatabase()
    sys.exit(-1)
if not db.existsFolder(bgdesc_fname) :
    print "Folder",bgdesc_fname,"not found"
    db.closeDatabase()
    sys.exit(-1)

bgkeys = []
folder=db.getFolder(bgkey_fname)
try:
    itr=folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())
    while itr.goToNext() :
        obj=itr.currentRef()
        payload=obj.payload()
        bgkeys.append(payload['Lvl1BunchGroupConfigurationKey'])
    itr.close()
except Exception,e:
    print "Reading data from",bgkey_fname,"failed:",e

print "Run",run,"bg keys",bgkeys

bgNames = []
bgMap = []
folder=db.getFolder(bgdesc_fname)
try:
    itr=folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())
    while itr.goToNext() :
        obj=itr.currentRef()
        payload=obj.payload()
        bgNames.append(str(payload['BunchGroup0']))
        bgNames.append(str(payload['BunchGroup1']))
        bgNames.append(str(payload['BunchGroup2']))
        bgNames.append(str(payload['BunchGroup3']))
        bgNames.append(str(payload['BunchGroup4']))
        bgNames.append(str(payload['BunchGroup5']))
        bgNames.append(str(payload['BunchGroup6']))
        bgNames.append(str(payload['BunchGroup7']))
        bgMap = payload['ItemToBunchGroupMap']

    itr.close()
except Exception,e:
    print "Reading data from",bgkey_fname,"failed:",e

for n in range(len(bgNames)):
    print "BG",n,":",bgNames[n]
for ii in range(len(bgMap)):
    bitCode = ''
    for bit in range(7,-1,-1):
        if (bgMap[ii]>>bit & 1) : bitCode += '1'
        else : bitCode += '0'
    print "Item",ii,"BG Code",bitCode

folder=db.getFolder(bgcont_fname)
try:
    index = 0
    itr=folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())
    while itr.goToNext() :
        bgList = []
        for _ in range(8): bgList.append([])
        obj=itr.currentRef()
        payload=obj.payload()
        bgcont = payload['BunchCode']
        for bcid in range(3564): 
            for bgrp in range(8):
                if(bgcont[bcid]>>bgrp & 1): bgList[bgrp].append(bcid)
        print "BG key",bgkeys[index]
        ++index
        for grp in range(8):
            print "BG",grp,"size :",len(bgList[grp])
        for grp in range(8):
            print "BG",grp,":",bgList[grp]

    itr.close()
except Exception,e:
    print "Reading data from",bgcont_fname,"failed:",e
