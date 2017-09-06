#!/usr/bin/env python
### import the modules 
import ROOT

### define the list of input files
import sys


if len (sys.argv) == 1:
    print 'checkTriggerEDM.py poolfile [poolfile2]'
    sys.exit(1)
    
# all the rest besides the 0th element is assumed to be file names
PoolAODInput=sys.argv[1:]

print PoolAODInput


### build the transient event and metadata trees
print '... build collection and metadata trees'
from TrigDecisionTool.BuildTransientTrees import BuildTransientTrees,filtBasic
(transientTree, transientMetaDataTree) = BuildTransientTrees(PoolAODInput,filtBasic)


### instantiate the TrigDecisionToolARA
print '... instantiate TrigDecisionToolARA'
tdt = ROOT.Trig.TrigDecisionToolARA(transientTree, transientMetaDataTree)
### changing the output level of the TrigDecisionTool (default is ERROR)
#import AthenaCommon.Constants as Constants
#tdt.setOutputLevel(Constants.WARNING)


### create two triggers


allChains = tdt.getChainGroup('(L2|EF).*')


# error counters 
l2errors = {}
eferrors = {}
chainerrors = {}


### loop over the events
nevt = transientTree.GetEntries()
print '... loop over %i events' % nevt
for evt in xrange(nevt):
    transientTree.GetEvent(evt)

    l2res = transientTree.HLTResult_L2
    efres = transientTree.HLTResult_EF
    iserror=False
    if l2res.getHLTStatus().code != 0:
        iserror = True
        if not l2errors.has_key(l2res.getHLTStatus().str()):
            l2errors[l2res.getHLTStatus().str()] = 0
        l2errors[l2res.getHLTStatus().str()] += 1

    if efres.getHLTStatus().code != 0:
        iserror = True
        if not eferrors.has_key(efres.getHLTStatus().str()):
            eferrors[efres.getHLTStatus().str()] = 0
        eferrors[efres.getHLTStatus().str()] += 1
    if iserror:
        for c in allChains.getListOfTriggers():
            cg = tdt.getChainGroup(c)
            ec = cg.error()
            if ec.code != 0:
                key = c+":"+ec.str()
                if not chainerrors.has_key(key):
                    chainerrors[key] = 0
                chainerrors[key] += 1
            
        

        
print "Events     processed: ", nevt
print "L2          problems: ", l2errors
print "EF          problems: ", eferrors
print "Chains      problems: ", chainerrors
print ""


