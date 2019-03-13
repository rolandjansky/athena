#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

# This file is based on pebTest.py

# Import flags
include("TrigUpgradeTest/testHLT_MT.py")
doElectron = True
doPhoton = False
doMuon   = False
doJet    = False
doCombo  = False

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep
testChains = []

##################################################################
# PEB Info writer step for data scouting
##################################################################
from TrigUpgradeTest.pebMenuDefs import pebInfoWriterSequence,dataScoutingSequence

##################################################################
# SignatureDicts addition
##################################################################
from TriggerMenuMT.HLTMenuConfig.Menu.SignatureDicts import AllowedEventBuildingIdentifiers
AllowedEventBuildingIdentifiers.extend(['dataScoutingElectronTest','pebtestthree'])

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
    step3_PEB1=ChainStep("Step3_pebtestone", [pebInfoWriterSequence("pebtestone")]) # adds some LAr ROBs and the full HLT result
    step3_DS=ChainStep("Step3_dataScoutingElectronTest", [dataScoutingSequence("dataScoutingElectronTest")]) # adds the special HLT result
    step4_PEB3=ChainStep("Step3_pebtestthree", [pebInfoWriterSequence("pebtestthree")]) # same as pebtestone but without any HLT result

    egammaChains = [
        # DS+PEB chain (special HLT result and subset of detector data saved)
        Chain(name='HLT_e3_etcut_dataScoutingElectronTest_pebtestthree',  Seed="L1_EM3",  ChainSteps=[step1, step2, step3_DS, step4_PEB3]  ),

        # Pure DS chain (only special HLT result saved and no detector data saved)
        Chain(name='HLT_e5_etcut_dataScoutingElectronTest',  Seed="L1_EM3",  ChainSteps=[step1, step2, step3_DS]  ),

        # PEB chain (full HLT result and subset of detector data saved)
        Chain(name='HLT_e7_etcut_pebtestone',  Seed="L1_EM3",  ChainSteps=[step1, step2, step3_PEB1]  ),

        # Standard chain (full HLT result and full detector data saved)
        Chain(name='HLT_e12_etcut',  Seed="L1_EM3",  ChainSteps=[step1, step2]  )
    ]
    testChains += egammaChains

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
summaryMakerAlg = getSequence("DecisionSummaryMakerAlg")
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

# Serialise HLT decision objects (in full result)
serialiser.addCollectionListToMainResult(decisionObjectsToRecord)

# Serialise L2 calo clusters (in full result)
serialiser.addCollectionListToMainResult([
    "xAOD::TrigEMClusterContainer_v1#L2CaloEMClusters",
    "xAOD::TrigEMClusterAuxContainer_v2#L2CaloEMClustersAux.RoIword.clusterQuality.e233.e237.e277.e2tsts1.ehad1.emaxs1.energy.energySample.et.eta.eta1.fracs1.nCells.phi.rawEnergy.rawEnergySample.rawEt.rawEta.rawPhi.viewIndex.weta2.wstot",
])

# This is the Data Scouting part! Let's add L2 electrons to the main result AND to the "electron DS" result
from TrigUpgradeTest.pebMenuDefs import dataScoutingResultIDFromName
electronDSModuleIDs = [serialiser.fullResultID(), dataScoutingResultIDFromName('dataScoutingElectronTest')] # 0 is main (full) result; we get the other ID from the EDM configuration
serialiser.addCollectionListToResults([
    "xAOD::TrigElectronContainer_v1#Electrons",
    "xAOD::TrigElectronAuxContainer_v1#ElectronsAux.pt.eta.phi.rawEnergy.rawEt.rawEta.nCells.energy.et.e237.e277.fracs1.weta2.ehad1.wstot",
], electronDSModuleIDs)

##### Result maker part 2 - stream tags #####

streamPhysicsMain = ['Main', 'physics', "True", "True"]
streamPhysicsPebtestone = ['pebtestone', 'physics', "True", "False"]
streamDataScoutingElectron = ['dataScoutingElectronTest', 'physics', "True", "False"]
streamDSPEBElectron = ['DSElectronWithPEB', 'physics', "True", "False"]

stmaker = StreamTagMakerTool()
stmaker.OutputLevel = DEBUG
stmaker.ChainDecisions = "HLTSummary"
stmaker.PEBDecisionKeys = pebDecisionKeys
stmaker.ChainToStream = dict( [(c.name, streamPhysicsMain) for c in testChains ] )
stmaker.ChainToStream["HLT_e3_etcut_dataScoutingElectronTest_pebtestthree"] = streamDSPEBElectron
stmaker.ChainToStream["HLT_e5_etcut_dataScoutingElectronTest"] = streamDataScoutingElectron
stmaker.ChainToStream["HLT_e7_etcut_pebtestone"] = streamPhysicsPebtestone
# The configuration above means:
# stream physics_Main is produced when the e12 chain passes and it includes full events
# stream physics_pebtestone is produced when the e7 chain passes and it includes the main HLT result and some detector ROBs
# stream physics_dataScoutingElectronTest is produced when the e5 chain passes and it includes only the special HLT DS result
# stream physics_DSElectronWithPEB is produced when the e3 chain passes and it includes the special HLT DS result and some detector ROBs

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
