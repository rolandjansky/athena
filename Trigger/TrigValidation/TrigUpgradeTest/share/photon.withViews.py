#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

include("TrigUpgradeTest/testHLT_MT.py")

testChains = ["HLT_g5_etcut"]

from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_ReFastAlgo
theFastCaloAlgo=T2CaloEgamma_ReFastAlgo("FastCaloAlgo" )
theFastCaloAlgo.ClustersName="L2CaloClusters"

 
from TrigMultiVarHypo.TrigL2CaloRingerFexMTInit import init_ringer
trigL2CaloRingerFexMT = init_ringer()

from AthenaCommon.CFElements import parOR, seqOR, seqAND, stepSeq, findAlgorithm
from DecisionHandling.DecisionHandlingConf import RoRSeqFilter, DumpDecisions
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm


def createFastCaloSequence(rerun=False):
   __prefix = "Rerurn_" if rerun else ""
   __l1RoIDecisions = "RerunEMRoIDecisions" if rerun else "EMRoIDecisions"
   __forViewDecsions = "RerunEMRoIDecisions"  if rerun else "Filtered"+__l1RoIDecisions 

   from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_ReFastAlgo
   clusterMaker=T2CaloEgamma_ReFastAlgo( "FastClusterMaker" )
   clusterMaker.ClustersName="L2CaloClusters"

   fastCaloInViewAlgs = seqAND( __prefix+"fastCaloInViewAlgs", [ clusterMaker ])

   filterL1RoIsAlg = RoRSeqFilter( __prefix+"filterL1RoIsAlg")
   filterL1RoIsAlg.Input = [__l1RoIDecisions]
   filterL1RoIsAlg.Output = ["Filtered"+__l1RoIDecisions]
   filterL1RoIsAlg.Chains = testChains

   fastCaloViewsMaker = EventViewCreatorAlgorithm( __prefix+"fastCaloViewsMaker" )
   fastCaloViewsMaker.ViewFallThrough = True
   fastCaloViewsMaker.InputMakerInputDecisions =  [ __forViewDecsions ]
   fastCaloViewsMaker.RoIsLink = "initialRoI" # -||-
   fastCaloViewsMaker.InViewRoIs = "EMCaloRoIs" # contract with the fastCalo
   fastCaloViewsMaker.Views = __prefix+"EMCaloViews"
   fastCaloViewsMaker.ViewNodeName = __prefix+"fastCaloInViewAlgs"
   fastCaloViewsMaker.InputMakerOutputDecisions = [ "L2CaloLinks"]
   clusterMaker.RoIs = fastCaloViewsMaker.InViewRoIs

   from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlgMT
   from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromName
   fastCaloHypo = TrigL2CaloHypoAlgMT( __prefix+"L2CaloHypo" )
   fastCaloHypo.HypoInputDecisions =  fastCaloViewsMaker.InputMakerOutputDecisions[0] #   __l1RoIDecisions
#   fastCaloHypo.Views = fastCaloViewsMaker.Views
   fastCaloHypo.CaloClusters = clusterMaker.ClustersName
#   fastCaloHypo.RoIs = fastCaloViewsMaker.InViewRoIs
   fastCaloHypo.HypoOutputDecisions = __prefix+"EgammaCaloDecisions"
   fastCaloHypo.HypoTools =  [ TrigL2CaloHypoToolFromName( c, c ) for c in testChains ]

   fastCaloSequence = seqAND( __prefix+"fastCaloSequence", [fastCaloViewsMaker, fastCaloInViewAlgs, fastCaloHypo ])
   #if rerun: 
   #   return parOR(__prefix+"egammaCaloStep", [ fastCaloSequence ] )
   return stepSeq(__prefix+"egammaCaloStep", filterL1RoIsAlg, [ fastCaloSequence ])

egammaCaloStep = createFastCaloSequence( rerun=False )



from TrigEgammaHypo.TrigL2PhotonFexMTConfig import L2PhotonFex_1
thePhotonFex= L2PhotonFex_1()
thePhotonFex.TrigEMClusterName = theFastCaloAlgo.ClustersName
thePhotonFex.PhotonsName="Photons"

filterCaloRoIsAlg = RoRSeqFilter("filterCaloRoIsAlg")
caloHypoDecisions = findAlgorithm(egammaCaloStep, "L2CaloHypo").HypoOutputDecisions
print "kkkk ", caloHypoDecisions
filterCaloRoIsAlg.Input = [caloHypoDecisions]
filterCaloRoIsAlg.Output = ["Filtered" + caloHypoDecisions]
filterCaloRoIsAlg.Chains = testChains

from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2PhotonHypoAlgMT
from TrigEgammaHypo.TrigL2PhotonHypoTool import TrigL2PhotonHypoToolFromName
thePhotonHypo = TrigL2PhotonHypoAlgMT()
thePhotonHypo.RunInView=True
thePhotonHypo.Photons = thePhotonFex.PhotonsName
thePhotonHypo.HypoTools = [ TrigL2PhotonHypoToolFromName( c, c ) for c in testChains ]


photonSequence = seqAND("photonSequence", [ thePhotonHypo ] )

egammaIDStep = stepSeq("egammaIDStep", filterCaloRoIsAlg, [ photonSequence ] )

# CF construction

from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg
summaryStep0 = TriggerSummaryAlg( "TriggerSummaryStep1" )
summaryStep0.InputDecision = "L1DecoderSummary"
summaryStep0.FinalDecisions = [ caloHypoDecisions ]

step0 = parOR("step0", [ egammaCaloStep, summaryStep0 ] )
step1 = parOR("step1", [ egammaIDStep ] )


egammaCaloStepRR = createFastCaloSequence( rerun=True )

step0r = parOR("step0r", [ egammaCaloStepRR ])

summary = TriggerSummaryAlg( "TriggerSummaryAlg" )
summary.InputDecision = "L1DecoderSummary"
summary.FinalDecisions = [ "PhotonL2Decisions" ]
from TrigOutputHandling.TrigOutputHandlingConf import HLTEDMCreator
edmCreator = HLTEDMCreator()
edmCreator.TrigCompositeContainer = [ "EgammaCaloDecisions", "PhotonL2Decisions", "EMRoIDecisions", "L1DecoderSummary" ]


egammaViewsMerger = HLTEDMCreator("egammaViewsMerger")

summary.OutputTools = [ edmCreator, egammaViewsMerger ]


step0filter = parOR("step0filter", [ findAlgorithm( egammaCaloStep, "filterL1RoIsAlg") ] )
step1filter = parOR("step1filter", [ findAlgorithm(egammaIDStep, "filterCaloRoIsAlg") ] )
step0rfilter = parOR("step0rfilter", [ findAlgorithm(egammaCaloStepRR, "Rerurn_filterL1RoIsAlg") ] )

#steps = seqAND("HLTSteps", [ step0, step1, step0r, summary ]  )
steps = seqAND("HLTSteps", [ step0filter, step0, step1filter, step1, step0rfilter, step0r, summary ]  )



from TrigSteerMonitor.TrigSteerMonitorConf import TrigSignatureMoniMT
mon = TrigSignatureMoniMT()
mon.FinalDecisions = [ "PhotonL2Decisions", "MuonL2Decisions", "WhateverElse" ]
from TrigUpgradeTest.TestUtils import MenuTest
mon.HLTTriggerMenu = [ x.split(":")[1] for x in  MenuTest.CTPToChainMapping ]

import AthenaPoolCnvSvc.WriteAthenaPool
from OutputStreamAthenaPool.CreateOutputStreams import  createOutputStream
StreamESD=createOutputStream("StreamESD","myESD.pool.root",True)
topSequence.remove( StreamESD )

def addTC(name):
   StreamESD.ItemList += [ "xAOD::TrigCompositeContainer#"+name, "xAOD::TrigCompositeAuxContainer#"+name+"Aux." ]

for tc in edmCreator.TrigCompositeContainer:
   addTC( tc )

addTC("HLTNav_Summary")

StreamESD.ItemList += [ "xAOD::TrigPhotonContainer#HLT_photons"]

print "ESD file content "
print StreamESD.ItemList


hltTop = seqOR( "hltTop", [ steps, mon, summary, StreamESD ] )
topSequence += hltTop


