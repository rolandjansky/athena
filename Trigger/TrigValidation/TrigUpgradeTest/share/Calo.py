#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

if not 'doHLTCaloTopo' in dir() :
  doHLTCaloTopo=True
if not 'doL2Egamma' in dir():
  doL2Egamma=True

include("TrigUpgradeTest/testHLT_MT.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# ----------------------------------------------------------------
# Setup Views
# ----------------------------------------------------------------
from AthenaCommon.CFElements import stepSeq,seqOR
from DecisionHandling.DecisionHandlingConf import RoRSeqFilter


steps = seqOR("HLTTop")
topSequence += steps
steps += topSequence.L1Decoder




if TriggerFlags.doCalo:

  if ( doHLTCaloTopo ) :
    from TrigT2CaloCommon.CaloDef import HLTFSTopoRecoSequence
    recosequence, caloclusters = HLTFSTopoRecoSequence("FSRoI")
    steps+=recosequence
 
  if ( doL2Egamma ) :

     from TrigT2CaloCommon.CaloDef import createFastCaloSequence

     filterL1RoIsAlg = RoRSeqFilter( "filterL1RoIsAlg")
     filterL1RoIsAlg.Input = ["L1EM"]
     filterL1RoIsAlg.Output = ["FilteredEMRoIDecisions"]
     filterL1RoIsAlg.Chains = [ "HLT_e3_etcut", "HLT_e5_etcut", "HLT_e7_etcut" ]
     (fastCaloSequence, sequenceOut) = createFastCaloSequence(filterL1RoIsAlg.Output[0])
     steps+=stepSeq("finalCaloSequence", filterL1RoIsAlg, [ fastCaloSequence ])

  from AthenaCommon.AlgSequence import dumpMasterSequence
  dumpMasterSequence()

