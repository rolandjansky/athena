#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

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
viewSeq = AthSequencer("AthViewSeq", Sequential=True, ModeOR=False, StopOverride=False)
topSequence += viewSeq

  
# View maker alg
viewNodeName = "allViewAlgorithms"
viewMaker = CfgMgr.AthViews__RoiCollectionToViews("viewMaker")
viewMaker.ViewBaseName = "testView"
viewMaker.InputRoICollection = "EMRoIs"
viewMaker.ViewNodeName = viewNodeName
viewMaker.OutputRoICollection = "EMViewRoIs"
viewMaker.ViewFallThrough = True
viewSeq += viewMaker

# Set of view algs
allViewAlgorithms = AthSequencer(viewNodeName, Sequential=False, ModeOR=False, StopOverride=False)


if TriggerFlags.doID:
  #workaround to prevent online trigger folders to be enabled
  from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
  InDetTrigFlags.useConditionsClasses.set_Value_and_Lock(False)

  
  
  from InDetRecExample.InDetJobProperties import InDetFlags
  InDetFlags.doCaloSeededBrem = False
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

  from TrigUpgradeTest.InDetSetup import makeInDetAlgs
  
  (viewAlgs, eventAlgs) = makeInDetAlgs()

  for eventAlg in eventAlgs:
    viewSeq += eventAlg

  for viewAlg in viewAlgs:
    allViewAlgorithms += viewAlg
  

if TriggerFlags.doCalo:
  svcMgr.ToolSvc.TrigDataAccess.ApplyOffsetCorrection=False
  
  from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_FastAlgo
  algo=T2CaloEgamma_FastAlgo("testFastAlgo")
  algo.OutputLevel=VERBOSE

  algo.RoIs="EMViewRoIs"
  allViewAlgorithms += algo

viewSeq += allViewAlgorithms
