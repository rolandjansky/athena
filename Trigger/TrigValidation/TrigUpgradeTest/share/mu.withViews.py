#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.doMuon=True
include("TrigUpgradeTest/testHLT_MT.py")

#workaround to prevent online trigger folders to be enabled
# from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
# InDetTrigFlags.useConditionsClasses.set_Value_and_Lock(False)


# from InDetRecExample.InDetJobProperties import InDetFlags
# InDetFlags.doCaloSeededBrem = False


# from InDetRecExample.InDetJobProperties import InDetFlags
# InDetFlags.InDet25nsec = True 
# InDetFlags.doPrimaryVertex3DFinding = False 
# InDetFlags.doPrintConfigurables = False
# InDetFlags.doResolveBackTracks = True 
# InDetFlags.doSiSPSeededTrackFinder = True
# InDetFlags.doTRTPhaseCalculation = True
# InDetFlags.doTRTSeededTrackFinder = True
# InDetFlags.doTruth = False
# InDetFlags.init()

# PixelLorentzAngleSvc and SCTLorentzAngleSvc
include("InDetRecExample/InDetRecConditionsAccess.py")

viewTest = opt.enableViews   # from testHLT_MT.py
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
if viewTest:
  allViewAlgorithms = topSequence.allViewAlgorithms

#from InDetRecExample.InDetKeys import InDetKeys


# provide a minimal menu information
#topSequence.L1DecoderTest.ctpUnpacker.CTPToChainMapping = ['0:HLT_e3', '0:HLT_e5', '1:HLT_e7', '1:HLT_e10']
topSequence.L1DecoderTest.ctpUnpacker.OutputLevel=DEBUG

#topSequence.L1DecoderTest.roiUnpackers[0].ThresholdToChainMapping = ['EM3 : HLT_e3', 'EM3 : HLT_e5', 'EM7 : HLT_e7', 'EM7 : HLT_e10']
topSequence.L1DecoderTest.roiUnpackers[0].OutputLevel=DEBUG
topSequence.L1DecoderTest.roiUnpackers[1].OutputLevel=DEBUG
#topSequence.L1DecoderTest.roi[].ThresholdToChainMapping = ['EM3 : HLT_e3', 'EM3 : HLT_e5', 'EM7 : HLT_e7', 'EM7 : HLT_e10']








from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
if viewTest:
  from TrigL2MuonSA.TrigL2MuonSAConfig import MuFastSteering
  muFast = MuFastSteering("muFast")
  muFast.OutputLevel=DEBUG
  muFast.MuRoIs = "L1MURoIs"
  allViewAlgorithms += muFast
  svcMgr.ViewAlgPool.TopAlg += [ muFast.getName() ]
  l2MuViewsMaker = EventViewCreatorAlgorithm("l2MuViewsMaker", OutputLevel=DEBUG)
  
  l2MuViewsMaker.Decisions = "MURoIDecisions" # from MURoIsUnpackingTool
  l2MuViewsMaker.RoIsLink = "initialRoI" # -||-
  l2MuViewsMaker.InViewRoIs = muFast.MuRoIs # contract with the consumer
  l2MuViewsMaker.Views = "MuViews"
  l2MuViewsMaker.AlgorithmNameSequence = [ muFast.getName() ]


  topSequence += l2MuViewsMaker



