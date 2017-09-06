#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import re

def printElectronContainer(elContFeature,flag):
    if not elContFeature: return
    ec = elContFeature.cptr()
    print '... Electron container with label %s is of size %i' % (elContFeature.label(), ec.size())
    for j,el in enumerate(ec):
        print '    --- electron %i with eta: %s phi: %s et: %s' % (j, el.eta(), el.phi(), el.et())
    printFlag(flag)



def printElectron(elFeature,flag):
    if not elFeature: return
    el = elFeature.cptr()
    print '... Electron with label %s with eta: %s phi: %s et: %s' % (elFeature.label(), el.eta(), el.phi(), el.et())
    printFlag(flag)

        

def printFlag(flag,asList=True):
    return 
    from TrigEventARA.TrigPassFlags import TrigPassFlagsARA
    flag = TrigPassFlagsARA(flag)
    print '... Flag size: %i' % (flag.size())
    for i in range(flag.size()):
        print '        %i: isEM = %i' % (i, flag.FlagsAsInt(i))

### import the modules 
print '... importing ROOT'
import ROOT

print '... define list of files'
# location
# eosatlas.cern.ch/eos/atlas/user/s/stelzer/ESD_withEGammaPassFlags.pool.root
#PoolAODInput=['./ESD_withEGammaPassFlags.pool.root']
PoolAODInput=['./ESD.pool.root']
for f in PoolAODInput: print '    + %s' % f


### build the transient event and metadata trees
print '... build collection and metadata trees'
from TrigDecisionTool.BuildTransientTrees import BuildTransientTrees
(transientTree, transientMetaDataTree) = BuildTransientTrees(PoolAODInput)


### instantiate the TrigDecisionToolARA
print '... instantiate TrigDecisionToolARA'
tdt = ROOT.Trig.TrigDecisionToolARA(transientTree, transientMetaDataTree)
### changing the output level of the TrigDecisionTool (default is ERROR)
#import AthenaCommon.Constants as Constants
#tdt.setOutputLevel(Constants.DEBUG)


### create trigger chain group
electronChain = tdt.getChainGroup('EF_e22_loose')

### loop over the events
nevt = transientTree.GetEntries()
print '... loop over %i events' % nevt


tdt.ExperimentalAndExpertMethods().enable()


for evt in xrange(nevt):
#for evt in [6,7]:

    print "\nreading event",evt, "=============================="
    transientTree.GetEntry(evt)

    if evt==0:
        print "... electron chains:", [x for x in electronChain.getListOfTriggers()]
        # getting some configuration details
        cfgCh = tdt.ExperimentalAndExpertMethods().getChainConfigurationDetails('EF_e22_loose')
        print "%s (%i)" % (cfgCh.chain_name(), cfgCh.chain_counter())


    testSingleAccessPhysics = True
    testSingleAccessDeactiv = True
    testContainerAccessPhysics = True
    testContainerAccessDeactiv = True

    for ch in [x for x in electronChain.getListOfTriggers()]:

        print "Chain:",ch,'... passed:', tdt.isPassed(ch)
        features = tdt.features(ch,ROOT.TrigDefs.alsoDeactivateTEs)

        print "... Now getting the electrons and pass flags\n"

        if testSingleAccessPhysics:
            print "Single object access (Physics)"
            print "------------------------------"
            eg = features.get('egamma')('egamma_Electrons', ROOT.TrigDefs.Physics) # list of Feature<egamma>
            for e in eg:
                printElectron(e,features.getFlags(e,'isEM'))


        if testSingleAccessDeactiv:
            print "Single object access (alsoDeactivateTEs)"
            print "----------------------------------------"
            eg = features.get('egamma')('egamma_Electrons', ROOT.TrigDefs.alsoDeactivateTEs) # list of Feature<egamma>
            for ec in eg:
                printElectron(ec,features.getFlags(ec,'isEM'))


        if testContainerAccessPhysics:
            print "Container access (Physics)"
            print "--------------------------"
            egC = features.get('egammaContainer')('egamma_Electrons', ROOT.TrigDefs.Physics) # list of Feature<egammaContainer>
            for ec in egC:
                printElectronContainer(ec,features.getFlags(ec,'isEM'))


        if testContainerAccessDeactiv:
            print "Container access (alsoDeactivateTEs)"
            print "------------------------------------"
            egC = features.get('egammaContainer')('egamma_Electrons', ROOT.TrigDefs.alsoDeactivateTEs) # list of Feature<egammaContainer>
            for ec in egC:
                printElectronContainer(ec,features.getFlags(ec,'isEM'))
