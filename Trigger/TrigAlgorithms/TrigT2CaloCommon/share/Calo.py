#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

if not 'doHLTCaloTopo' in dir() :
  doHLTCaloTopo=True
if not 'doL2Egamma' in dir():
  doL2Egamma=True
createHLTMenuExternally=True
doWriteRDOTrigger = False
doWriteBS = False
include("TriggerJobOpts/runHLT_standalone.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

doHLTCaloTopo=True
doL2Egamma=True

# ----------------------------------------------------------------
# Setup Views
# ----------------------------------------------------------------
from AthenaCommon.CFElements import stepSeq,seqOR,findAlgorithm
from DecisionHandling.DecisionHandlingConf import RoRSeqFilter

topSequence.remove( findAlgorithm(topSequence, "HLTSeeding") )
from HLTSeeding.HLTSeedingConf import HLTSeedingNoCtpForTesting
topSequence += HLTSeedingNoCtpForTesting("HLTSeedingNoCtpForTesting", OutputLevel=DEBUG)

steps = seqOR("HLTTop")
topSequence += steps

if ConfigFlags.Trigger.doCalo:
  if ( doHLTCaloTopo ) :
    from TrigT2CaloCommon.CaloDef import HLTFSTopoRecoSequence
    recosequence, caloclusters = HLTFSTopoRecoSequence("HLT_TestFSRoI")
    steps += recosequence

  if ( doL2Egamma ) :
     from TrigT2CaloCommon.CaloDef import createFastCaloSequence
     filterL1RoIsAlg = RoRSeqFilter( "filterL1RoIsAlg")
     filterL1RoIsAlg.Input = ["HLTNav_TestL1EM"]
     filterL1RoIsAlg.Output = ["HLTNav_FilteredEMRoIDecisions"]
     filterL1RoIsAlg.Chains = [ "HLT_EMTestChain" ]
     (fastCaloSequence, sequenceOut) = createFastCaloSequence(filterL1RoIsAlg.Output[0])
     steps+=stepSeq("finalCaloSequence", filterL1RoIsAlg, [ fastCaloSequence ])

  if (True):
    from LArRecUtils.LArMCSymCondAlg import LArMCSymCondAlgDefault
    LArMCSymCondAlgDefault()
    from LArBadChannelTool.LArBadChannelAccess import LArBadChannelAccess
    LArBadChannelAccess()

    from TrigMinBias.TrigMinBiasConf import MbtsFex
    alg=MbtsFex()
    steps += alg



  from AthenaCommon.AlgSequence import dumpMasterSequence
  dumpMasterSequence()

from AthenaCommon.CFElements import findAlgorithm

findAlgorithm( topSequence, "HLTCaloCellMakerFS").OutputLevel=DEBUG
findAlgorithm( topSequence, "FastCaloL2EgammaAlg").OutputLevel=DEBUG
