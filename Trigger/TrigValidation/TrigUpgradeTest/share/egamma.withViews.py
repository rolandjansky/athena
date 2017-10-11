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

viewTest = opt.enableViews   # from testHLT_MT.py
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
if viewTest:
  allViewAlgorithms = topSequence.allViewAlgorithms

from InDetRecExample.InDetKeys import InDetKeys

# provide a minimal menu information

topSequence.L1DecoderTest.ctpUnpacker.OutputLevel=DEBUG
topSequence.L1DecoderTest.roiUnpackers[0].OutputLevel=DEBUG
testChains = ["HLT_e3_etcut", "HLT_e5_etcut", "HLT_e7_etcut", "HLT_2e3_etcut", "HLT_e3e5_etcut"]


from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_FastAlgo
theFastCaloAlgo=T2CaloEgamma_FastAlgo("FastCaloAlgo" )
theFastCaloAlgo.OutputLevel=VERBOSE
theFastCaloAlgo.ClustersName="L2CaloClusters"
svcMgr.ToolSvc.TrigDataAccess.ApplyOffsetCorrection=False

from AthenaCommon.CFElements import parOR, seqOR, seqAND, stepSeq

from DecisionHandling.DecisionHandlingConf import RoRSeqFilter, DumpDecisions

from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
if viewTest:
  filterL1RoIsAlg = RoRSeqFilter("filterL1RoIsAlg")
  filterL1RoIsAlg.Input = ["EMRoIDecisions"]
  filterL1RoIsAlg.Output = ["FilteredEMRoIDecisions"]
  filterL1RoIsAlg.Chains = testChains
  filterL1RoIsAlg.OutputLevel = DEBUG
  
  allViewAlgorithms += theFastCaloAlgo
  svcMgr.ViewAlgPool.TopAlg += [ theFastCaloAlgo.getName() ]
  l2CaloViewsMaker = EventViewCreatorAlgorithm("l2CaloViewsMaker", OutputLevel=DEBUG)
  l2CaloViewsMaker.ViewFallThrough = True
  l2CaloViewsMaker.Decisions = "FilteredEMRoIDecisions" # from EMRoIsUnpackingTool
  l2CaloViewsMaker.RoIsLink = "initialRoI" # -||-
  l2CaloViewsMaker.InViewRoIs = "EMCaloRoIs" # contract with the fastCalo
  l2CaloViewsMaker.Views = "EMCaloViews"
  l2CaloViewsMaker.AlgorithmNameSequence = [ theFastCaloAlgo.getName() ]
  theFastCaloAlgo.RoIs = l2CaloViewsMaker.InViewRoIs


  from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlg
  from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromName
  theFastCaloHypo = TrigL2CaloHypoAlg("L2CaloHypo")
  theFastCaloHypo.OutputLevel = DEBUG
  theFastCaloHypo.L1Decisions = "EMRoIDecisions"
  theFastCaloHypo.Views = l2CaloViewsMaker.Views
  theFastCaloHypo.CaloClusters = theFastCaloAlgo.ClustersName
  theFastCaloHypo.RoIs = l2CaloViewsMaker.InViewRoIs
  theFastCaloHypo.Decisions = "EgammaCaloDecisions"
  theFastCaloHypo.HypoTools =  [ TrigL2CaloHypoToolFromName( c ) for c in testChains ]
#[ TrigL2CaloHypoToolFromName("HLT_e5_etcut"),   TrigL2CaloHypoToolFromName("HLT_e7_etcut") , TrigL2CaloHypoToolFromName("HLT_2e3_etcut"), TrigL2CaloHypoToolFromName("HLT_e3e5_etcut") ]

  for t in theFastCaloHypo.HypoTools:
    t.OutputLevel = DEBUG

  # topSequence += theFastCaloHypo

  caloDecisionsDumper = DumpDecisions("caloDecisionsDumper", OutputLevel=DEBUG, Decisions = theFastCaloHypo.Decisions )  

  egammaCaloStep = stepSeq("egammaCaloStep", filterL1RoIsAlg, [ l2CaloViewsMaker, theFastCaloHypo, caloDecisionsDumper ])

else:
  topSequence += theFastCaloAlgo




#Pixel

from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRodDecoder
InDetPixelRodDecoder = PixelRodDecoder(name = "InDetPixelRodDecoder")
ToolSvc += InDetPixelRodDecoder

from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRawDataProviderTool
InDetPixelRawDataProviderTool = PixelRawDataProviderTool(name    = "InDetPixelRawDataProviderTool",
                                                         Decoder = InDetPixelRodDecoder)
ToolSvc += InDetPixelRawDataProviderTool
if (InDetFlags.doPrintConfigurables()):
  print      InDetPixelRawDataProviderTool

# load the PixelRawDataProvider
from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRawDataProvider
InDetPixelRawDataProvider = PixelRawDataProvider(name         = "InDetPixelRawDataProvider",
                                                 RDOKey       = InDetKeys.PixelRDOs(),
                                                 ProviderTool = InDetPixelRawDataProviderTool,
                                                 isRoI_Seeded = True )

#SCT
from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCT_RodDecoder
InDetSCTRodDecoder = SCT_RodDecoder(name        = "InDetSCTRodDecoder",
                                    TriggerMode = False)
ToolSvc += InDetSCTRodDecoder

from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCTRawDataProviderTool
InDetSCTRawDataProviderTool = SCTRawDataProviderTool(name    = "InDetSCTRawDataProviderTool",
                                                    Decoder = InDetSCTRodDecoder)
ToolSvc += InDetSCTRawDataProviderTool
if (InDetFlags.doPrintConfigurables()):
  print      InDetSCTRawDataProviderTool

# load the SCTRawDataProvider
from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCTRawDataProvider
InDetSCTRawDataProvider = SCTRawDataProvider(name         = "InDetSCTRawDataProvider",
                                             RDOKey       = InDetKeys.SCT_RDOs(),
                                             ProviderTool = InDetSCTRawDataProviderTool,
                                             isRoI_Seeded = True )



#TRT
from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
InDetTRTCalDbSvc = TRT_CalDbSvc()
ServiceMgr += InDetTRTCalDbSvc

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummarySvc
InDetTRTStrawStatusSummarySvc = TRT_StrawStatusSummarySvc(name = "InDetTRTStrawStatusSummarySvc")
ServiceMgr += InDetTRTStrawStatusSummarySvc

from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRT_RodDecoder
InDetTRTRodDecoder = TRT_RodDecoder(name = "InDetTRTRodDecoder",
                                    LoadCompressTableDB = True)#(globalflags.DataSource() != 'geant4'))  
ToolSvc += InDetTRTRodDecoder
  
from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRTRawDataProviderTool
InDetTRTRawDataProviderTool = TRTRawDataProviderTool(name    = "InDetTRTRawDataProviderTool",
                                                      Decoder = InDetTRTRodDecoder)
ToolSvc += InDetTRTRawDataProviderTool

  
# load the TRTRawDataProvider
from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRTRawDataProvider
InDetTRTRawDataProvider = TRTRawDataProvider(name         = "InDetTRTRawDataProvider",
                                             RDOKey       = "TRT_RDOs",
                                             ProviderTool = InDetTRTRawDataProviderTool,
                                             isRoI_Seeded = True
                                             )



#Pixel clusterisation

from SiClusterizationTool.SiClusterizationToolConf import InDet__ClusterMakerTool
InDetClusterMakerTool = InDet__ClusterMakerTool(name                 = "InDetClusterMakerTool",
    PixelCalibSvc        = None,
    PixelOfflineCalibSvc = None,
    UsePixelCalibCondDB  = False)

ToolSvc += InDetClusterMakerTool


from SiClusterizationTool.SiClusterizationToolConf import InDet__MergedPixelsTool
InDetMergedPixelsTool = InDet__MergedPixelsTool(name                    = "InDetMergedPixelsTool",
                                                globalPosAlg            = InDetClusterMakerTool,
                                                MinimalSplitSize        = 0,
                                                MaximalSplitSize        = 49,
                                                MinimalSplitProbability = 0,
                                                DoIBLSplitting = True,
                                                SplitClusterAmbiguityMap= InDetKeys.SplitClusterAmbiguityMap())
ToolSvc += InDetMergedPixelsTool

from SiClusterizationTool.SiClusterizationToolConf import InDet__PixelGangedAmbiguitiesFinder
InDetPixelGangedAmbiguitiesFinder = InDet__PixelGangedAmbiguitiesFinder(name = "InDetPixelGangedAmbiguitiesFinder")
ToolSvc += InDetPixelGangedAmbiguitiesFinder

from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__PixelClusterization
InDetPixelClusterization = InDet__PixelClusterization(name                    = "InDetPixelClusterization",
                                                      clusteringTool          = InDetMergedPixelsTool,
                                                      gangedAmbiguitiesFinder = InDetPixelGangedAmbiguitiesFinder,
                                                      DetectorManagerName     = InDetKeys.PixelManager(),
                                                      DataObjectName          = InDetKeys.PixelRDOs(),
                                                      ClustersName            = "PixelTrigClusters",
                                                      isRoI_Seeded            = True)

if viewTest:
   InDetPixelClusterization.ClusterContainerCacheKey = topSequence.InDetCacheCreatorTrigViews.Pixel_ClusterKey

#
# --- SCT_ClusteringTool (public)
#
from SiClusterizationTool.SiClusterizationToolConf import InDet__SCT_ClusteringTool
InDetSCT_ClusteringTool = InDet__SCT_ClusteringTool(name              = "InDetSCT_ClusteringTool",
                                                    globalPosAlg      = InDetClusterMakerTool,
                                                    conditionsService = InDetSCT_ConditionsSummarySvc)
#
# --- SCT_Clusterization algorithm
#
from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__SCT_Clusterization
InDetSCT_Clusterization = InDet__SCT_Clusterization(name                    = "InDetSCT_Clusterization",
                                                    clusteringTool          = InDetSCT_ClusteringTool,
                                                    # ChannelStatus         = InDetSCT_ChannelStatusAlg,
                                                    DetectorManagerName     = InDetKeys.SCT_Manager(),
                                                    DataObjectName          = InDetKeys.SCT_RDOs(),
                                                    ClustersName            = "SCT_TrigClusters",
                                                    conditionsService       = InDetSCT_ConditionsSummarySvc,
                                                    FlaggedConditionService = InDetSCT_FlaggedConditionSvc, 
                                                    isRoI_Seeded            = True )

if viewTest:
   InDetSCT_Clusterization.ClusterContainerCacheKey = topSequence.InDetCacheCreatorTrigViews.SCT_ClusterKey

#Space points and FTF

from SiSpacePointTool.SiSpacePointToolConf import InDet__SiSpacePointMakerTool
InDetSiSpacePointMakerTool = InDet__SiSpacePointMakerTool(name = "InDetSiSpacePointMakerTool")
ToolSvc += InDetSiSpacePointMakerTool

from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiTrackerSpacePointFinder
InDetSiTrackerSpacePointFinder = InDet__SiTrackerSpacePointFinder(name                   = "InDetSiTrackerSpacePointFinder",
                                                                  SiSpacePointMakerTool  = InDetSiSpacePointMakerTool,
                                                                  PixelsClustersName     = "PixelTrigClusters",
                                                                  SCT_ClustersName       = "SCT_TrigClusters",
                                                                  SpacePointsPixelName   = "PixelTrigSpacePoints",
                                                                  SpacePointsSCTName     = "SCT_TrigSpacePoints",
                                                                  SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
                                                                  ProcessPixels          = DetFlags.haveRIO.pixel_on(),
                                                                  ProcessSCTs            = DetFlags.haveRIO.SCT_on(),
                                                                  ProcessOverlaps        = DetFlags.haveRIO.SCT_on(),
                                                                  OutputLevel=DEBUG)
if viewTest:
   InDetSiTrackerSpacePointFinder.SpacePointCacheSCT = topSequence.InDetCacheCreatorTrigViews.SpacePointCacheSCT
   InDetSiTrackerSpacePointFinder.SpacePointCachePix = topSequence.InDetCacheCreatorTrigViews.SpacePointCachePix

from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_eGamma
theFTF = TrigFastTrackFinder_eGamma()
theFTF.OutputLevel = DEBUG

from TrigInDetConf.TrigInDetRecCommonTools import InDetTrigFastTrackSummaryTool
from TrigInDetConf.TrigInDetPostTools import  InDetTrigParticleCreatorToolFTF

from InDetTrigParticleCreation.InDetTrigParticleCreationConf import InDet__TrigTrackingxAODCnvMT
theTrackParticleCreatorAlg = InDet__TrigTrackingxAODCnvMT(name = "InDetTrigTrackParticleCreatorAlg",
                                                         doIBLresidual = False,
                                                         TrackName = "TrigFastTrackFinder_Tracks",
                                                         TrackParticlesName = "xAODTracks",
                                                         ParticleCreatorTool = InDetTrigParticleCreatorToolFTF)

IDSequence = [ InDetPixelRawDataProvider, InDetSCTRawDataProvider, InDetTRTRawDataProvider, InDetPixelClusterization, InDetSCT_Clusterization, InDetSiTrackerSpacePointFinder, theFTF, theTrackParticleCreatorAlg ]


from TrigEgammaHypo.TrigL2ElectronFexMTConfig import L2ElectronFex_1
theElectronFex= L2ElectronFex_1()
theElectronFex.TrigEMClusterName = theFastCaloAlgo.ClustersName
theElectronFex.TrackParticlesName = theTrackParticleCreatorAlg.TrackParticlesName
theElectronFex.ElectronsName="Electrons"
theElectronFex.OutputLevel=VERBOSE


if viewTest:
  filterCaloRoIsAlg = RoRSeqFilter("filterCaloRoIsAlg")
  filterCaloRoIsAlg.Input = [theFastCaloHypo.Decisions]
  filterCaloRoIsAlg.Output = ["Filtered"+theFastCaloHypo.Decisions]
  filterCaloRoIsAlg.Chains = testChains
  filterCaloRoIsAlg.OutputLevel = DEBUG


  l2ElectronViewsMaker = EventViewCreatorAlgorithm("l2ElectronViewsMaker", OutputLevel=DEBUG)
  # topSequence += l2ElectronViewsMaker
  l2ElectronViewsMaker.Decisions = filterCaloRoIsAlg.Output[0] # output of L2CaloHypo
  l2ElectronViewsMaker.RoIsLink = "roi" # -||-
  l2ElectronViewsMaker.InViewRoIs = "EMIDRoIs" # contract with the fastCalo
  l2ElectronViewsMaker.Views = "EMElectronViews"
  l2ElectronViewsMaker.ViewFallThrough = True

  theTrackParticleCreatorAlg.roiCollectionName = l2ElectronViewsMaker.InViewRoIs
  for idAlg in IDSequence:
    if idAlg.properties().has_key("RoIs"):
      idAlg.RoIs = l2ElectronViewsMaker.InViewRoIs

    allViewAlgorithms += idAlg
    svcMgr.ViewAlgPool.TopAlg += [ idAlg.getName() ]
    l2ElectronViewsMaker.AlgorithmNameSequence += [ idAlg.getName() ]    


  theElectronFex.RoIs = l2ElectronViewsMaker.InViewRoIs
  allViewAlgorithms += theElectronFex
  svcMgr.ViewAlgPool.TopAlg += [ theElectronFex.getName() ]
  l2ElectronViewsMaker.AlgorithmNameSequence += [ theElectronFex.getName() ]    


  from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2ElectronHypoAlg
  from TrigEgammaHypo.TrigL2ElectronHypoTool import TrigL2ElectronHypoToolFromName
  theElectronHypo = TrigL2ElectronHypoAlg()
  theElectronHypo.Views = l2ElectronViewsMaker.Views
  theElectronHypo.Electrons = theElectronFex.ElectronsName
  theElectronHypo.ClusterDecisions = theFastCaloHypo.Decisions 
  theElectronHypo.ElectronDecisions = "ElectronL2Decisions"
  theElectronHypo.OutputLevel = VERBOSE
  theElectronHypo.HypoTools = [ TrigL2ElectronHypoToolFromName( c ) for c in testChains ]

  for t in theElectronHypo.HypoTools:
    t.OutputLevel = VERBOSE
  # topSequence += theElectronHypo
  electronDecisionsDumper = DumpDecisions("electronDecisionsDumper", OutputLevel=DEBUG, Decisions = theElectronHypo.ElectronDecisions )    
  egammaIDStep = stepSeq("egammaIDStep", filterCaloRoIsAlg, [ l2ElectronViewsMaker, theElectronHypo, electronDecisionsDumper ] )

else:
  # ID algs can't run w/o views yet
  pass


# CF construction



step0 = parOR("step0", [ egammaCaloStep ] )
step1 = parOR("step1", [ egammaIDStep ] )

from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg
summary = TriggerSummaryAlg( "TriggerSummaryAlg" )
summary.L1Decision = "HLTChains"
summary.FinalDecisions = [ "ElectronL2Decisions", "MuonL2Decisions" ]
summary.OutputLevel = DEBUG

steps = seqAND("HLTSteps", [ step0, step1, summary ]  )

mon = TriggerSummaryAlg( "TriggerMonitoringAlg" )
mon.L1Decision = "HLTChains"
mon.FinalDecisions = [ "ElectronL2Decisions", "MuonL2Decisions", "WhateverElse" ]
mon.HLTSummary = "MonitoringSummary"
mon.OutputLevel = DEBUG

hltTop = seqOR( "hltTop", [ steps, mon] )
topSequence += hltTop
  
