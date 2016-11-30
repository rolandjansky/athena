#!/usr/bin/env python
# copy of TrigAnalysisExamples/CompareESDs.py by Tomasz Bold
# 

### import the modules 
from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

print '... importing ROOT and PyCintex'
import ROOT, PyCintex
import AthenaROOTAccess.transientTree

### define the list of input files
print '... define list of files'

import sys
if '-c' in sys.argv:
    opt = sys.argv[sys.argv.index('-c')+1]
    exec(opt)


if not ('PoolAODInput1' in dir()):
    PoolAODInput1=["/afs/cern.ch/user/t/tbold/public/TDTtest/menu1/AOD.pool.root"]

if not ('PoolAODInput2' in dir()):
    PoolAODInput2=["/afs/cern.ch/user/t/tbold/public/TDTtest/menu2/AOD.pool.root"]

    
### build the transient event and metadata trees
print '... build collection and metadata trees'
#from TrigDecisionTool.BuildTransientTrees import BuildTransientTrees
#(transientTree1, transientMetaDataTree1) = BuildTransientTrees(files1)

CollectionTree1 = ROOT.AthenaROOTAccess.TChainROOTAccess('CollectionTree')
MetaDataTree1   = ROOT.AthenaROOTAccess.TChainROOTAccess('MetaData')

for file1 in PoolAODInput1:
    CollectionTree1.Add(file1+'/CollectionTree')
    MetaDataTree1.Add(file1+'/MetaData')
    
transientCollectionTree1 = AthenaROOTAccess.transientTree.makeTree(CollectionTree1)
transientMetaDataTree1   = AthenaROOTAccess.transientTree.makeTree(MetaDataTree1,
                                                                   persTreeName = 'MetaData',
                                                                  dhTreeName = 'MetaDataHdrDataHeader')




CollectionTree2 = ROOT.AthenaROOTAccess.TChainROOTAccess('CollectionTree')
MetaDataTree2   = ROOT.AthenaROOTAccess.TChainROOTAccess('MetaData')


for file2 in PoolAODInput2:
    CollectionTree2.Add(file2+'/CollectionTree')
    MetaDataTree2.Add(file2+'/MetaData')
    
transientCollectionTree2 = AthenaROOTAccess.transientTree.makeTree(CollectionTree2)
transientMetaDataTree2   = AthenaROOTAccess.transientTree.makeTree(MetaDataTree2,
                                                                   persTreeName = 'MetaData',
                                                                  dhTreeName = 'MetaDataHdrDataHeader')


### instantiate the TrigDecisionToolARA
print '... instantiate TrigDecisionToolARA'
tdt1 = ROOT.Trig.TrigDecisionToolARA(transientCollectionTree1, transientMetaDataTree1)
tdt2 = ROOT.Trig.TrigDecisionToolARA(transientCollectionTree2, transientMetaDataTree2)


### changing the output level of the TrigDecisionTool (default is ERROR)
#import AthenaCommon.Constants as Constants
#tdt.setOutputLevel(Constants.WARNING)


### create two triggers
chains1 = tdt1.getChainGroup('EF_.*')
chains2 = tdt2.getChainGroup('EF_.*')


### loop over the events
if transientCollectionTree1.GetEntries() != transientCollectionTree2.GetEntries():
    print '... WARNING both files have different number of events in, ',\
          transientCollectionTree1.GetEntries(), ' and ', transientCollectionTree2.GetEntries()


nevt = min(transientCollectionTree1.GetEntries(), transientCollectionTree2.GetEntries() )
print '... loop over %i events' % nevt


for evt in xrange(nevt):
    eventOK=True
    print "... reading event",evt
    transientCollectionTree1.GetEntry(evt)
    transientCollectionTree2.GetEntry(evt)

    c1 = list(chains1.getListOfTriggers())
    c2 = list(chains2.getListOfTriggers())
    for a in c1:
        if a not in c2:
            print "REGTEST ... chain ", a ," present in file 1 while not in file 2"
            eventOK=False
        else:
            if tdt1.isPassedBits(a) != tdt2.isPassedBits(a):
                print "REGTEST .... chain ", a, " has different isPassedBits in file 1: ", tdt1.isPassedBits(a) ," while in file 2: ", tdt2.isPassedBits(a) 
                eventOK=False
    if eventOK:
        print "INFO event OK"
