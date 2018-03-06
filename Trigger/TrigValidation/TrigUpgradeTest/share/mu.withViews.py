# 
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration 
# 
#  OutputLevel: INFO < DEBUG < VERBOSE 
# 
  
include("TrigUpgradeTest/testHLT_MT.py") 

### workaround to prevent online trigger folders to be enabled ###
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

### PixelLorentzAngleSvc and SCTLorentzAngleSvc ###
include("InDetRecExample/InDetRecConditionsAccess.py")

from InDetRecExample.InDetKeys import InDetKeys

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

### If inputFile is BS(ByteStream), the bool is true. ###
isData = False 
if globalflags.InputFormat.is_bytestream():
  isData = True

### provide a minimal menu information ###
if isData:
  topSequence.L1DecoderTest.ctpUnpacker.OutputLevel=DEBUG
  topSequence.L1DecoderTest.roiUnpackers[0].OutputLevel=DEBUG
  topSequence.L1DecoderTest.roiUnpackers[1].OutputLevel=DEBUG
else:
  pass
 
### for Desplaying StoreGate Dump ###
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.StoreGateSvc=Service("StoreGateSvc") 
ServiceMgr.StoreGateSvc.Dump=True 
 
### for Control Flow ###
from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq
 
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.CheckDependencies( True )
AlgScheduler.OutputLevel( DEBUG )
AlgScheduler.ShowDataDependencies( True )
AlgScheduler.setDataLoaderAlg( 'SGInputLoader' )


from AthenaCommon.CfgGetter import getPublicTool, getPublicToolClone
from AthenaCommon import CfgMgr

doL2SA=True
doL2CB=True
doEFSA=False


# ===============================================================================================
#               Setup PrepData                                                                    
# ===============================================================================================
 
### Used the algorithms as Step2 "muComb step" ###
if TriggerFlags.doID:
  from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__CacheCreator
  InDetCacheCreatorTrigViews = InDet__CacheCreator(name = "InDetCacheCreatorTrigViews",
                                                   Pixel_ClusterKey = "PixelTrigClustersCache",
                                                   SCT_ClusterKey   = "SCT_ClustersCache",
                                                   SpacePointCachePix = "PixelSpacePointCache",
                                                   SpacePointCacheSCT   = "SctSpacePointCache",
                                                   SCTRDOCacheKey       = "SctRDOCache",
                                                   PixRDOCacheKey = "PixRDOCache",
                                                   OutputLevel=DEBUG)
  ### Pixel ###
  from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRodDecoder
  InDetPixelRodDecoder = PixelRodDecoder(name = "InDetPixelRodDecoder")
  ToolSvc += InDetPixelRodDecoder
  
  from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRawDataProviderTool
  InDetPixelRawDataProviderTool = PixelRawDataProviderTool(name    = "InDetPixelRawDataProviderTool",
                                                           Decoder = InDetPixelRodDecoder)
  ToolSvc += InDetPixelRawDataProviderTool
  
  ### load the PixelRawDataProvider ###
  from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRawDataProvider
  InDetPixelRawDataProvider = PixelRawDataProvider(name         = "InDetPixelRawDataProvider",
                                                   RDOKey       = InDetKeys.PixelRDOs(),
                                                   ProviderTool = InDetPixelRawDataProviderTool,
                                                   RDOCacheKey  = InDetCacheCreatorTrigViews.PixRDOCacheKey,
                                                   isRoI_Seeded = True,
                                                   OutputLevel = INFO )
  
  ### SCT ###
  from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCT_RodDecoder
  InDetSCTRodDecoder = SCT_RodDecoder(name        = "InDetSCTRodDecoder",
                                      TriggerMode = False)
  ToolSvc += InDetSCTRodDecoder
  
  from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCTRawDataProviderTool
  InDetSCTRawDataProviderTool = SCTRawDataProviderTool(name    = "InDetSCTRawDataProviderTool",
                                                      Decoder = InDetSCTRodDecoder)
  ToolSvc += InDetSCTRawDataProviderTool
  
  ### load the SCTRawDataProvider###
  from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCTRawDataProvider
  InDetSCTRawDataProvider = SCTRawDataProvider(name         = "InDetSCTRawDataProvider",
                                               RDOKey       = InDetKeys.SCT_RDOs(),
                                               ProviderTool = InDetSCTRawDataProviderTool,
                                               isRoI_Seeded = True )
  
  InDetSCTRawDataProvider.RDOCacheKey = InDetCacheCreatorTrigViews.SCTRDOCacheKey
  
  ### TRT ###
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
  
    
  #### load the TRTRawDataProvider ###
  from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRTRawDataProvider
  InDetTRTRawDataProvider = TRTRawDataProvider(name         = "InDetTRTRawDataProvider",
                                               RDOKey       = "TRT_RDOs",
                                               ProviderTool = InDetTRTRawDataProviderTool,
                                               isRoI_Seeded = True
                                               )
  
  ### Pixel clusterisation ###
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
  
  
  InDetPixelClusterization.ClusterContainerCacheKey = InDetCacheCreatorTrigViews.Pixel_ClusterKey
  
  #
  # --- SCT_ClusteringTool (public)
  #
  from SiClusterizationTool.SiClusterizationToolConf import InDet__SCT_ClusteringTool
  InDetSCT_ClusteringTool = InDet__SCT_ClusteringTool(name              = "InDetSCT_ClusteringTool",
                                                      globalPosAlg      = InDetClusterMakerTool,
                                                      conditionsService = InDetSCT_ConditionsSummarySvcWithoutFlagged)
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
                                                      conditionsService       = InDetSCT_ConditionsSummarySvcWithoutFlagged,
                                                      isRoI_Seeded            = True )
  
  
  InDetSCT_Clusterization.ClusterContainerCacheKey = InDetCacheCreatorTrigViews.SCT_ClusterKey
  
  ### #Space points and FTF ###
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
  
  InDetSiTrackerSpacePointFinder.SpacePointCacheSCT = InDetCacheCreatorTrigViews.SpacePointCacheSCT
  InDetSiTrackerSpacePointFinder.SpacePointCachePix = InDetCacheCreatorTrigViews.SpacePointCachePix


  ### load TrigFastTrackFainder_Muon for muComb ###  
  from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Muon
  theFTF = TrigFastTrackFinder_Muon()
  theFTF.OutputLevel = DEBUG
  theFTF.TracksName = "TrigFastTrackFinder_MuTracks"
  
  ### A simple algorithm to confirm that data has been inherited from parent view ###
  ### Required to satisfy data dependencies                                       ###
  ViewVerify = CfgMgr.AthViews__ViewDataVerifier("muFastViewDataVerifier")
  ViewVerify.DataObjects = [('xAOD::L2StandAloneMuonContainer','StoreGateSvc+MuonL2SAInfo')]
  
  from TrigInDetConf.TrigInDetRecCommonTools import InDetTrigFastTrackSummaryTool
  from TrigInDetConf.TrigInDetPostTools import  InDetTrigParticleCreatorToolFTF
  
  from InDetTrigParticleCreation.InDetTrigParticleCreationConf import InDet__TrigTrackingxAODCnvMT
  theTrackParticleCreatorAlg = InDet__TrigTrackingxAODCnvMT(name = "InDetTrigTrackParticleCreatorAlg",
                                                           doIBLresidual = False,
                                                           TrackName = theFTF.TracksName,
                                                           TrackParticlesName = "xAODTracks",
                                                           ParticleCreatorTool = InDetTrigParticleCreatorToolFTF)

  IDSequence = [ InDetPixelRawDataProvider, InDetSCTRawDataProvider, InDetTRTRawDataProvider, InDetPixelClusterization, InDetSCT_Clusterization, InDetSiTrackerSpacePointFinder, theFTF, ViewVerify, theTrackParticleCreatorAlg ]
 

### Used the algorithms as Step1 "muFast step" ###
if TriggerFlags.doMuon:
  ### Load data from Muon detectors ###
  import MuonRecExample.MuonRecStandaloneOnlySetup
  from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
  muonRecFlags.doTrackPerformance    = True
  muonRecFlags.TrackPerfSummaryLevel = 2
  muonRecFlags.TrackPerfDebugLevel   = 5
  muonRecFlags.doCSCs                = True
  muonRecFlags.doNSWNewThirdChain    = False
  muonCombinedRecFlags.doCaloTrkMuId = False
  muonCombinedRecFlags.printSummary = False
  from RecExConfig.RecFlags import rec

  if doL2SA:
    from AthenaCommon.AlgSequence import AthSequencer
    l2MuViewNode = AthSequencer("l2MuViewNode", Sequential=False, ModeOR=False, StopOverride=False)
    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
    l2MuViewsMaker = EventViewCreatorAlgorithm("l2MuViewsMaker", OutputLevel=DEBUG)
    l2MuViewsMaker.ViewFallThrough = True
 
    l2MuViewsMaker.Decisions = "MURoIDecisions"
    l2MuViewsMaker.RoIsLink = "initialRoI" # -||-
    l2MuViewsMaker.InViewRoIs = "MURoIs" # contract with the consumer
    l2MuViewsMaker.Views = "MUViewRoIs"
    l2MuViewsMaker.ViewNodeName = l2MuViewNode.name()

  if doEFSA or doL2SA:
    ### ==================== Data prepartion needed for the EF and L2 SA #######################333
    ### CSC RDO data ###
    if muonRecFlags.doCSCs():
      from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscROD_Decoder
      CSCRodDecoder = Muon__CscROD_Decoder(name		= "CSCRodDecoder",
                                           IsCosmics		= False,
                                           IsOldCosmics 	= False,
                                           OutputLevel 	= INFO )
      ToolSvc += CSCRodDecoder
  
      from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CSC_RawDataProviderTool
      MuonCscRawDataProviderTool = Muon__CSC_RawDataProviderTool(name        = "MuonCscRawDataProviderTool",
                                                                 Decoder     = ToolSvc.CSCRodDecoder,
                                                                 OutputLevel = INFO )
      ToolSvc += MuonCscRawDataProviderTool
  
      from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscRdoToCscPrepDataTool
      CscRdoToCscPrepDataTool = Muon__CscRdoToCscPrepDataTool(name                = "CscRdoToPrepDataTool",
                                                              OutputLevel         = INFO,
                                                              RawDataProviderTool = MuonCscRawDataProviderTool,
                                                              useBStoRdoTool	    = True)
      ToolSvc += CscRdoToCscPrepDataTool
  
      from MuonRdoToPrepData.MuonRdoToPrepDataConf import CscRdoToCscPrepData
      CscRdoToCscPrepData = CscRdoToCscPrepData(name                    = "CscRdoToCscPrepDataProvider",
                                                CscRdoToCscPrepDataTool = CscRdoToCscPrepDataTool,
                                                PrintPrepData	    	  = False,
                                                OutputLevel             = INFO)
      CscRdoToCscPrepData.DoSeededDecoding = True
      CscRdoToCscPrepData.RoIs = "MURoIs"
   
      from MuonByteStream.MuonByteStreamConf import Muon__CscRawDataProvider
      CscRawDataProvider = Muon__CscRawDataProvider(name         = "CscRawDataProvider",
                                                    ProviderTool = MuonCscRawDataProviderTool,
                                                    OutputLevel  = INFO)

      from CscClusterization.CscClusterizationConf import CscThresholdClusterBuilderTool
      CscClusterBuilderTool = CscThresholdClusterBuilderTool(name        = "CscThesholdClusterBuilderTool",
                                                             OutputLevel = INFO)
      ToolSvc += CscClusterBuilderTool

      #CSC cluster building
      from CscClusterization.CscClusterizationConf import CscThresholdClusterBuilder
      CscClusterBuilder = CscThresholdClusterBuilder(name            = "CscThesholdClusterBuilder",
                                                     OutputLevel     = INFO,
                                                     cluster_builder = CscClusterBuilderTool)    
     
      if doEFSA: 
        topSequence += CscRdoToCscPrepData  
        topSequence += CscClusterBuilder 
      if doL2SA:
        l2MuViewNode += CscRawDataProvider
   
    ### MDT RDO data ###
    if muonRecFlags.doMDTs():
      from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import MdtROD_Decoder
      MDTRodDecoder = MdtROD_Decoder(name	   = "MDTRodDecoder",
                                     OutputLevel = INFO )
      ToolSvc += MDTRodDecoder
  
      from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MDT_RawDataProviderTool
      MuonMdtRawDataProviderTool = Muon__MDT_RawDataProviderTool(name        = "MuonMdtRawDataProviderTool",
                                                                 Decoder     = ToolSvc.MDTRodDecoder,
                                                                 OutputLevel = INFO )
      ToolSvc += MuonMdtRawDataProviderTool
  
      from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MdtRdoToPrepDataTool
      MdtRdoToMdtPrepDataTool = Muon__MdtRdoToPrepDataTool(name                = "MdtRdoToPrepDataTool",
                                                           OutputLevel         = INFO,
                                                           RawDataProviderTool = MuonMdtRawDataProviderTool,
                                                           useBStoRdoTool      = True)
      ToolSvc += MdtRdoToMdtPrepDataTool
  
      from MuonRdoToPrepData.MuonRdoToPrepDataConf import MdtRdoToMdtPrepData
      MdtRdoToMdtPrepData = MdtRdoToMdtPrepData(name          = "MdtRdoToMdtPrepDataProvider",
                                                DecodingTool  = MdtRdoToMdtPrepDataTool,
                                                PrintPrepData = False,
                                                OutputLevel   = INFO)  
      MdtRdoToMdtPrepData.DoSeededDecoding = True
      MdtRdoToMdtPrepData.RoIs = "MURoIs"
      
      from MuonByteStream.MuonByteStreamConf import Muon__MdtRawDataProvider
      MdtRawDataProvider = Muon__MdtRawDataProvider(name         = "MdtRawDataProvider",
                                                    ProviderTool = MuonMdtRawDataProviderTool,
                                                    OutputLevel  = INFO)
      if doEFSA:
        topSequence += MdtRdoToMdtPrepData
      if doL2SA:
        l2MuViewNode += MdtRawDataProvider

    ### RPC RDO data ###
    if muonRecFlags.doRPCs():
      from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcROD_Decoder
      RPCRodDecoder = Muon__RpcROD_Decoder(name	 = "RPCRodDecoder",
                                           OutputLevel = INFO )
      ToolSvc += RPCRodDecoder
  
      from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RPC_RawDataProviderTool
      MuonRpcRawDataProviderTool = Muon__RPC_RawDataProviderTool(name    = "MuonRpcRawDataProviderTool",
                                                                 Decoder = RPCRodDecoder )
      ToolSvc += MuonRpcRawDataProviderTool
  
      from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRdoToPrepDataTool
      RpcRdoToRpcPrepDataTool = Muon__RpcRdoToPrepDataTool(name                = "RpcRdoToPrepDataTool",
                                                           OutputLevel         = INFO,
                                                           RawDataProviderTool = MuonRpcRawDataProviderTool,
                                                           useBStoRdoTool      = True)
      ToolSvc += RpcRdoToRpcPrepDataTool
  
      from MuonRdoToPrepData.MuonRdoToPrepDataConf import RpcRdoToRpcPrepData
      RpcRdoToRpcPrepData = RpcRdoToRpcPrepData(name          = "RpcRdoToRpcPrepDataProvider",
                                                DecodingTool  = RpcRdoToRpcPrepDataTool,
                                                PrintPrepData = False,
                                                OutputLevel   = INFO)    
      RpcRdoToRpcPrepData.DoSeededDecoding = True
      RpcRdoToRpcPrepData.RoIs = "MURoIs"
  
      from MuonByteStream.MuonByteStreamConf import Muon__RpcRawDataProvider
      RpcRawDataProvider = Muon__RpcRawDataProvider(name         = "RpcRawDataProvider",
                                                    ProviderTool = MuonRpcRawDataProviderTool,
                                                    OutputLevel  = INFO)
      if doEFSA:
        topSequence += RpcRdoToRpcPrepData
      if doL2SA:
        l2MuViewNode += RpcRawDataProvider

    ### TGC RDO data ###
    if muonRecFlags.doTGCs():
      from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TGC_RodDecoderReadout
      TGCRodDecoder = Muon__TGC_RodDecoderReadout(name		= "TGCRodDecoder",
                                                  OutputLevel 	= INFO )
      ToolSvc += TGCRodDecoder
  
      from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TGC_RawDataProviderTool
      MuonTgcRawDataProviderTool = Muon__TGC_RawDataProviderTool(name    = "MuonTgcRawDataProviderTool",
                                                                 Decoder = TGCRodDecoder )
      ToolSvc += MuonTgcRawDataProviderTool
        
      from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TgcRdoToPrepDataTool
      TgcRdoToTgcPrepDataTool = Muon__TgcRdoToPrepDataTool(name                = "TgcRdoToPrepDataTool",
                                                           OutputLevel         = INFO,
                                                           RawDataProviderTool = MuonTgcRawDataProviderTool,
                                                           useBStoRdoTool      = True)
  
      ToolSvc += TgcRdoToTgcPrepDataTool

      from MuonRdoToPrepData.MuonRdoToPrepDataConf import TgcRdoToTgcPrepData
      TgcRdoToTgcPrepData = TgcRdoToTgcPrepData(name          = "TgcRdoToTgcPrepDataProvider",
                                                DecodingTool  = TgcRdoToTgcPrepDataTool,
                                                PrintPrepData = False,
                                                OutputLevel   = INFO)
      TgcRdoToTgcPrepData.DoSeededDecoding = True
      TgcRdoToTgcPrepData.RoIs = "MURoIs"


      from MuonByteStream.MuonByteStreamConf import Muon__TgcRawDataProvider
      TgcRawDataProvider = Muon__TgcRawDataProvider(name         = "TgcRawDataProvider",
                                                    ProviderTool = MuonTgcRawDataProviderTool,
                                                    OutputLevel  = INFO)
      if doEFSA:
        topSequence += TgcRdoToTgcPrepData
      if doL2SA:
        l2MuViewNode += TgcRawDataProvider

    
    #Run clustering
    if muonRecFlags.doCreateClusters():
      from MuonClusterization.MuonClusterizationConf import MuonClusterizationTool
      MuonClusterTool = MuonClusterizationTool(name        = "MuonClusterizationTool",
                                               OutputLevel = INFO)
      ToolSvc += MuonClusterTool
      
      from MuonClusterization.MuonClusterizationConf import MuonClusterizationAlg
      MuonClusterAlg = MuonClusterizationAlg(name                 = "MuonClusterizationAlg",
                                             OutputLevel          = INFO,
                                             ClusterTool          = MuonClusterTool,
                                             TgcPrepDataContainer = "TGC_MeasurementsAllBCs")
    
      if doEFSA:
        topSequence += MuonClusterAlg 


  ### muon thresholds ###
  testChains = ["HLT_mu6"]

  ### set up L1RoIsFilter ###
  from DecisionHandling.DecisionHandlingConf import RoRSeqFilter, DumpDecisions
  filterL1RoIsAlg = RoRSeqFilter("filterL1RoIsAlg")
  filterL1RoIsAlg.Input = ["MURoIDecisions"]
  filterL1RoIsAlg.Output = ["FilteredMURoIDecisions"]
  filterL1RoIsAlg.Chains = testChains
  filterL1RoIsAlg.OutputLevel = DEBUG


# ===============================================================================================
#               Setup L2MuonSA
# ===============================================================================================
 
  if doL2SA:
    ### set up MuFastSteering ###
    from TrigL2MuonSA.TrigL2MuonSAConfig import TrigL2MuonSAMTConfig
    muFastAlg = TrigL2MuonSAMTConfig("Muon")
    muFastAlg.OutputLevel = DEBUG

    svcMgr.ToolSvc.TrigDataAccess.ApplyOffsetCorrection = False

    l2MuViewNode += muFastAlg

    muFastAlg.MuRoIs = l2MuViewsMaker.InViewRoIs
    muFastAlg.RecMuonRoI = "RecMURoIs"
    muFastAlg.MuonL2SAInfo = "MuonL2SAInfo"
    muFastAlg.MuonCalibrationStream = "MuonCalibrationStream"
    muFastAlg.forID = "forID"
    muFastAlg.forMS = "forMS"

    # set up MuFastHypo
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMufastHypoConfig
    trigMufastHypo = TrigMufastHypoConfig("L2MufastHypoAlg")
    trigMufastHypo.OutputLevel = DEBUG

    trigMufastHypo.ViewRoIs = l2MuViewsMaker.Views
    trigMufastHypo.MuonL2SAInfoFromMuFastAlg = muFastAlg.MuonL2SAInfo
    trigMufastHypo.RoIs = l2MuViewsMaker.InViewRoIs
    trigMufastHypo.Decisions = "L2MuonFastDecisions"
    trigMufastHypo.L1Decisions = l2MuViewsMaker.Decisions

    trigMufastHypo.HypoTools = [ trigMufastHypo.TrigMufastHypoToolFromName( "L2MufastHypoTool", c ) for c in testChains ] 

    muFastDecisionsDumper = DumpDecisions("muFastDecisionsDumper", OutputLevel=DEBUG, Decisions = trigMufastHypo.Decisions )
    l2muFastSequence = seqAND("l2muFastSequence", [ l2MuViewsMaker, l2MuViewNode, trigMufastHypo ])

    muFastStep = stepSeq("muFastStep", filterL1RoIsAlg, [ l2muFastSequence,  muFastDecisionsDumper ] )


# ===============================================================================================
#               Setup muComb                                                                    
# ===============================================================================================
 
  if doL2CB:
    ### RoRSeqFilter step2 ###
    filterL2SAAlg = RoRSeqFilter("filterL2SAAlg")
    filterL2SAAlg.Input = [trigMufastHypo.Decisions]
    filterL2SAAlg.Output = ["Filtered"+trigMufastHypo.Decisions]
    filterL2SAAlg.Chains = testChains
    filterL2SAAlg.OutputLevel = DEBUG

    l2muCombViewNode = AthSequencer("l2muCombViewNode", Sequential=False, ModeOR=False, StopOverride=False)
    l2muCombViewsMaker = EventViewCreatorAlgorithm("l2muCombViewsMaker", OutputLevel=DEBUG)
    l2muCombViewsMaker.ViewFallThrough = True
 
    l2muCombViewsMaker.Decisions = filterL2SAAlg.Output[0] # Output of TrigMufastHypo
    l2muCombViewsMaker.RoIsLink = "roi" # -||-
    l2muCombViewsMaker.InViewRoIs = "MUTrkRoIs" # contract with the consumer
    l2muCombViewsMaker.Views = "MUTrkViewRoIs"
    l2muCombViewsMaker.ViewNodeName = l2muCombViewNode.name()

    ### Define input data of Inner Detector algorithms  ###
    ### and Define EventViewNodes to run the algprithms ###
    theTrackParticleCreatorAlg.roiCollectionName = l2muCombViewsMaker.InViewRoIs
    for idAlg in IDSequence:
      l2muCombViewNode += idAlg
      if idAlg.properties().has_key("RoIs"):
        idAlg.RoIs = l2muCombViewsMaker.InViewRoIs

    ### please read out TrigmuCombMTConfig file ###
    ### and set up to run muCombMT algorithm    ###
    from TrigmuComb.TrigmuCombMTConfig import TrigmuCombMTConfig
    muCombAlg = TrigmuCombMTConfig("Muon", theFTF.getName())
    muCombAlg.OutputLevel = DEBUG
    muCombAlg.L2StandAloneMuonContainerName = muFastAlg.MuonL2SAInfo
    muCombAlg.TrackParticlesContainerName = theTrackParticleCreatorAlg.TrackParticlesName
    muCombAlg.L2CombinedMuonContainerName = "MuonL2CBInfo"

    l2muCombViewNode += muCombAlg

    ### set up muCombHypo algorithm ###
    from TrigMuonHypo.TrigMuonHypoConfig import TrigmuCombHypoConfig
    trigmuCombHypo = TrigmuCombHypoConfig()
    trigmuCombHypo.OutputLevel = DEBUG
  
    trigmuCombHypo.Decisions = "MuonL2CBDecisions"
    trigmuCombHypo.L2MuonFastDecisions = trigMufastHypo.Decisions
    trigmuCombHypo.ViewRoIs = l2muCombViewsMaker.Views
    trigmuCombHypo.MuonL2CBInfoFromMuCombAlg = muCombAlg.L2CombinedMuonContainerName 
  
    trigmuCombHypo.HypoTools = [ trigmuCombHypo.TrigmuCombHypoToolFromName( name = "L2muCombHypoTool", nath = c ) for c in testChains ] 
  
    muCombDecisionsDumper = DumpDecisions("muCombDecisionsDumper", OutputLevel=DEBUG, Decisions = trigmuCombHypo.Decisions )
 
    ### Define a Sequence to run for muComb ### 
    l2muCombSequence = seqAND("l2muCombSequence", [ InDetCacheCreatorTrigViews, l2muCombViewsMaker, l2muCombViewNode, trigmuCombHypo ] )
    muCombStep = stepSeq("muCombStep", filterL2SAAlg, [ l2muCombSequence,  muCombDecisionsDumper ] )


# ===============================================================================================
#               Setup EFMuonSA                                                                    
# ===============================================================================================

  if doEFSA:
    from TrkDetDescrSvc.TrkDetDescrSvcConf import Trk__TrackingVolumesSvc
    ServiceMgr += Trk__TrackingVolumesSvc("TrackingVolumesSvc",BuildVolumesFromTagInfo = False)

    theSegmentFinder = CfgGetter.getPublicToolClone("MuonSegmentFinder","MooSegmentFinder")
    theSegmentFinder.DoSummary=True
    CfgGetter.getPublicTool("MuonLayerHoughTool").DoTruth=False
    theSegmentFinderAlg=CfgMgr.MooSegmentFinderAlg( "MuonSegmentMaker",
                                                    SegmentFinder=theSegmentFinder,
                                                    MuonSegmentOutputLocation = "MooreSegments",
                                                    UseCSC = muonRecFlags.doCSCs(),
                                                    UseMDT = muonRecFlags.doMDTs(),
                                                    UseRPC = muonRecFlags.doRPCs(),
                                                    UseTGC = muonRecFlags.doTGCs(),
                                                    doClusterTruth=False,
                                                    UseTGCPriorBC = False,
                                                    UseTGCNextBC  = False,
                                                    doTGCClust = muonRecFlags.doTGCClusterSegmentFinding(),
                                                    doRPCClust = muonRecFlags.doRPCClusterSegmentFinding(), OutputLevel=DEBUG )
    

 
    theNCBSegmentFinderAlg=CfgMgr.MooSegmentFinderAlg( "MuonSegmentMaker_NCB",
                                                       SegmentFinder = getPublicToolClone("MooSegmentFinder_NCB","MuonSegmentFinder",
                                                                                          DoSummary=False,
                                                                                          Csc2dSegmentMaker = getPublicToolClone("Csc2dSegmentMaker_NCB","Csc2dSegmentMaker",
                                                                                                                                 segmentTool = getPublicToolClone("CscSegmentUtilTool_NCB",
                                                                                                                                                                  "CscSegmentUtilTool",
                                                                                                                                                                  TightenChi2 = False, 
                                                                                                                                                                  IPconstraint=False)),
                                                                                          Csc4dSegmentMaker = getPublicToolClone("Csc4dSegmentMaker_NCB","Csc4dSegmentMaker",
                                                                                                                                 segmentTool = getPublicTool("CscSegmentUtilTool_NCB")),
                                                                                          DoMdtSegments=False,DoSegmentCombinations=False,DoSegmentCombinationCleaning=False),
                                                       MuonPatternCombinationLocation = "NCB_MuonHoughPatternCombinations", 
                                                       MuonSegmentOutputLocation = "NCB_MuonSegments", 
                                                       MuonSegmentCombinationOutputLocation = "NCB_MooreSegmentCombinations",
                                                       UseCSC = muonRecFlags.doCSCs(),
                                                       UseMDT = False,
                                                       UseRPC = False,
                                                       UseTGC = False,
                                                       UseTGCPriorBC = False,
                                                       UseTGCNextBC  = False,
                                                       doTGCClust = False,
                                                       doRPCClust = False)

    from MuonRecExample.MuonStandalone import MuonTrackSteering
    MuonTrackSteering.DoSummary=True
    MuonTrackSteering.DoSummary=DEBUG
    TrackBuilder = CfgMgr.MuPatTrackBuilder("MuPatTrackBuilder" )
    TrackBuilder.TrackSteering=CfgGetter.getPublicToolClone("MuonTrackSteering", "MuonTrackSteering")

    from AthenaCommon.Include import include
    include("InDetBeamSpotService/BeamCondSvc.py" )        
    from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg, xAODMaker__TrackCollectionCnvTool, xAODMaker__RecTrackParticleContainerCnvTool
  
    muonParticleCreatorTool = getPublicTool("MuonParticleCreatorTool")
  
    muonTrackCollectionCnvTool = xAODMaker__TrackCollectionCnvTool( name = "MuonTrackCollectionCnvTool", TrackParticleCreator = muonParticleCreatorTool )
  
    muonRecTrackParticleContainerCnvTool = xAODMaker__RecTrackParticleContainerCnvTool(name = "MuonRecTrackParticleContainerCnvTool", TrackParticleCreator = muonParticleCreatorTool )
 
    xAODTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg( name = "MuonStandaloneTrackParticleCnvAlg", 
                                                              TrackParticleCreator = muonParticleCreatorTool,
                                                              TrackCollectionCnvTool=muonTrackCollectionCnvTool,
                                                              RecTrackParticleContainerCnvTool = muonRecTrackParticleContainerCnvTool,
                                                              TrackContainerName = "MuonSpectrometerTracks",
                                                              xAODTrackParticlesFromTracksContainerName = "MuonSpectrometerTrackParticles",
                                                              ConvertTrackParticles = False,
                                                              ConvertTracks = True)


    thetrkbuilder = getPublicToolClone("CombinedMuonTrackBuilder_SA", "CombinedMuonTrackBuilder", MuonHoleRecovery="", CaloMaterialProvider='TMEF_TrkMaterialProviderTool')

    theCandidateTool = getPublicToolClone("MuonCandidateTool_SA", "MuonCandidateTool", TrackBuilder=thetrkbuilder)
    theMuonCandidateAlg=CfgMgr.MuonCombinedMuonCandidateAlg("MuonCandidateAlg",MuonCandidateTool=theCandidateTool)


    thecreatortool= getPublicToolClone("MuonCreatorTool_SA", "MuonCreatorTool", ScatteringAngleTool="", CaloMaterialProvider='TMEF_TrkMaterialProviderTool', MuonSelectionTool="", FillTimingInformation=False, OutputLevel=DEBUG)

    themuoncreatoralg = CfgMgr.MuonCreatorAlg("MuonCreatorAlg")
    themuoncreatoralg.MuonCreatorTool=thecreatortool
    themuoncreatoralg.CreateSAmuons=True


# ===============================================================================================
#               Setup CF(Control Flow)
# ===============================================================================================

  ### CF construction ###
if TriggerFlags.doMuon==True:    
  if doL2SA==True and doL2CB==False:
    from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg 
    summary = TriggerSummaryAlg( "TriggerSummaryAlg" ) 
    summary.InputDecision = "HLTChains" 
    summary.FinalDecisions = [ trigMufastHypo.Decisions ]
    summary.OutputLevel = DEBUG 
    step0 = parOR("step0", [ muFastStep ] )
    HLTsteps = seqAND("HLTsteps", [ step0, summary ]  ) 

    mon = TriggerSummaryAlg( "TriggerMonitoringAlg" ) 
    mon.InputDecision = "HLTChains" 
    mon.FinalDecisions = [ trigMufastHypo.Decisions, "WhateverElse" ] 
    mon.HLTSummary = "MonitoringSummary" 
    mon.OutputLevel = DEBUG 
    hltTop = seqOR( "hltTop", [ HLTsteps, mon] )

    topSequence += hltTop  

  if doEFSA:
    topSequence+=theSegmentFinderAlg
    #topSequence += theNCBSegmentFinderAlg #The configuration still needs some sorting out for this so disabled for now.
    topSequence +=TrackBuilder 
    topSequence +=xAODTrackParticleCnvAlg
    topSequence += theMuonCandidateAlg
    topSequence +=themuoncreatoralg

if TriggerFlags.doMuon==True and TriggerFlags.doID==True:    
  if doL2SA==True and doL2CB==True:
    from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg 
    summary = TriggerSummaryAlg( "TriggerSummaryAlg" ) 
    summary.InputDecision = "HLTChains" 
    summary.FinalDecisions = [ trigmuCombHypo.Decisions ]
    summary.OutputLevel = DEBUG 
    step0 = parOR("step0", [ muFastStep ] )
    step1 = parOR("step1", [ muCombStep ] )
    HLTsteps = seqAND("HLTsteps", [ step0, step1, summary ]  ) 

    mon = TriggerSummaryAlg( "TriggerMonitoringAlg" ) 
    mon.InputDecision = "HLTChains" 
    mon.FinalDecisions = [ trigmuCombHypo.Decisions, "WhateverElse" ] 
    mon.HLTSummary = "MonitoringSummary" 
    mon.OutputLevel = DEBUG 
    hltTop = seqOR( "hltTop", [ HLTsteps, mon] )

    topSequence += hltTop   

   
def TMEF_TrkMaterialProviderTool(name='TMEF_TrkMaterialProviderTool',**kwargs):
    from TrkMaterialProvider.TrkMaterialProviderConf import Trk__TrkMaterialProviderTool
    kwargs.setdefault("UseCaloEnergyMeasurement", False)
    return Trk__TrkMaterialProviderTool(name,**kwargs)
