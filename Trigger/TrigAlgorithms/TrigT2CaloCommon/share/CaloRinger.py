#
#Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

if 'doHLTCaloTopo' not in dir() :
  doHLTCaloTopo=True
if 'doL2Egamma' not in dir():
  doL2Egamma=True
createHLTMenuExternally=True
doWriteRDOTrigger = False
doWriteBS = False

include("TriggerJobOpts/runHLT_standalone.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# ----------------------------------------------------------------
# Setup Views
# ----------------------------------------------------------------
from AthenaCommon.CFElements import stepSeq,seqOR,findAlgorithm
from DecisionHandling.DecisionHandlingConf import RoRSeqFilter
from AthenaCommon.Constants import DEBUG
from TriggerJobOpts.TriggerFlags import TriggerFlags

topSequence.remove( findAlgorithm(topSequence, "L1Decoder") )
from L1Decoder.L1DecoderConf import L1TestDecoder
topSequence += L1TestDecoder("L1TestDecoder", OutputLevel=DEBUG)


steps = seqOR("HLTTop")
topSequence += steps
#steps += topSequence.L1Decoder

if TriggerFlags.doCalo:

  if ( doHLTCaloTopo ) :
    from TrigT2CaloCommon.CaloDef import HLTFSTopoRecoSequence

    recosequence, caloclusters = HLTFSTopoRecoSequence("HLT_TestFSRoI")
    steps+=recosequence

  if ( doL2Egamma ) :

     from TrigT2CaloCommon.CaloDef import createFastCaloSequence


     filterL1RoIsAlg = RoRSeqFilter( "filterL1RoIsAlg")
     filterL1RoIsAlg.Input = ["HLTNav_TestL1EM"]
     filterL1RoIsAlg.Output = ["HLTNav_FilteredEMRoIDecisions"]
     filterL1RoIsAlg.Chains = [ "HLT_EMTestChain" ]
     (fastCaloSequence, sequenceOut) = createFastCaloSequence(filterL1RoIsAlg.Output[0], doRinger=True,
                                                              ClustersName="HLT_FastCaloEMClusters",
                                                              RingerKey="HLT_FastCaloRinger")
     steps+=stepSeq("finalCaloSequence", filterL1RoIsAlg, [ fastCaloSequence ])

  from AthenaCommon.AlgSequence import dumpMasterSequence
  dumpMasterSequence()

