#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

if not 'doHLTCaloTopo' in dir() :
  doHLTCaloTopo=True
if not 'doL2Egamma' in dir():
  doL2Egamma=True

include("TrigUpgradeTest/testHLT_MT.py")

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
from DecisionHandling.DecisionHandlingConf import RoRSeqFilter


steps = seqOR("HLTTop")
topSequence += steps
steps += topSequence.L1DecoderTest




if TriggerFlags.doCalo:

  if ( doHLTCaloTopo ) :

     from TrigT2CaloCommon.CaloDef import algoHLTCaloCell, algoHLTTopoCluster
     steps+=algoHLTCaloCell(OutputLevel=DEBUG)    
     steps+=algoHLTTopoCluster(OutputLevel=DEBUG)    

  if ( doL2Egamma ) :

     from TrigT2CaloCommon.CaloDef import createFastCaloSequence
     from AthenaCommon.Constants import DEBUG

     filterL1RoIsAlg = RoRSeqFilter( "filterL1RoIsAlg")
     filterL1RoIsAlg.Input = ["L1EM"]
     filterL1RoIsAlg.Output = ["FilteredEMRoIDecisions"]
     filterL1RoIsAlg.Chains = [ "HLT_e3_etcut", "HLT_e5_etcut", "HLT_e7_etcut" ]
     filterL1RoIsAlg.OutputLevel = DEBUG
     (fastCaloSequence, sequenceOut) = createFastCaloSequence(filterL1RoIsAlg.Output[0])
     steps+=stepSeq("finalCaloSequence", filterL1RoIsAlg, [ fastCaloSequence ])

  from AthenaCommon.AlgSequence import dumpMasterSequence
  dumpMasterSequence()

