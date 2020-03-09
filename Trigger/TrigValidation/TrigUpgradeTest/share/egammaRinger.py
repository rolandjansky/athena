#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Constants import ERROR, DEBUG
from AthenaCommon.CFElements import seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm


doWriteRDOTrigger = False
doWriteBS = False
include("TriggerJobOpts/runHLT_standalone.py")
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
isData = False
if globalflags.InputFormat.is_bytestream():
  isData = True
# ----------------------------------------------------------------
# Setup Views
# ----------------------------------------------------------------
from AthenaCommon.AlgSequence import AthSequencer
from AthenaCommon.CFElements import stepSeq,seqOR
from AthenaCommon.Constants import DEBUG
from DecisionHandling.DecisionHandlingConf import RoRSeqFilter
from TriggerJobOpts.TriggerFlags import TriggerFlags
from TrigT2CaloCommon.CaloDef import _algoHLTCaloCell, _algoHLTTopoCluster, _algoL2Egamma
TriggerFlags.enableMonitoring = ['Validation','Online']



doRinger=True

steps = seqOR("HLTTop")
topSequence += steps
steps += topSequence.L1Decoder
steps+=_algoHLTCaloCell(OutputLevel=DEBUG)    
steps+=_algoHLTTopoCluster(OutputLevel=DEBUG)    

from L1Decoder.L1DecoderConfig import mapThresholdToL1DecisionCollection

filterL1RoIsAlg = RoRSeqFilter( "filterL1RoIsAlg")
filterL1RoIsAlg.Input = [mapThresholdToL1DecisionCollection("EM")]
filterL1RoIsAlg.Output = ["FilteredEMRoIDecisions"]
filterL1RoIsAlg.Chains = [ 
                           "HLT_e3_etcut", 
                           "HLT_e5_etcut", 
                           "HLT_e7_etcut", 
                           "HLT_e26_lhtight",
                           ]
filterL1RoIsAlg.OutputLevel = DEBUG
 
InViewRoIs="EMCaloRoIs"
clusterMaker = _algoL2Egamma(OutputLevel=FATAL,inputEDM=InViewRoIs,doRinger=True, ClustersName="HLT_L2CaloEMClusters",RingerKey="HLT_L2CaloRinger")
fastCaloInViewSequence = seqAND( 'fastCaloInViewSequence', [clusterMaker] )
fastCaloViewsMaker = EventViewCreatorAlgorithm( "fastCaloViewsMaker", OutputLevel=DEBUG)
fastCaloViewsMaker.ViewFallThrough = True
fastCaloViewsMaker.InputMakerInputDecisions =  [ "FilteredEMRoIDecisions" ]
fastCaloViewsMaker.RoIsLink = "initialRoI"
fastCaloViewsMaker.InViewRoIs = InViewRoIs
fastCaloViewsMaker.Views = "EMCaloViews"
fastCaloViewsMaker.ViewNodeName = "fastCaloInViewSequence"
fastCaloViewsMaker.InputMakerOutputDecisions = [ "L2CaloLinks"]
clusterMaker.OutputLevel=FATAL

if doRinger:
   from TrigMultiVarHypo.TrigL2CaloRingerHypoTool import createRingerDecisions
   fastCaloHypo = createRingerDecisions( "testRingerHypo" , filterL1RoIsAlg.Chains,
                                         ClustersKey=clusterMaker.ClustersName,
                                         RingerKey="HLT_L2CaloRinger")

else:
   from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlgMT
   from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromName
   fastCaloHypo = TrigL2CaloHypoAlgMT( "L2CaloHypo" )
   fastCaloHypo.HypoTools =  [ TrigL2CaloHypoToolFromName( c,c ) for c in filterL1RoIsAlg.Chains ]
   fastCaloHypo.CaloClusters = clusterMaker.ClustersName

for t in fastCaloHypo.HypoTools:
   t.OutputLevel=DEBUG

fastCaloHypo.HypoOutputDecisions = "EgammaCaloDecisions"
fastCaloHypo.OutputLevel= DEBUG
fastCaloHypo.HypoInputDecisions =  fastCaloViewsMaker.InputMakerOutputDecisions[0] #   __l1RoIDecisions


fastCaloSequence = seqAND("fastCaloSequence", [fastCaloViewsMaker, fastCaloInViewSequence, fastCaloHypo ])
steps+=stepSeq("finalCaloSequence", filterL1RoIsAlg, [ fastCaloSequence ])

 




from AthenaCommon.AlgSequence import dumpMasterSequence
dumpMasterSequence()
