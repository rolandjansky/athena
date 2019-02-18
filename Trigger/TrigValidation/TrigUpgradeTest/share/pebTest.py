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
from AthenaCommon.CFElements import seqAND, seqOR
emptySequence = seqAND("emptySequence")

from DecisionHandling.DecisionHandlingConf import InputMakerForRoI
def pebInputMaker(name):
    _pebInputMaker = InputMakerForRoI("pebInputMaker_"+name, mergeOutputs=False)
    _pebInputMaker.RoIs="pebInputRoI_"+name
    return _pebInputMaker

def pebSequence(inputMaker):
    return seqAND("pebSequence_"+inputMaker.name(), [inputMaker])


from TrigPartialEventBuilding.TrigPartialEventBuildingConf import PEBInfoWriterAlg,StaticPEBInfoWriterTool

def pebInfoWriterToolFromName(name, conf):
    if "pebtestone" in name:
        tool = StaticPEBInfoWriterTool(name)
        tool.ROBList = [0x42002e, 0x420060, 0x420064] # a few example LAr ROBs
        return tool
    if "pebtesttwo" in name:
        tool = StaticPEBInfoWriterTool(name)
        tool.SubDetList = [0x65, 0x66] # example: RPC side A and C
        return tool
    else:
        return None

def pebInfoWriterSequence(name):
    """Creates a MenuSequence for PEBInfo writer. The algorithm and tools are given unique names derived from
    the name parameter. This is required to avoid execution stall from having the same algorithm instance configured
    in different steps awaiting different inputs."""
    inputMaker = pebInputMaker(name)
    return MenuSequence(Sequence    = pebSequence(inputMaker),
                        Maker       = inputMaker,
                        Hypo        = PEBInfoWriterAlg("PEBInfoWriterAlg_"+name),
                        HypoToolGen = pebInfoWriterToolFromName)

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
# HLT Result maker
##########################################
from TrigOutputHandling.TrigOutputHandlingConf import HLTResultMTMakerAlg, HLTResultMTMaker, StreamTagMakerTool, TriggerBitsMakerTool, TriggerEDMSerialiserTool

serialiser = TriggerEDMSerialiserTool(name="Serialiser", OutputLevel=VERBOSE)
serialiser.CollectionsToSerialize = [ "xAOD::TrigCompositeContainer_v1#remap_EgammaCaloDecisions",
                                      "xAOD::TrigCompositeAuxContainer_v1#remap_EgammaCaloDecisionsAux.",
                                      "xAOD::TrigEMClusterContainer_v1#HLT_xAOD__TrigEMClusterContainer_L2CaloClusters",
                                      "xAOD::TrigEMClusterAuxContainer_v2#HLT_xAOD__TrigEMClusterContainer_L2CaloClustersAux.RoIword.clusterQuality.e233.e237.e277.e2tsts1.ehad1.emaxs1.energy.energySample.et.eta.eta1.fracs1.nCells.phi.rawEnergy.rawEnergySample.rawEt.rawEta.rawPhi.viewIndex.weta2.wstot",
                                      "xAOD::TrigElectronContainer_v1#HLT_xAOD__TrigElectronContainer_L2ElectronFex",
                                      "xAOD::TrigElectronAuxContainer_v1#HLT_xAOD__TrigElectronContainer_L2ElectronFexAux.pt.eta.phi.rawEnergy.rawEt.rawEta.nCells.energy.et.e237.e277.fracs1.weta2.ehad1.e232.wstot"  ]

streamPhysicsMain = ['Main', 'physics', "True", "True"]
streamCalibPebtestone = ['pebtestone', 'calibration', "True", "False"]
streamCalibPebtesttwo = ['pebtesttwo', 'calibration', "True", "False"]

stmaker = StreamTagMakerTool()
stmaker.OutputLevel = DEBUG
stmaker.ChainDecisions = "HLTSummary"
stmaker.PEBDecisionKeys = pebDecisionKeys
stmaker.ChainToStream = dict( [(c.name, streamPhysicsMain) for c in testChains ] )
stmaker.ChainToStream["HLT_e3_etcut_pebtestone"] = streamCalibPebtestone
stmaker.ChainToStream["HLT_e5_etcut_pebtestone"] = streamCalibPebtestone
stmaker.ChainToStream["HLT_mu6_pebtesttwo"] = streamCalibPebtesttwo

bitsmaker = TriggerBitsMakerTool()
bitsmaker.ChainDecisions = "HLTSummary"
bitsmaker.ChainToBit = dict( [ (chain.name, 10*num) for num,chain in enumerate(testChains) ] )
bitsmaker.OutputLevel = DEBUG

hltResultMakerTool =  HLTResultMTMaker("MKTool")
hltResultMakerTool.MakerTools = [ stmaker, bitsmaker, serialiser ]
hltResultMakerTool.OutputLevel = DEBUG

hltResultMakerAlg =  HLTResultMTMakerAlg("HLTRMakerAlg")

from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
hltResultMakerTool.MonTool = GenericMonitoringTool("MonOfHLTResultMTtest")
hltResultMakerTool.MonTool.HistPath = "OutputMonitoring"
hltResultMakerTool.MonTool.Histograms = [ defineHistogram( 'TIME_build', path='EXPERT', type='TH1F', title='Time of result construction in;[micro seccond]',
                                                           xbins=100, xmin=0, xmax=1000 ),
                                          defineHistogram( 'nstreams', path='EXPERT', type='TH1F', title='number of streams',
                                                           xbins=60, xmin=0, xmax=60 ),
                                          defineHistogram( 'nfrags', path='EXPERT', type='TH1F', title='number of HLT results',
                                                           xbins=10, xmin=0, xmax=10 ),
                                          defineHistogram( 'sizeMain', path='EXPERT', type='TH1F', title='Main (physics) HLT Result size;4B words',
                                                           xbins=100, xmin=-1, xmax=999 ) ] # 1000 k span

hltResultMakerAlg.ResultMaker = hltResultMakerTool
topSequence.hltTop += hltResultMakerAlg

##########################################
# Some debug
##########################################
from AthenaCommon.AlgSequence import dumpSequence
dumpSequence(topSequence)
