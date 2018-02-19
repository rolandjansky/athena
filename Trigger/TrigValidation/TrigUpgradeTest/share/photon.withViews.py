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


from InDetRecExample.InDetKeys import InDetKeys

# provide a minimal menu information
if globalflags.InputFormat.is_bytestream():
   topSequence.L1DecoderTest.ctpUnpacker.OutputLevel=DEBUG
   topSequence.L1DecoderTest.roiUnpackers[0].OutputLevel=DEBUG
testChains = ["HLT_g5_etcut"]


from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_FastAlgo
theFastCaloAlgo=T2CaloEgamma_FastAlgo("FastCaloAlgo" )
theFastCaloAlgo.OutputLevel=VERBOSE
theFastCaloAlgo.ClustersName="L2CaloClusters"
svcMgr.ToolSvc.TrigDataAccess.ApplyOffsetCorrection=False

from AthenaCommon.CFElements import parOR, seqOR, seqAND, stepSeq

from DecisionHandling.DecisionHandlingConf import RoRSeqFilter, DumpDecisions

from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm

fastCaloInViewAlgs = seqAND("fastCaloInViewAlgs", [ theFastCaloAlgo ])


filterL1RoIsAlg = RoRSeqFilter("filterL1RoIsAlg")
filterL1RoIsAlg.Input = ["EMRoIDecisions"]
filterL1RoIsAlg.Output = ["FilteredEMRoIDecisions"]
filterL1RoIsAlg.Chains = testChains
filterL1RoIsAlg.OutputLevel = DEBUG


  


fastCaloViewsMaker = EventViewCreatorAlgorithm("fastCaloViewsMaker", OutputLevel=DEBUG)
fastCaloViewsMaker.ViewFallThrough = True
fastCaloViewsMaker.Decisions = "FilteredEMRoIDecisions" # from EMRoIsUnpackingTool
fastCaloViewsMaker.RoIsLink = "initialRoI" # -||-
fastCaloViewsMaker.InViewRoIs = "EMCaloRoIs" # contract with the fastCalo
fastCaloViewsMaker.Views = "EMCaloViews"
fastCaloViewsMaker.ViewNodeName = "fastCaloInViewAlgs"
theFastCaloAlgo.RoIs = fastCaloViewsMaker.InViewRoIs


from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2PhotonHypo
from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlg
from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromName
theFastCaloHypo = TrigL2CaloHypoAlg("L2CaloHypo")
theFastCaloHypo.OutputLevel = DEBUG
theFastCaloHypo.L1Decisions = "EMRoIDecisions"
theFastCaloHypo.Views = fastCaloViewsMaker.Views
theFastCaloHypo.CaloClusters = theFastCaloAlgo.ClustersName
theFastCaloHypo.RoIs = fastCaloViewsMaker.InViewRoIs
theFastCaloHypo.Decisions = "EgammaCaloDecisions"
theFastCaloHypo.HypoTools =  [ TrigL2CaloHypoToolFromName ( c ) for c in testChains ]

for t in theFastCaloHypo.HypoTools:
  t.OutputLevel = DEBUG
  
# topSequence += theFastCaloHypo

caloDecisionsDumper = DumpDecisions("caloDecisionsDumper", OutputLevel=DEBUG, Decisions = theFastCaloHypo.Decisions )  

fastCaloSequence = seqAND("fastCaloSequence", [fastCaloViewsMaker, fastCaloInViewAlgs, theFastCaloHypo ])
  
egammaCaloStep = stepSeq("egammaCaloStep", filterL1RoIsAlg, [ fastCaloSequence,  caloDecisionsDumper ])



from TrigEgammaHypo.TrigL2PhotonFexMTConfig import L2PhotonFex_1

thePhotonFex= L2PhotonFex_1()
thePhotonFex.OutputLevel=VERBOSE



filterCaloRoIsAlg = RoRSeqFilter("filterCaloRoIsAlg")
filterCaloRoIsAlg.Input = [theFastCaloHypo.Decisions]
filterCaloRoIsAlg.Output = ["Filtered"+theFastCaloHypo.Decisions]
filterCaloRoIsAlg.Chains = testChains
filterCaloRoIsAlg.OutputLevel = DEBUG


l2PhotonViewsMaker = EventViewCreatorAlgorithm("l2PhotonViewsMaker", OutputLevel=DEBUG)
l2PhotonViewsMaker.Decisions = filterCaloRoIsAlg.Output[0] # output of L2CaloHypo
l2PhotonViewsMaker.RoIsLink = "roi" # -||-
l2PhotonViewsMaker.InViewRoIs = "EMIDRoIs" # contract with the fastCalo
l2PhotonViewsMaker.Views = "EMPhotonViews"
l2PhotonViewsMaker.ViewFallThrough = True


egammaIDStep = stepSeq("egammaIDStep", filterCaloRoIsAlg, [ l2PhotonViewsMaker ] )



# CF construction

step0 = parOR("step0", [ egammaCaloStep ] )
step1 = parOR("step1", [ egammaIDStep ] )

from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg
summary = TriggerSummaryAlg( "TriggerSummaryAlg" )
summary.L1Decision = "HLTChains"
summary.FinalDecisions = [ "ElectronL2Decisions", "MuonL2Decisions", "PhotonL2Decisions" ]
summary.OutputLevel = DEBUG

steps = seqAND("HLTSteps", [ step0, step1, summary ]  )

mon = TriggerSummaryAlg( "TriggerMonitoringAlg" )
mon.L1Decision = "HLTChains"
mon.FinalDecisions = [ "PhotonL2Decisions", "MuonL2Decisions", "WhateverElse" ]
mon.HLTSummary = "MonitoringSummary"
mon.OutputLevel = DEBUG

hltTop = seqOR( "hltTop", [ steps, mon] )
topSequence += hltTop
  
