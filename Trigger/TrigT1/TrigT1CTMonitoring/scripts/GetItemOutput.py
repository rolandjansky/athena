#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, sys, getopt, math
import CoolHelper

def usage():
    print "%s" % sys.argv[0]
    print "  --help                                this printout"
    print "  --itemList i1,i2                      list of items"
    print "  --grl <grl>                           GRL to be used" 
    print "  --verbose                             print run/LB with non-zero item count" 

try:
    longopts=['grl=','help','itemList=', 'verbose']
    opts,args=getopt.getopt(sys.argv[1:],'',longopts)
except getopt.GetoptError,e:
    usage()
    sys.exit(2)

grlFile=''
itemList=[]
verbose=False

for o,a in opts:
    if o in ('-h','--help'):
        usage()
        sys.exit(0)
    if o == '--itemList':
        ilist = str(a).split(',')
        itemList = [i for i in ilist if i]
    if o == '--grl':
        grlFile = str(a)
    if o == '--verbose':
        verbose = True

if (not itemList or not grlFile) : 
    usage()
    sys.exit(2)

import ROOT
ROOT.gSystem.Load('libGoodRunsListsLib')
from ROOT import Root

itemCounts = {}
## read the goodrunslist xml file(s)
reader = Root.TGoodRunsListReader(grlFile)
#reader.AddXMLFile('LumiRangeCollectionB.xml')
reader.Interpret()
grl = reader.GetMergedGoodRunsList()
goodruns = grl.GetGoodRuns()
for rr in goodruns:
    #rr.Summary()
    runNum = int(rr.GetRunNumber())
    TAVs = CoolHelper.readLvl1Counters(runNum, itemList, "TAV")
    for name in itemList:    
        lbDictRefTAV = TAVs[name]
        itemCountsPerRun = 0
        for testLb in lbDictRefTAV.keys():
            if not grl.HasRunLumiBlock(runNum,testLb): continue
            try:
                itemCounts[name] += lbDictRefTAV[testLb]
                itemCountsPerRun += lbDictRefTAV[testLb]
            except KeyError:
                itemCounts[name] = int(0)
                itemCounts[name] += lbDictRefTAV[testLb]
                itemCountsPerRun += lbDictRefTAV[testLb]

        if verbose and itemCountsPerRun:
            print "Run",runNum,"triggers",name,itemCountsPerRun
                
for name in itemList:
    print "Item",name,"triggers",itemCounts[name]
