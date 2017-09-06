#!/usr/bin/env python
### import the modules 

import sys

if len (sys.argv) == 1:
    print 'checkTriggerEDM.py --chains=\'PATTERN\' --nev=10 poolfile [poolfile2] '
    sys.exit(1)
    
# all the rest besides the 0th element is assumed to be file names
pattern='(EF|L2)_.*'
PoolAODInput=[]
nevt=-1

for arg in sys.argv[1:]:
    if '--chains' in arg:
        pattern = arg.split("=")[1]
        print "selected chains in pattern: ", pattern 
    if '--nev' in arg:
        nevt = int(arg.split("=")[1])
        print "number of events: ", nevt 
    else:
        PoolAODInput.append(arg)

print PoolAODInput


import ROOT
### build the transient event and metadata trees
#print '... build collection and metadata trees'
from TrigDecisionTool.BuildTransientTrees import BuildTransientTrees, filtEDMAccess
(transientTree, transientMetaDataTree) = BuildTransientTrees(PoolAODInput, filtEDMAccess)


### instantiate the TrigDecisionToolARA
#print '... instantiate TrigDecisionToolARA'
tdt = ROOT.Trig.TrigDecisionToolARA(transientTree, transientMetaDataTree)
### changing the output level of the TrigDecisionTool (default is ERROR)
#import AthenaCommon.Constants as Constants
#tdt.setOutputLevel(Constants.WARNING)


### create two triggers

#allL2 = tdt.getChainGroup('L2_.*')
#allEF = tdt.getChainGroup('EF_.*')
all = tdt.getChainGroup(pattern)


### possible HLT objects
from pprint import pprint as pp
from TriggerJobOpts.TriggerEDM  import EDMDetails
collection2type = {}
for t, details in EDMDetails.iteritems():
    try:
        collection2type[details['collection']] = t
    except:
        pass
# pp(collection2type)
branches = []
for b in list(transientTree.GetListOfBranches()):
    if 'HLT' == b.GetName() or 'HLT_' in b.GetName():
        try:
            branches.append( (b.getPersBranchName(), b.GetObjClassName()) )            
        except:
            print "failing to access info about branch: ", b.GetName()



type2key = []
for i in branches:
     key =  i[0].split('HLT')[1].lstrip('_')
     collection = i[1]
     _type = collection
     if collection2type.has_key(collection):
         _type = collection2type[collection]
         
     type2key.append( (_type, key, {}) )

print sorted([x[0] for x in type2key]),"\n\n\n"

# huge loop now
if nevt<0:
    nevt = transientTree.GetEntries()

for evt in xrange(nevt):
    #print "... reading event",evt,"/",nevt
    transientTree.GetEntry(evt)
    for chain in all.getListOfTriggers():
        features = tdt.features(chain)
        for (_type, key, counts) in type2key:
            try:
                back = features.get(_type)( key if key else '!' ) # do here ! trick)
                if back.size() == 0:
                    continue                
                if not counts.has_key(chain):
                    counts[chain] = 0
                counts[chain] = counts[chain] + back.size()
            except:
                if not counts.has_key(chain):
                    counts[chain] = -1
                counts[chain] = -1
                    
chain2types = {}
chain2types.update( [ (c, []) for c in list(all.getListOfTriggers())] )
for t in type2key:
    for c in t[2].keys():
        chain2types[c].append(t[0]+'#'+t[1])


#final printout
    

sortedchains = sorted(chain2types.keys())

print 'Chains to types mapping ' + '-'*70
for c in sortedchains:
    t = chain2types[c]
    print 'chain: %-30s  type#key: %s' % (c, str(t))

print 'Types to chains mapping ' + '-'*70
type2key.sort()
for i in type2key:    
    print 'type#key: %-60s chains: %s' % (i[0]+'#'+i[1], str(i[2]))


print '-'*90
print 'HELP: TrigRoiDescriptor# - means that no key was used for recording'
print 'HELP: negiative numbers signify problems with the access to given type'
print '-'*90
