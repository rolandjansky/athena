#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

include("TrigUpgradeTest/testHLT_MT.py")

#workaround to prevent online trigger folders to be enabled
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
InDetTrigFlags.useConditionsClasses.set_Value_and_Lock(False)


from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doCaloSeededBrem = False


from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.InDet25nsec = True 
InDetFlags.doPrimaryVertex3DFinding = False 
InDetFlags.doPrintConfigurables = False
InDetFlags.doResolveBackTracks = True 
InDetFlags.doSiSPSeededTrackFinder = True
InDetFlags.doTRTPhaseCalculation = True
InDetFlags.doTRTSeededTrackFinder = True
InDetFlags.doTruth = False
InDetFlags.init()

# PixelLorentzAngleSvc and SCTLorentzAngleSvc
include("InDetRecExample/InDetRecConditionsAccess.py")
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


from InDetRecExample.InDetKeys import InDetKeys

# provide a minimal menu information
if globalflags.InputFormat.is_bytestream():
   topSequence.L1DecoderTest.ctpUnpacker.OutputLevel=DEBUG
   topSequence.L1DecoderTest.roiUnpackers[0].OutputLevel=DEBUG
testChains = ["HLT_e3_etcut", "HLT_e5_etcut", "HLT_e7_etcut", "HLT_2e3_etcut", "HLT_e3e5_etcut"]


from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_FastAlgo
theFastCaloAlgo=T2CaloEgamma_FastAlgo("FastCaloAlgo" )
theFastCaloAlgo.OutputLevel=VERBOSE
theFastCaloAlgo.ClustersName="L2CaloClusters"
svcMgr.ToolSvc.TrigDataAccess.ApplyOffsetCorrection=False

# start here new part
#############################################################

from TrigUpgradeTest.HLTCFConfig import *
from TrigUpgradeTest.MenuComponents import *


def InputMakerAlg(name):
    from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestInputMaker
    return HLTTest__TestInputMaker(name, OutputLevel = DEBUG, LinkName="initialRoI")
 

def FastCaloAlg(name):
    from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_FastAlgo
    theFastCaloAlgo=T2CaloEgamma_FastAlgo(name)
    theFastCaloAlgo.OutputLevel=VERBOSE
    theFastCaloAlgo.ClustersName="L2CaloClusters"
    svcMgr.ToolSvc.TrigDataAccess.ApplyOffsetCorrection=False
    return theFastCaloAlgo

def fastCaloViewsMaker(name):
    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
    fastCaloViewsMaker = EventViewCreatorAlgorithm(name, OutputLevel=DEBUG)
    fastCaloViewsMaker.ViewFallThrough = True
#    fastCaloViewsMaker.Decisions = "FilteredEMRoIDecisions" # from EMRoIsUnpackingTool
    fastCaloViewsMaker.RoIsLink = "initialRoI" # -||-
    fastCaloViewsMaker.InViewRoIs = "EMCaloRoIs" # contract with the fastCalo
    fastCaloViewsMaker.Views = "EMCaloViews"
 #   fastCaloViewsMaker.ViewNodeName = "fastCaloInViewAlgs"
 #add algo
    theFastCaloAlgo = FastCaloAlg(name="FastCaloAlgo")
    fastCaloInViewAlgs = seqAND("fastCaloInViewAlgs", [ theFastCaloAlgo ])
    theFastCaloAlgo.RoIs = fastCaloViewsMaker.InViewRoIs


  
def FastCaloHypo(name):
  from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlg
  theFastCaloHypo = TrigL2CaloHypoAlg(name)
  theFastCaloHypo.OutputLevel = DEBUG 
  return theFastCaloHypo


def TrigL2CaloHypoTool(name):
  from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromName
  hypotool= TrigL2CaloHypoToolFromName(name)
  hypotool.OutputLevel = DEBUG
  return hypotool

def e3_etcut():
  return TrigL2CaloHypoTool("HLT_e3_etcut")

def e5_etcut():
  return TrigL2CaloHypoTool("HLT_e5_etcut")


#####################
# Make the sequence


fastCaloViewsMaker = fastCaloViewsMaker(name="fastCaloViewsMaker")
theFastCaloHypo = FastCaloHypo(name="L2CaloHypo")

# Nodes & sequence
fastCaloViewsMakerNode = AlgNode( Alg=fastCaloViewsMaker,inputProp='Decisions', outputProp='Views') 
fastCaloHypoNode = HypoAlgNode( Alg=theFastCaloHypo,inputProp='Views', outputProp='Decisions')
fastCalo_NodeSequence = NodeSequence("fastCalo_NodeSequence", Algs=[fastCaloViewsMaker], Hypo=fastCaloHypoNode, Seed=EMRoIDecisions)

#remaining connecitons to the Hypo
theFastCaloHypo.RoIs = l2CaloViewsMaker.InViewRoIs
theFastCaloHypo.CaloClusters = theFastCaloAlgo.ClustersName
theFastCaloHypo.L1Decisions = EMRoIDecisions

#final sequence
fastCaloSequence = MenuSequence("egammaCaloStep", nodeSeqList=[fastCalo_NodeSequence])
print fastCaloSequence


#########################################
# here tracking.....
#########################################


# menu

testChains  = [
  Chain(name='HLT_e3_etcut', Seed="L1_EM3",   ChainSteps=[ChainStep("Step1_e3_etcut", [SequenceHypoTool(fastCaloSequence,e3_etcut() )])]),
  Chain(name='HLT_e5_etcut', Seed="L1_EM3",   ChainSteps=[ChainStep("Step1_e5_etcut", [SequenceHypoTool(fastCaloSequence,e5_etcut() )])])
  ]


# CF construction

from AthenaCommon.CFElements import parOR, seqOR, seqAND, stepSeq

from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg
summary = TriggerSummaryAlg( "TriggerSummaryAlg" )
summary.L1Decision = "HLTChains"
summary.FinalDecisions = [ "ElectronL2Decisions", "MuonL2Decisions" ]
summary.OutputLevel = DEBUG

steps = seqAND("HLTSteps"  )
decisionTree_From_Chains(steps, testChains, NSTEPS=1)
steps += summary

mon = TriggerSummaryAlg( "TriggerMonitoringAlg" )
mon.L1Decision = "HLTChains"
mon.FinalDecisions = [ "ElectronL2Decisions", "MuonL2Decisions", "WhateverElse" ]
mon.HLTSummary = "MonitoringSummary"
mon.OutputLevel = DEBUG


hltTop = seqOR( "hltTop", [ steps, mon] )
topSequence += hltTop


print fastCaloViewsMaker
print theFastCaloHypo


##########################################  


  
from AthenaCommon.AlgSequence import dumpSequence
dumpSequence(topSequence)
