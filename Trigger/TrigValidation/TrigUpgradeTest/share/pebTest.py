#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

# This file is based on fullMenu.py

# Import flags
include("TrigUpgradeTest/testHLT_MT.py")
doElectron = True
doPhoton = False
doMuon   = True
doJet    = False
doCombo  = False

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep, MenuSequence
testChains = []

##################################################################
# PEB Info Writer step
##################################################################
from TrigUpgradeTest.pebMenuDefs import pebInfoWriterSequence


##################################################################
# egamma chains
##################################################################
if (doElectron):
    from TrigUpgradeTest.CaloMenuDefs import fastCaloMenuSequence
    from TrigUpgradeTest.electronMenuDefs import electronMenuSequence,  inDetSetup

    inDetSetup()
    fastCaloStep= fastCaloMenuSequence("Ele")
    electronStep= electronMenuSequence()

    step1=ChainStep("Step1_etcut", [fastCaloStep])
    step2=ChainStep("Step2_etcut", [electronStep])
    step3=ChainStep("Step3_pebtestone", [pebInfoWriterSequence("pebtestone")])

    egammaChains = [
        Chain(name='HLT_e3_etcut_pebtestone',  Seed="L1_EM3",  ChainSteps=[step1, step2, step3]  ),
        Chain(name='HLT_e5_etcut_pebtestone',  Seed="L1_EM3",  ChainSteps=[step1, step2, step3]  ),
        Chain(name='HLT_e7_etcut',             Seed="L1_EM3",  ChainSteps=[step1, step2]  )
    ]
    testChains += egammaChains

##################################################################
# muon chains
##################################################################
if (doMuon):
    from TrigUpgradeTest.muMenuDefs import muFastStep
    muFastStep1 = muFastStep()

    step1mufast=ChainStep("Step1_mufast", [ muFastStep1 ])
    step2peb=ChainStep("Step2_pebtesttwo", [pebInfoWriterSequence("pebtesttwo")])

    muonChains = [
        Chain(name='HLT_mu6_pebtesttwo',  Seed="L1_MU6",  ChainSteps=[step1mufast, step2peb]  ),
        Chain(name='HLT_2mu6',        Seed="L1_MU6",  ChainSteps=[step1mufast]  )
    ]
    testChains += muonChains

#################################
# Configure L1Decoder
#################################

# this is a temporary hack to include new test chains
EnabledChainNamesToCTP = dict([ (c.name, c.seed)  for c in testChains])
topSequence.L1DecoderTest.ChainToCTPMapping = EnabledChainNamesToCTP


##########################################
# CF construction
##########################################

##### Make all HLT #######
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import makeHLTTree
makeHLTTree(testChains)

# Helper method for getting a sequence for bootstrapping the output configuration
import AthenaCommon.AlgSequence as acas
def getSequence(name):
    '''Returns the first sequence under topSequence which matches the name'''
    return [s for s in acas.iter_algseq(topSequence) if s.getName() == name][0]

##########################################
# Map decisions producing PEBInfo from DecisionSummaryMakerAlg.FinalStepDecisions to StreamTagMakerTool.PEBDecisionKeys
##########################################
import AthenaCommon.AlgSequence as acas
summaryMakerAlg = [s for s in acas.iter_algseq(topSequence) if s.getName() == "DecisionSummaryMakerAlg"][0]
chainToDecisionKeyDict = summaryMakerAlg.getProperties()['FinalStepDecisions']

pebDecisionKeys = []
for chain, decisionKey in chainToDecisionKeyDict.iteritems():
    if 'PEBInfoWriterAlg' in decisionKey:
        print "Chain ", chain, " produces decision ", decisionKey, " with PEBInfo"
        pebDecisionKeys.append(decisionKey)

##########################################
# EDM Maker
##########################################
l1decoder = getSequence("L1DecoderTest")
hltAllSteps = getSequence("HLTAllSteps")
from TriggerJobOpts.TriggerConfig import collectHypos,collectFilters,collectDecisionObjects,triggerAddMissingEDMCfg
hypos = collectHypos(hltAllSteps)
filters = collectFilters(hltAllSteps)
decisionObjects = collectDecisionObjects(hypos,filters,l1decoder)
edmMakerAlg = triggerAddMissingEDMCfg([],decisionObjects)
topSequence.hltTop += edmMakerAlg

# Add Electrons merger (somehow not created by triggerAddMissingEDMCfg above)
from TrigOutputHandling.TrigOutputHandlingConf import HLTEDMCreator
electronsMerger = HLTEDMCreator("electronsMerger")
electronsMerger.TrigElectronContainerViews = [ "EMElectronViews" ]
electronsMerger.TrigElectronContainerInViews = [ "Electrons" ]
electronsMerger.TrigElectronContainer = [ "Electrons" ]
edmMakerAlg.OutputTools += [ electronsMerger ]

# Make a list of HLT decision objects to be added to the ByteStream output
decisionObjectsToRecord = []
for d in decisionObjects:
    decisionObjectsToRecord.extend([
        "xAOD::TrigCompositeContainer_v1#%s" % d,
        "xAOD::TrigCompositeAuxContainer_v2#%s.decisions" % d
    ])

##########################################
# HLT Result maker
##########################################
from TrigOutputHandling.TrigOutputHandlingConf import HLTResultMTMakerAlg, StreamTagMakerTool, TriggerBitsMakerTool
from TrigOutputHandling.TrigOutputHandlingConfig import TriggerEDMSerialiserToolCfg, HLTResultMTMakerCfg

##### Result maker part 1 - serialiser #####

serialiser = TriggerEDMSerialiserToolCfg("Serialiser")
serialiser.OutputLevel=VERBOSE

# Serialise HLT decision objects
serialiser.addCollectionListToMainResult(decisionObjectsToRecord)

# Serialise L2 calo clusters and electrons
serialiser.addCollectionListToMainResult([
    "xAOD::TrigEMClusterContainer_v1#L2CaloEMClusters",
    "xAOD::TrigEMClusterAuxContainer_v2#L2CaloEMClustersAux.RoIword.clusterQuality.e233.e237.e277.e2tsts1.ehad1.emaxs1.energy.energySample.et.eta.eta1.fracs1.nCells.phi.rawEnergy.rawEnergySample.rawEt.rawEta.rawPhi.viewIndex.weta2.wstot",
    "xAOD::TrigElectronContainer_v1#Electrons",
    "xAOD::TrigElectronAuxContainer_v1#ElectronsAux.pt.eta.phi.rawEnergy.rawEt.rawEta.nCells.energy.et.e237.e277.fracs1.weta2.ehad1.wstot",
])

##### Result maker part 2 - stream tags #####

streamPhysicsMain = ['Main', 'physics', "True", "True"]
streamPhysicsPebtestone = ['pebtestone', 'physics', "True", "False"]
streamCalibPebtesttwo = ['pebtesttwo', 'calibration', "True", "False"]

stmaker = StreamTagMakerTool()
stmaker.OutputLevel = DEBUG
stmaker.ChainDecisions = "HLTSummary"
stmaker.PEBDecisionKeys = pebDecisionKeys
stmaker.ChainToStream = dict( [(c.name, streamPhysicsMain) for c in testChains ] )
stmaker.ChainToStream["HLT_e3_etcut_pebtestone"] = streamPhysicsPebtestone
stmaker.ChainToStream["HLT_e5_etcut_pebtestone"] = streamPhysicsPebtestone
stmaker.ChainToStream["HLT_mu6_pebtesttwo"] = streamCalibPebtesttwo

##### Result maker part 3 - HLT bits #####

bitsmaker = TriggerBitsMakerTool()
bitsmaker.ChainDecisions = "HLTSummary"
bitsmaker.ChainToBit = dict( [ (chain.name, 10*num) for num,chain in enumerate(testChains) ] )
bitsmaker.OutputLevel = DEBUG

##### Result maker part 4 - configure all together #####

hltResultMakerTool =  HLTResultMTMakerCfg()
hltResultMakerTool.MakerTools = [ stmaker, bitsmaker, serialiser ]
hltResultMakerTool.OutputLevel = DEBUG

hltResultMakerAlg =  HLTResultMTMakerAlg("HLTRMakerAlg")

hltResultMakerAlg.ResultMaker = hltResultMakerTool
topSequence.hltTop += hltResultMakerAlg

##########################################
# Some debug
##########################################
from AthenaCommon.AlgSequence import dumpSequence
dumpSequence(topSequence)
