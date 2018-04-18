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
topSequence.L1DecoderTest.prescaler.Prescales = ["HLT_e3_etcut:2", "HLT_2e3_etcut:2.5"]



from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_FastAlgo
theFastCaloAlgo=T2CaloEgamma_FastAlgo("FastCaloAlgo" )
theFastCaloAlgo.OutputLevel=VERBOSE
theFastCaloAlgo.ClustersName="L2CaloClusters"
svcMgr.ToolSvc.TrigDataAccess.ApplyOffsetCorrection=False

 
from TrigMultiVarHypo.TrigL2CaloRingerFexMTInit import init_ringer
trigL2CaloRingerFexMT = init_ringer()
trigL2CaloRingerFexMT.OutputLevel = DEBUG    



from AthenaCommon.CFElements import parOR, seqOR, seqAND, stepSeq, findAlgorithm
from DecisionHandling.DecisionHandlingConf import RoRSeqFilter, DumpDecisions
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm, TestEventViewCreatorAlgorithm


def createFastCaloSequence(rerun=False):
   __prefix = "Rerurn_" if rerun else ""
   __l1RoIDecisions = "RerunEMRoIDecisions" if rerun else "EMRoIDecisions"
   __forViewDecsions = "RerunEMRoIDecisions"  if rerun else "Filtered"+__l1RoIDecisions 

   from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_FastAlgo
   #clusterMaker=T2CaloEgamma_FastAlgo(__prefix+"FastClusterMaker" )
   clusterMaker=T2CaloEgamma_FastAlgo( "FastClusterMaker" )
   clusterMaker.OutputLevel=VERBOSE
   clusterMaker.ClustersName="L2CaloClusters"
   svcMgr.ToolSvc.TrigDataAccess.ApplyOffsetCorrection=False

   fastCaloInViewAlgs = seqAND( __prefix+"fastCaloInViewAlgs", [ clusterMaker ])

   filterL1RoIsAlg = RoRSeqFilter( __prefix+"filterL1RoIsAlg")
   filterL1RoIsAlg.Input = [__l1RoIDecisions]
   filterL1RoIsAlg.Output = ["Filtered"+__l1RoIDecisions]
   filterL1RoIsAlg.Chains = testChains
   filterL1RoIsAlg.OutputLevel = DEBUG

   fastCaloViewsMaker = TestEventViewCreatorAlgorithm( __prefix+"fastCaloViewsMaker", OutputLevel=DEBUG)
   fastCaloViewsMaker.ViewFallThrough = True
   fastCaloViewsMaker.InputDecisions =  [ __forViewDecsions ]
   fastCaloViewsMaker.RoIsLink = "initialRoI" # -||-
   fastCaloViewsMaker.InViewRoIs = "EMCaloRoIs" # contract with the fastCalo
   fastCaloViewsMaker.Views = __prefix+"EMCaloViews"
   fastCaloViewsMaker.ViewNodeName = __prefix+"fastCaloInViewAlgs"
   fastCaloViewsMaker.OutputDecisions = [ "L2CaloLinks"]
   clusterMaker.RoIs = fastCaloViewsMaker.InViewRoIs

   from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlgMT
   from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromName
   fastCaloHypo = TrigL2CaloHypoAlgMT( __prefix+"L2CaloHypo" )
   fastCaloHypo.OutputLevel = DEBUG
   fastCaloHypo.previousDecisions =  fastCaloViewsMaker.OutputDecisions[0] #   __l1RoIDecisions
#   fastCaloHypo.Views = fastCaloViewsMaker.Views
   fastCaloHypo.CaloClusters = clusterMaker.ClustersName
#   fastCaloHypo.RoIs = fastCaloViewsMaker.InViewRoIs
   fastCaloHypo.Output = __prefix+"EgammaCaloDecisions"
   fastCaloHypo.HypoTools =  [ TrigL2CaloHypoToolFromName( c ) for c in testChains ]

   for t in fastCaloHypo.HypoTools:
      t.OutputLevel = DEBUG

   fastCaloSequence = seqAND( __prefix+"fastCaloSequence", [fastCaloViewsMaker, fastCaloInViewAlgs, fastCaloHypo ])
   #if rerun: 
   #   return parOR(__prefix+"egammaCaloStep", [ fastCaloSequence ] )
   return stepSeq(__prefix+"egammaCaloStep", filterL1RoIsAlg, [ fastCaloSequence ])

egammaCaloStep = createFastCaloSequence( rerun=False )


from TrigUpgradeTest.InDetSetup import makeInDetAlgs

(viewAlgs, eventAlgs) = makeInDetAlgs()
from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_eGamma

theFTF = TrigFastTrackFinder_eGamma()
theFTF.isRoI_Seeded = True
viewAlgs.append(theFTF)


# A simple algorithm to confirm that data has been inherited from parent view
# Required to satisfy data dependencies
ViewVerify = CfgMgr.AthViews__ViewDataVerifier("electronViewDataVerifier")
ViewVerify.DataObjects = [('xAOD::TrigEMClusterContainer','StoreGateSvc+L2CaloClusters')]
ViewVerify.OutputLevel = DEBUG
viewAlgs.append(ViewVerify)

TrackParticlesName = ""
for viewAlg in viewAlgs:
  if viewAlg.name() == "InDetTrigTrackParticleCreatorAlg":
    TrackParticlesName = viewAlg.TrackParticlesName
    

from TrigEgammaHypo.TrigL2ElectronFexMTConfig import L2ElectronFex_1
theElectronFex= L2ElectronFex_1()
theElectronFex.TrigEMClusterName = theFastCaloAlgo.ClustersName
theElectronFex.TrackParticlesName = TrackParticlesName
theElectronFex.ElectronsName="Electrons"
theElectronFex.OutputLevel=VERBOSE



filterCaloRoIsAlg = RoRSeqFilter("filterCaloRoIsAlg")
caloHypoDecisions = findAlgorithm(egammaCaloStep, "L2CaloHypo").Output
print "kkkk ", caloHypoDecisions
filterCaloRoIsAlg.Input = [caloHypoDecisions]
filterCaloRoIsAlg.Output = ["Filtered" + caloHypoDecisions]
filterCaloRoIsAlg.Chains = testChains
filterCaloRoIsAlg.OutputLevel = DEBUG


l2ElectronViewsMaker = EventViewCreatorAlgorithm("l2ElectronViewsMaker", OutputLevel=DEBUG)
# topSequence += l2ElectronViewsMaker
l2ElectronViewsMaker.Decisions = filterCaloRoIsAlg.Output[0] # output of L2CaloHypo
l2ElectronViewsMaker.RoIsLink = "roi" # -||-
l2ElectronViewsMaker.InViewRoIs = "EMIDRoIs" # contract with the fastCalo
l2ElectronViewsMaker.Views = "EMElectronViews"
l2ElectronViewsMaker.ViewFallThrough = True


for viewAlg in viewAlgs:
  if viewAlg.properties().has_key("RoIs"):
    viewAlg.RoIs = l2ElectronViewsMaker.InViewRoIs
  if viewAlg.properties().has_key("roiCollectionName"):
    viewAlg.roiCollectionName = l2ElectronViewsMaker.InViewRoIs
theElectronFex.RoIs = l2ElectronViewsMaker.InViewRoIs    

electronInViewAlgs = parOR("electronInViewAlgs", viewAlgs + [ theElectronFex ])

l2ElectronViewsMaker.ViewNodeName = "electronInViewAlgs"


from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2ElectronHypoAlg
from TrigEgammaHypo.TrigL2ElectronHypoTool import TrigL2ElectronHypoToolFromName
theElectronHypo = TrigL2ElectronHypoAlg()
theElectronHypo.Views = l2ElectronViewsMaker.Views
theElectronHypo.Electrons = theElectronFex.ElectronsName
theElectronHypo.ClusterDecisions = caloHypoDecisions #theFastCaloHypo.Decisions 
theElectronHypo.ElectronDecisions = "ElectronL2Decisions"
theElectronHypo.OutputLevel = VERBOSE
theElectronHypo.HypoTools = [ TrigL2ElectronHypoToolFromName( c ) for c in testChains ]

for t in theElectronHypo.HypoTools:
  t.OutputLevel = VERBOSE
# topSequence += theElectronHypo
# InDetCacheCreatorTrigViews,
electronSequence = seqAND("electronSequence", eventAlgs + [l2ElectronViewsMaker, electronInViewAlgs, theElectronHypo ] )

electronDecisionsDumper = DumpDecisions("electronDecisionsDumper", OutputLevel=DEBUG, Decisions = theElectronHypo.ElectronDecisions )    
egammaIDStep = stepSeq("egammaIDStep", filterCaloRoIsAlg, [ electronSequence,  electronDecisionsDumper ] )


# CF construction

from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg
summaryStep0 = TriggerSummaryAlg( "TriggerSummaryStep1" )
summaryStep0.InputDecision = "HLTChains"
summaryStep0.HLTSummary = "MonitoringSummaryStep1"
summaryStep0.FinalDecisions = [ caloHypoDecisions ]
summaryStep0.OutputLevel = DEBUG


step0 = parOR("step0", [ egammaCaloStep, summaryStep0 ] )
step1 = parOR("step1", [ egammaIDStep ] )


egammaCaloStepRR = createFastCaloSequence( rerun=True )

step0r = parOR("step0r", [ egammaCaloStepRR ])

summary = TriggerSummaryAlg( "TriggerSummaryAlg" )
summary.InputDecision = "HLTChains"
summary.FinalDecisions = [ "ElectronL2Decisions", "MuonL2Decisions" ]
from TrigOutputHandling.TrigOutputHandlingConf import HLTEDMCreator
edmCreator = HLTEDMCreator()
edmCreator.TrigCompositeContainer = [ "EgammaCaloDecisions", "ElectronL2Decisions", "MuonL2Decisions", "EMRoIDecisions", "METRoIDecisions", "MURoIDecisions", "HLTChainsResult" ]
summary.OutputTools = [ edmCreator ]
summary.OutputLevel = DEBUG

steps = seqAND("HLTSteps", [ step0, step1, step0r, summary ]  )

from TrigSteerMonitor.TrigSteerMonitorConf import TrigSignatureMoniMT
mon = TrigSignatureMoniMT()
mon.FinalDecisions = [ "ElectronL2Decisions", "MuonL2Decisions", "WhateverElse" ]
from TrigUpgradeTest.TestUtils import MenuTest
mon.ChainsList = [ x.split(":")[1] for x in  MenuTest.CTPToChainMapping ]
mon.OutputLevel = DEBUG

import AthenaPoolCnvSvc.WriteAthenaPool
from OutputStreamAthenaPool.OutputStreamAthenaPool import  createOutputStream
StreamESD=createOutputStream("StreamESD","myESD.pool.root",True)
StreamESD.OutputLevel=VERBOSE
topSequence.remove( StreamESD )

def addTC(name):   
   StreamESD.ItemList += [ "xAOD::TrigCompositeContainer#"+name, "xAOD::TrigCompositeAuxContainer#"+name+"Aux." ]

for tc in edmCreator.TrigCompositeContainer:
   addTC( tc )

addTC("HLTSummary")

print "ESD file content " 
print StreamESD.ItemList


hltTop = seqOR( "hltTop", [ steps, mon, summary, StreamESD ] )
topSequence += hltTop


from AthenaCommon.AlgSequence import dumpSequence
dumpSequence(topSequence)

#print theElectronFex
#print ViewVerify
