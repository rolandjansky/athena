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
# SignatureDicts addition
##################################################################
from TriggerMenuMT.HLTMenuConfig.Menu.SignatureDicts import AllowedEventBuildingIdentifiers
AllowedEventBuildingIdentifiers.extend(['pebtestfour','pebtestfive'])

##################################################################
# egamma chains
##################################################################
if (doElectron):
    from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import fastCaloMenuSequence
    from TriggerMenuMT.HLTMenuConfig.Egamma.ElectronSequenceSetup import electronMenuSequence

    fastCaloStep= fastCaloMenuSequence("Ele")
    electronStep= electronMenuSequence()

    step1=ChainStep("Step1_etcut", [fastCaloStep])
    step2=ChainStep("Step2_etcut", [electronStep])
    step3_pebtestone=ChainStep("Step3_pebtestone", [pebInfoWriterSequence("pebtestone")])
    step3_pebtestfour=ChainStep("Step3_pebtestfour", [pebInfoWriterSequence("pebtestfour")])

    egammaChains = [
        Chain(name='HLT_e3_etcut_pebtestone',  Seed="L1_EM3",  ChainSteps=[step1, step2, step3_pebtestone]  ),
        Chain(name='HLT_e5_etcut_pebtestone',  Seed="L1_EM3",  ChainSteps=[step1, step2, step3_pebtestone]  ),
        Chain(name='HLT_e5_etcut_pebtestfour',  Seed="L1_EM3",  ChainSteps=[step1, step2, step3_pebtestfour]  ),
        Chain(name='HLT_e7_etcut',             Seed="L1_EM3",  ChainSteps=[step1, step2]  )
    ]
    testChains += egammaChains

##################################################################
# muon chains
##################################################################
if (doMuon):
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSequenceSetup import muFastSequence
    muFastSequence = muFastSequence()

    step1mufast=ChainStep("Step1_muFast", [ muFastSequence ])
    step2_pebtesttwo=ChainStep("Step2_pebtesttwo", [pebInfoWriterSequence("pebtesttwo")])
    step2_pebtestfive=ChainStep("Step2_pebtestfive", [pebInfoWriterSequence("pebtestfive")])

    muonChains = [
        Chain(name='HLT_mu6_pebtesttwo',  Seed="L1_MU6",  ChainSteps=[step1mufast, step2_pebtesttwo]  ),
        Chain(name='HLT_mu6_pebtestfive',  Seed="L1_MU6",  ChainSteps=[step1mufast, step2_pebtestfive]  ),
        Chain(name='HLT_2mu6',        Seed="L1_MU6",  ChainSteps=[step1mufast]  )
    ]
    testChains += muonChains

#################################
# Configure L1Decoder
#################################

# this is a temporary hack to include new test chains
EnabledChainNamesToCTP = dict([ (c.name, c.seed)  for c in testChains])
topSequence.L1Decoder.ChainToCTPMapping = EnabledChainNamesToCTP

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
l1decoder = getSequence("L1Decoder")
hltAllSteps = getSequence("HLTAllSteps")
from TriggerJobOpts.TriggerConfig import collectHypos,collectFilters,collectViewMakers,collectDecisionObjects,triggerMergeViewsAndAddMissingEDMCfg
hypos = collectHypos(hltAllSteps)
filters = collectFilters(hltAllSteps)
viewMakers = collectViewMakers(hltAllSteps)
decisionObjects = collectDecisionObjects(hypos,filters,l1decoder)
edmMakerAlg = triggerMergeViewsAndAddMissingEDMCfg( [], hypos, viewMakers, decisionObjects )
topSequence.HLTTop += edmMakerAlg

# Add Electrons merger (somehow not created by triggerAddMissingEDMCfg above)
from TrigOutputHandling.TrigOutputHandlingConf import HLTEDMCreator
electronsMerger = HLTEDMCreator("electronsMerger")
electronsMerger.TrigElectronContainerViews = [ "EMElectronViews" ]
electronsMerger.TrigElectronContainerInViews = [ "HLT_L2Electrons" ]
electronsMerger.TrigElectronContainer = [ "HLT_L2Electrons" ]
edmMakerAlg.OutputTools += [ electronsMerger ]

# Make a list of HLT decision objects to be added to the ByteStream output
decisionObjectsToRecord = []
for d in decisionObjects:
    decisionObjectsToRecord.extend([
        "xAOD::TrigCompositeContainer_v1#%s" % d,
        "xAOD::TrigCompositeAuxContainer_v2#%s." % d
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
    "xAOD::TrigEMClusterContainer_v1#HLT_L2CaloEMClusters",
    "xAOD::TrigEMClusterAuxContainer_v2#HLT_L2CaloEMClustersAux.RoIword.clusterQuality.e233.e237.e277.e2tsts1.ehad1.emaxs1.energy.energySample.et.eta.eta1.fracs1.nCells.phi.rawEnergy.rawEnergySample.rawEt.rawEta.rawPhi.viewIndex.weta2.wstot",
    "xAOD::TrigElectronContainer_v1#HLT_L2Electrons",
    "xAOD::TrigElectronAuxContainer_v1#HLT_L2ElectronsAux.pt.eta.phi.rawEnergy.rawEt.rawEta.nCells.energy.et.e237.e277.fracs1.weta2.ehad1.wstot",
])

##### Result maker part 2 - stream tags #####

streamPhysicsMain = ['Main', 'physics', "True", "True"]
streamPhysicsPebtestone = ['pebtestone', 'physics', "True", "False"]
streamCalibPebtesttwo = ['pebtesttwo', 'calibration', "True", "False"]
streamPhysicsPebtestfour = ['pebtestfour', 'physics', "True", "False"]
streamPhysicsPebtestfive = ['pebtestfive', 'physics', "True", "False"]

stmaker = StreamTagMakerTool()
stmaker.OutputLevel = DEBUG
stmaker.ChainDecisions = "HLTSummary"
stmaker.PEBDecisionKeys = pebDecisionKeys
stmaker.ChainToStream = dict( [(c.name, streamPhysicsMain) for c in testChains ] )
stmaker.ChainToStream["HLT_e3_etcut_pebtestone"] = streamPhysicsPebtestone
stmaker.ChainToStream["HLT_e5_etcut_pebtestone"] = streamPhysicsPebtestone
stmaker.ChainToStream["HLT_e5_etcut_pebtestfour"] = streamPhysicsPebtestfour
stmaker.ChainToStream["HLT_mu6_pebtesttwo"] = streamCalibPebtesttwo
stmaker.ChainToStream["HLT_mu6_pebtestfive"] = streamPhysicsPebtestfive

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
topSequence.HLTTop += hltResultMakerAlg

##########################################
# Some debug
##########################################
from AthenaCommon.AlgSequence import dumpSequence
dumpSequence(topSequence)
