#!/usr/bin/env python
### import the modules 
#print '... importing ROOT'
import ROOT

import sys
if '-c' in sys.argv:
    opt = sys.argv[sys.argv.index('-c')+1]
    exec(opt)
    
if 'pool.root' in sys.argv[-1]:
    exec('PoolAODInput=["'+sys.argv[-1]+'"]')

### define the list of input files
print '... define list of files'
if not ('PoolAODInput' in dir()):
    PoolAODInput=['/afs/cern.ch/user/t/tbold/public/TDTtest/menu1/AOD.pool.root']
for f in PoolAODInput:
    print '    + %s' % f


### build the transient event and metadata trees
#print '... build collection and metadata trees'
from TrigDecisionTool.BuildTransientTrees import BuildTransientTrees
(transientTree, transientMetaDataTree) = BuildTransientTrees(PoolAODInput)


### instantiate the TrigDecisionToolARA
#print '... instantiate TrigDecisionToolARA'
tdt = ROOT.Trig.TrigDecisionToolARA(transientTree, transientMetaDataTree)
### changing the output level of the TrigDecisionTool (default is ERROR)
#import AthenaCommon.Constants as Constants
#tdt.setOutputLevel(Constants.WARNING)


### create two triggers

#allL2 = tdt.getChainGroup('L2_.*')
#allEF = tdt.getChainGroup('EF_.*')
all = tdt.getChainGroup('(EF|L2)_.*')


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

branches = [(b.getPersBranchName(), b.GetObjClassName()) for b in list(transientTree.GetListOfBranches()) if 'HLT' == b.GetName() or 'HLT_' in b.GetName() ]


type2key = []
for i in branches:
     key =  i[0].split('HLT')[1].lstrip('_')
     collection = i[1]
     type = collection
     if collection2type.has_key(collection):
         type = collection2type[collection]
         
     type2key.append( (type, key, {}) )

# huge loop now
nevt = transientTree.GetEntries()
for evt in xrange(nevt):
    #print "... reading event",evt
    transientTree.GetEntry(evt)
    for chain in all.getListOfTriggers():
        features = tdt.features(chain)
        for i in range(len(type2key)):
            type = type2key[i][0]
            key  = type2key[i][1]
            back = features.get(type)(key)
            if back.size() == 0:
                continue
            counts = type2key[i][2]            
            if not counts.has_key(chain):
                counts[chain] = 0
            counts[chain] = counts[chain] + back.size()

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
print '-'*90
