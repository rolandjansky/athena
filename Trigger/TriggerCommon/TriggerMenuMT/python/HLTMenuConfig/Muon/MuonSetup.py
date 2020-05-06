#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
log = logging.getLogger('MuonSetup')

### Output data name ###
from TrigEDMConfig.TriggerEDMRun3 import recordable
from MuonConfig.MuonBytestreamDecodeConfig import MuonCacheNames
from MuonConfig.MuonRdoDecodeConfig import MuonPrdCacheNames

TrackParticlesName = recordable("HLT_IDTrack_Muon_FTF")
theFTF_name = "FTFTracks_Muons"
CBTPname = recordable("HLT_CBCombinedMuon_RoITrackParticles")
CBTPnameFS = recordable("HLT_CBCombinedMuon_FSTrackParticles")
ExtrpTPname = recordable("HLT_MSExtrapolatedMuons_RoITrackParticles")
ExtrpTPnameFS = recordable("HLT_MSExtrapolatedMuons_FSTrackParticles")
MSextrpTPname = recordable("HLT_MSOnlyExtrapolatedMuons_FSTrackParticles")
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.MuonSlice.doTrigMuonConfig=True

class muonNames(object):
  def __init__(self):
    #EFSA and EFCB containers have different names 
    #for RoI and FS running. Other containers are 
    #produced in RoIs only.

    self.L2SAName = recordable("HLT_MuonL2SAInfo")
    self.L2CBName = recordable("HLT_MuonL2CBInfo")
    self.EFSAName = "Muons"
    self.EFCBName = "MuonsCB"
    self.EFCBOutInName = recordable("HLT_MuonsCBOutsideIn")
    self.EFCBInOutName = "HLT_MuonsCBInsideOut"
    self.L2IsoMuonName = recordable("HLT_MuonL2ISInfo")
    self.EFIsoMuonName = recordable("HLT_MuonsIso")
    self.L2forIDName   = "RoIs_fromL2SAViews"

  def getNames(self, name):

    if "FS" in name:
      self.EFSAName = recordable("HLT_Muons_FS")
      self.EFCBName = recordable("HLT_MuonsCB_FS")
    if "RoI" in name:
      self.EFSAName = recordable("HLT_Muons_RoI")
      self.EFCBName = recordable("HLT_MuonsCB_RoI")
    return self


muNames = muonNames().getNames('RoI')
muNamesFS = muonNames().getNames('FS')


### ==================== Data prepartion needed for the EF and L2 SA ==================== ###
def makeMuonPrepDataAlgs(RoIs="MURoIs", forFullScan=False):

  from AthenaCommon.CFElements import parOR
  
  muDecodeRecoSequence = parOR("decodeMuViewNode_"+RoIs)

  postFix = "_"+RoIs

  viewAlgs_MuonPRD = []  # These algs should be executed to prepare muon PRDs for muFast and muEF steps.

  from AthenaCommon.AppMgr import ToolSvc

  ### CSC RDO data ###
  from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscROD_Decoder
  CSCRodDecoder = Muon__CscROD_Decoder(name		= "CscROD_Decoder",
                                       IsCosmics	= False,
                                       IsOldCosmics 	= False )
  ToolSvc += CSCRodDecoder

  from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CSC_RawDataProviderToolMT
  MuonCscRawDataProviderTool = Muon__CSC_RawDataProviderToolMT(name        = "CSC_RawDataProviderToolMT",
                                                               CscContainerCacheKey = MuonCacheNames.CscCache,
                                                               Decoder     = CSCRodDecoder )
  ToolSvc += MuonCscRawDataProviderTool

  from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscRdoToCscPrepDataToolMT
  CscRdoToCscPrepDataTool = Muon__CscRdoToCscPrepDataToolMT(name           = "CscRdoToCscPrepDataTool",
                                                            CscStripPrdContainerCacheKey = MuonPrdCacheNames.CscStripCache)
  
  ToolSvc += CscRdoToCscPrepDataTool

  from MuonRdoToPrepData.MuonRdoToPrepDataConf import CscRdoToCscPrepData
  CscRdoToCscPrepData = CscRdoToCscPrepData(name                    = "CscRdoToCscPrepData" + postFix,
                                            CscRdoToCscPrepDataTool = CscRdoToCscPrepDataTool,
                                            PrintPrepData           = False,
                                            DoSeededDecoding        = not forFullScan,
                                            RoIs                    = RoIs )


  from MuonByteStream.MuonByteStreamConf import Muon__CscRawDataProvider
  CscRawDataProvider = Muon__CscRawDataProvider(name         = "CscRawDataProvider" + postFix,
                                                ProviderTool = MuonCscRawDataProviderTool,
                                                DoSeededDecoding        = not forFullScan,
                                                RoIs                    = RoIs)

  from CscClusterization.CscClusterizationConf import CscThresholdClusterBuilderTool
  CscClusterBuilderTool = CscThresholdClusterBuilderTool(name        = "CscThresholdClusterBuilderTool" )
  ToolSvc += CscClusterBuilderTool

  #CSC cluster building
  from CscClusterization.CscClusterizationConf import CscThresholdClusterBuilder
  CscClusterBuilder = CscThresholdClusterBuilder(name            = "CscThresholdClusterBuilder",
                                                 cluster_builder = CscClusterBuilderTool)

  if globalflags.InputFormat.is_bytestream():
    viewAlgs_MuonPRD.append( CscRawDataProvider )
  viewAlgs_MuonPRD.append( CscRdoToCscPrepData )
  viewAlgs_MuonPRD.append( CscClusterBuilder )


  ### MDT RDO data ###
  from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import MdtROD_Decoder
  MDTRodDecoder = MdtROD_Decoder(name	   = "MdtROD_Decoder" )
  ToolSvc += MDTRodDecoder

  from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MDT_RawDataProviderToolMT
  MuonMdtRawDataProviderTool = Muon__MDT_RawDataProviderToolMT(name        = "MDT_RawDataProviderToolMT",
                                                               CsmContainerCacheKey = MuonCacheNames.MdtCsmCache,
                                                               Decoder     = MDTRodDecoder )
  ToolSvc += MuonMdtRawDataProviderTool

  from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MdtRdoToPrepDataToolMT
  MdtRdoToMdtPrepDataTool = Muon__MdtRdoToPrepDataToolMT(name                     = "MdtRdoToPrepDataTool",
                                                         MdtPrdContainerCacheKey = MuonPrdCacheNames.MdtCache)

  ToolSvc += MdtRdoToMdtPrepDataTool

  from MuonRdoToPrepData.MuonRdoToPrepDataConf import MdtRdoToMdtPrepData
  MdtRdoToMdtPrepData = MdtRdoToMdtPrepData(name             = "MdtRdoToMdtPrepData" + postFix,
                                            DecodingTool     = MdtRdoToMdtPrepDataTool,
                                            PrintPrepData    = False,
                                            DoSeededDecoding = not forFullScan,
                                            RoIs             = RoIs)

  from RegionSelector.RegSelToolConfig import makeRegSelTool_MDT
  MdtRdoToMdtPrepData.RegSel_MDT = makeRegSelTool_MDT()

  from MuonByteStream.MuonByteStreamConf import Muon__MdtRawDataProvider
  MdtRawDataProvider = Muon__MdtRawDataProvider(name         = "MdtRawDataProvider" + postFix,
                                                ProviderTool = MuonMdtRawDataProviderTool,
                                                DoSeededDecoding = not forFullScan,
                                                RoIs = RoIs
                                                )

  if globalflags.InputFormat.is_bytestream():
    viewAlgs_MuonPRD.append( MdtRawDataProvider )
  viewAlgs_MuonPRD.append( MdtRdoToMdtPrepData )


  ### RPC RDO data ###
  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcROD_Decoder
  RPCRodDecoder = Muon__RpcROD_Decoder(name	     = "RpcROD_Decoder" )
  ToolSvc += RPCRodDecoder

  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RPC_RawDataProviderToolMT
  MuonRpcRawDataProviderTool = Muon__RPC_RawDataProviderToolMT(name    = "RPC_RawDataProviderToolMT",
                                                               RpcContainerCacheKey = MuonCacheNames.RpcCache,
                                                               WriteOutRpcSectorLogic = False, # we don't need the RPC sector logic when running the trigger and can't write it out if we want to use the IDC cache for the RDOs
                                                               Decoder = RPCRodDecoder )
  ToolSvc += MuonRpcRawDataProviderTool

  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRdoToPrepDataTool
  RpcRdoToRpcPrepDataTool = Muon__RpcRdoToPrepDataTool(name                = "RpcRdoToPrepDataTool")

  #from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRdoToPrepDataToolMT
  #RpcRdoToRpcPrepDataTool = Muon__RpcRdoToPrepDataToolMT(name                = "RpcRdoToPrepDataTool",
  #                                                       RpcPrdContainerCacheKey = MuonPrdCacheNames.RpcCache,
  #                                                       RpcCoinContainerCacheKey = MuonPrdCacheNames.RpcCoinCache)
  #RpcRdoToRpcPrepDataTool.OutputLevel = DEBUG

  if athenaCommonFlags.isOnline: 
      RpcRdoToRpcPrepDataTool.ReadKey = ""

  ToolSvc += RpcRdoToRpcPrepDataTool

  from MuonRdoToPrepData.MuonRdoToPrepDataConf import RpcRdoToRpcPrepData
  RpcRdoToRpcPrepData = RpcRdoToRpcPrepData(name             = "RpcRdoToRpcPrepData" + postFix,
                                            DecodingTool     = RpcRdoToRpcPrepDataTool,
                                            PrintPrepData    = False,
                                            DoSeededDecoding = not forFullScan,
                                            RoIs             = RoIs)

  from MuonByteStream.MuonByteStreamConf import Muon__RpcRawDataProvider
  RpcRawDataProvider = Muon__RpcRawDataProvider(name         = "RpcRawDataProvider" + postFix,
                                                ProviderTool = MuonRpcRawDataProviderTool,
                                                DoSeededDecoding = not forFullScan,
                                                RoIs = RoIs)

  if globalflags.InputFormat.is_bytestream():
    viewAlgs_MuonPRD.append( RpcRawDataProvider )
  viewAlgs_MuonPRD.append( RpcRdoToRpcPrepData )


  ### TGC RDO data ###
  from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TGC_RodDecoderReadout
  TGCRodDecoder = Muon__TGC_RodDecoderReadout(name	    = "TGC_RodDecoderReadout" )
  ToolSvc += TGCRodDecoder

  from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TGC_RawDataProviderToolMT
  MuonTgcRawDataProviderTool = Muon__TGC_RawDataProviderToolMT(name    = "TGC_RawDataProviderToolMT",
                                                               TgcContainerCacheKey = MuonCacheNames.TgcCache,
                                                               Decoder = TGCRodDecoder )
  ToolSvc += MuonTgcRawDataProviderTool

  from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TgcRdoToPrepDataTool
  TgcRdoToTgcPrepDataTool = Muon__TgcRdoToPrepDataTool(name                = "TgcRdoToPrepDataTool")
  ToolSvc += TgcRdoToTgcPrepDataTool

  from MuonRdoToPrepData.MuonRdoToPrepDataConf import TgcRdoToTgcPrepData
  TgcRdoToTgcPrepData = TgcRdoToTgcPrepData(name             = "TgcRdoToTgcPrepData" + postFix,
                                            DecodingTool     = TgcRdoToTgcPrepDataTool,
                                            PrintPrepData    = False,
                                            DoSeededDecoding = not forFullScan,
                                            RoIs             = RoIs)

  from MuonByteStream.MuonByteStreamConf import Muon__TgcRawDataProvider
  TgcRawDataProvider = Muon__TgcRawDataProvider(name         = "TgcRawDataProvider" + postFix,                                                
                                                ProviderTool = MuonTgcRawDataProviderTool,
                                                DoSeededDecoding = not forFullScan,
                                                RoIs             = RoIs )

  if globalflags.InputFormat.is_bytestream():
    viewAlgs_MuonPRD.append( TgcRawDataProvider )
  viewAlgs_MuonPRD.append( TgcRdoToTgcPrepData )

  from MuonRecExample.MuonRecFlags import muonRecFlags

  if muonRecFlags.doCreateClusters():
    #Run clustering
    from MuonClusterization.MuonClusterizationConf import MuonClusterizationTool
    MuonClusterTool = MuonClusterizationTool(name        = "MuonClusterizationTool" )
    ToolSvc += MuonClusterTool

    from MuonClusterization.MuonClusterizationConf import MuonClusterizationAlg
    MuonClusterAlg = MuonClusterizationAlg(name                 = "MuonClusterizationAlg",
                                           ClusterTool          = MuonClusterTool,
                                           TgcPrepDataContainer = "TGC_MeasurementsAllBCs")

    viewAlgs_MuonPRD.append( MuonClusterAlg )

  muDecodeRecoSequence += viewAlgs_MuonPRD
  return muDecodeRecoSequence


def muFastRecoSequence( RoIs ):

  from AthenaCommon.AppMgr import ToolSvc
  from AthenaCommon.CFElements import parOR

  muFastRecoSequence = parOR("l2MuViewNode")


  # Configure the L2 CSC data preparator - we can turn off the data decoding here
  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__CscDataPreparator
  L2CscDataPreparator = TrigL2MuonSA__CscDataPreparator(name = "L2MuonSACscDataPreparator",
                                                        DecodeBS = False,
                                                        DoDecoding = False,
                                                        CscRawDataProvider   = "",
                                                        CscPrepDataProvider  = "",
                                                        CscClusterProvider   = "")
  ToolSvc += L2CscDataPreparator
 
  # Configure the L2 MDT data preparator - we can turn off the data decoding here
  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__MdtDataPreparator
  L2MdtDataPreparator = TrigL2MuonSA__MdtDataPreparator(name = "L2MuonSAMdtDataPreparator",
                                                        DecodeBS = False,
                                                        DoDecoding = False,
                                                        MDT_RawDataProvider = "",
                                                        MdtPrepDataProvider = "")


  ### RPC RDO data - turn off the data decoding here ###
  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__RpcDataPreparator
  L2RpcDataPreparator = TrigL2MuonSA__RpcDataPreparator(name = "L2MuonSARpcDataPreparator",
                                                        RpcPrepDataProvider  = "",
                                                        RpcPrepDataContainer = "RPC_Measurements",
                                                        RpcRawDataProvider   = "",
                                                        DoDecoding           = False,
                                                        DecodeBS             = False)
  ToolSvc += L2RpcDataPreparator


  ### TGC data preparation - turn off the data decoding here ###
  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__TgcDataPreparator
  L2TgcDataPreparator = TrigL2MuonSA__TgcDataPreparator(name = "L2MuonSATgcDataPreparator",
                                                        DecodeBS = False,
                                                        DoDecoding = False,
                                                        TgcPrepDataProvider  = "",
                                                        TgcRawDataProvider = "")

  ### set up MuFastSteering ###
  from TrigL2MuonSA.TrigL2MuonSAConfig import TrigL2MuonSAMTConfig
  muFastAlg = TrigL2MuonSAMTConfig("Muon")

  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__MuFastDataPreparator
  MuFastDataPreparator = TrigL2MuonSA__MuFastDataPreparator()
  MuFastDataPreparator.CSCDataPreparator = L2CscDataPreparator
  MuFastDataPreparator.MDTDataPreparator = L2MdtDataPreparator
  MuFastDataPreparator.RPCDataPreparator = L2RpcDataPreparator
  MuFastDataPreparator.TGCDataPreparator = L2TgcDataPreparator
  
  muFastAlg.DataPreparator = MuFastDataPreparator

  muFastAlg.RecMuonRoI = "HLT_RecMURoIs"
  muFastAlg.MuRoIs = RoIs
  muFastAlg.MuonL2SAInfo = muNames.L2SAName
  muFastAlg.MuonCalibrationStream = "MuonCalibrationStream"
  muFastAlg.forID = muNames.L2forIDName
  muFastAlg.forMS = "forMS" 

  muFastRecoSequence += muFastAlg
  sequenceOut = muFastAlg.MuonL2SAInfo

  return muFastRecoSequence, sequenceOut

def muonIDFastTrackingSequence( RoIs, name ):

  # ATR-20453
  # Until such time as FS and RoI collections do not interfere, a hacky fix
  #from AthenaCommon.CFElements import parOR
  from AthenaCommon.CFElements import seqAND

  viewNodeName=name+"FastIDViewNode"

  # ATR-20453
  # Until such time as FS and RoI collections do not interfere, a hacky fix
  #muonIDFastTrackingSequence = parOR(viewNodeName)
  muonIDFastTrackingSequence = seqAND(viewNodeName)

  ### Define input data of Inner Detector algorithms  ###
  ### and Define EventViewNodes to run the algorithms ###
  from TrigInDetConfig.InDetSetup import makeInDetAlgs
  viewAlgs = makeInDetAlgs(whichSignature="Muon"+name, rois = RoIs)


  for viewAlg in viewAlgs:
      muonIDFastTrackingSequence += viewAlg

  return muonIDFastTrackingSequence

def muCombRecoSequence( RoIs, name ):

  from AthenaCommon.CFElements import parOR
  muCombRecoSequence = parOR("l2muCombViewNode")
  ### A simple algorithm to confirm that data has been inherited from parent view ###
  ### Required to satisfy data dependencies                                       ###
  import AthenaCommon.CfgMgr as CfgMgr
  ViewVerify = CfgMgr.AthViews__ViewDataVerifier("muFastViewDataVerifier")
  ViewVerify.DataObjects = [('xAOD::L2StandAloneMuonContainer','StoreGateSvc+'+muNames.L2SAName)]
  muCombRecoSequence+=ViewVerify

  ### please read out TrigmuCombMTConfig file ###
  ### and set up to run muCombMT algorithm    ###
  from TrigmuComb.TrigmuCombMTConfig import TrigmuCombMTConfig
  muCombAlg = TrigmuCombMTConfig("Muon", name)
  muCombAlg.L2StandAloneMuonContainerName = muNames.L2SAName
  muCombAlg.TrackParticlesContainerName = TrackParticlesName
  muCombAlg.L2CombinedMuonContainerName = muNames.L2CBName

  muCombRecoSequence += muCombAlg
  sequenceOut = muCombAlg.L2CombinedMuonContainerName

  return muCombRecoSequence, sequenceOut


def l2muisoRecoSequence( RoIs ):

  import AthenaCommon.CfgMgr as CfgMgr
  from AthenaCommon.CFElements import parOR

  l2muisoRecoSequence = parOR("l2muIsoViewNode")

  ViewVerify = CfgMgr.AthViews__ViewDataVerifier("muCombViewDataVerifier")
  ViewVerify.DataObjects = [('xAOD::TrackParticleContainer' , 'StoreGateSvc+'+TrackParticlesName),
                            ('xAOD::L2CombinedMuonContainer','StoreGateSvc+'+muNames.L2CBName)]

  l2muisoRecoSequence += ViewVerify

  # set up algs
  from TrigmuIso.TrigmuIsoConfig import TrigmuIsoMTConfig
  trigL2muIso = TrigmuIsoMTConfig("TrigL2muIso")
  trigL2muIso.MuonL2CBInfoName = muNames.L2CBName
  trigL2muIso.TrackParticlesName = TrackParticlesName
  trigL2muIso.MuonL2ISInfoName = muNames.L2IsoMuonName

  l2muisoRecoSequence += trigL2muIso

  sequenceOut = trigL2muIso.MuonL2ISInfoName

  return l2muisoRecoSequence, sequenceOut


def muEFSARecoSequence( RoIs, name ):

  import AthenaCommon.CfgGetter as CfgGetter

  from AthenaCommon import CfgMgr
  from AthenaCommon.CFElements import parOR
  from MuonRecExample.MuonStandalone import MooSegmentFinderAlg, MuonStandaloneTrackParticleCnvAlg
  from MuonCombinedRecExample.MuonCombinedAlgs import MuonCombinedMuonCandidateAlg, MuonCreatorAlg
  from MuonCombinedAlgs.MuonCombinedAlgsMonitoring import MuonCreatorAlgMonitoring

  muEFSARecoSequence = parOR("efmsViewNode_"+name)

  efAlgs = []

  if name != 'FS':
    # we now try to share the data preparation algorithms with L2, so we tell the view that it should expect the MDT, TGC, CSC and RPC PRDs to be available
    efAlgs.append( CfgMgr.AthViews__ViewDataVerifier(name = "EFMuonViewDataVerifier",
                                                     DataObjects = [( 'Muon::MdtPrepDataContainer'      , 'StoreGateSvc+MDT_DriftCircles' ),
                                                                    ( 'Muon::TgcPrepDataContainer'      , 'StoreGateSvc+TGC_Measurements' ),
                                                                    ( 'Muon::RpcPrepDataContainer'      , 'StoreGateSvc+RPC_Measurements' ),
                                                                    ( 'Muon::CscStripPrepDataContainer' , 'StoreGateSvc+CSC_Measurements'),
                                                                    ( 'Muon::CscPrepDataContainer'      , 'StoreGateSvc+CSC_Clusters') ] ))
   
  #need MdtCondDbAlg for the MuonStationIntersectSvc (required by segment and track finding)
  from AthenaCommon.AlgSequence import AthSequencer
  from MuonCondAlg.MuonTopCondAlgConfigRUN2 import MdtCondDbAlg
  if not athenaCommonFlags.isOnline:
    condSequence = AthSequencer("AthCondSeq")
    if not hasattr(condSequence,"MdtCondDbAlg"):
      condSequence += MdtCondDbAlg("MdtCondDbAlg")
    # Sets up and configures the muon alignment:
    from MuonRecExample import MuonAlignConfig # noqa: F401


  theSegmentFinderAlg = MooSegmentFinderAlg("TrigMuonSegmentMaker_"+name)
  from MuonSegmentTrackMaker.MuonTrackMakerAlgsMonitoring import MuPatTrackBuilderMonitoring
  TrackBuilder = CfgMgr.MuPatTrackBuilder("TrigMuPatTrackBuilder_"+name ,MuonSegmentCollection = "MuonSegments", 
                                          TrackSteering=CfgGetter.getPublicToolClone("TrigMuonTrackSteering", "MuonTrackSteering"), 
                                          MonTool = MuPatTrackBuilderMonitoring("MuPatTrackBuilderMonitoringSA_"+name))
  xAODTrackParticleCnvAlg = MuonStandaloneTrackParticleCnvAlg("TrigMuonStandaloneTrackParticleCnvAlg_"+name)
  theMuonCandidateAlg=MuonCombinedMuonCandidateAlg("TrigMuonCandidateAlg_"+name)

  msMuonName = muNames.EFSAName
  if 'FS' in name:
    msMuonName = muNamesFS.EFSAName

  themuoncreatoralg = MuonCreatorAlg("TrigMuonCreatorAlg_"+name, CreateSAmuons=True, TagMaps=[], MuonContainerLocation=msMuonName,
                                     ExtrapolatedLocation = "HLT_MSExtrapolatedMuons_"+name, MSOnlyExtrapolatedLocation = "HLT_MSOnlyExtrapolatedMuons_"+name,
                                     MuonCreatorTool = CfgGetter.getPrivateToolClone("TrigMuonCreatorToolSA", "MuonCreatorTool", TrackSummaryTool = CfgGetter.getPublicTool("MuonTrackSummaryTool")),
                                     MonTool = MuonCreatorAlgMonitoring("MuonCreatorAlgSA_"+name))


  #Algorithms to views
  efAlgs.append( theSegmentFinderAlg )
  efAlgs.append( TrackBuilder )
  efAlgs.append( xAODTrackParticleCnvAlg )
  efAlgs.append( theMuonCandidateAlg )
  efAlgs.append( themuoncreatoralg )


  # setup muEFMsonly algs
  for efAlg in efAlgs:
      if "RoIs" in efAlg.properties():
        if name == "FS":
          from L1Decoder.L1DecoderConfig import mapThresholdToL1RoICollection 
          efAlg.RoIs = mapThresholdToL1RoICollection("FS")
        else:
          efAlg.RoIs = RoIs
      muEFSARecoSequence += efAlg
  sequenceOut = themuoncreatoralg.MuonContainerLocation

  return muEFSARecoSequence, sequenceOut




def muEFCBRecoSequence( RoIs, name ):

  from AthenaCommon import CfgMgr
  from AthenaCommon.CFElements import parOR
  from MuonCombinedRecExample.MuonCombinedAlgs import MuonCombinedInDetCandidateAlg, MuonCombinedAlg, MuonCreatorAlg
  from MuonCombinedAlgs.MuonCombinedAlgsMonitoring import MuonCreatorAlgMonitoring

  efAlgs = []
  muEFCBRecoSequence = parOR("efcbViewNode_"+name)
  #Need ID tracking related objects and MS tracks from previous steps
  ViewVerifyMS = CfgMgr.AthViews__ViewDataVerifier("muonCBViewDataVerifier")
  ViewVerifyMS.DataObjects = [( 'Muon::CscStripPrepDataContainer' , 'StoreGateSvc+CSC_Measurements' ),  
                              ( 'Muon::MdtPrepDataContainer' , 'StoreGateSvc+MDT_DriftCircles' ),  
                              ( 'Muon::TgcPrepDataContainer'      , 'StoreGateSvc+TGC_Measurements' ),
                              ( 'Muon::RpcPrepDataContainer'      , 'StoreGateSvc+RPC_Measurements' ),
                              ( 'MuonCandidateCollection' , 'StoreGateSvc+MuonCandidates') ]

  muEFCBRecoSequence += ViewVerifyMS
  if "FS" in name:
    #Need to run tracking for full scan chains
    from TrigInDetConfig.InDetSetup import makeInDetAlgs
    viewAlgs = makeInDetAlgs(whichSignature = "MuonFS", rois = RoIs) 

    for viewAlg in viewAlgs:
      muEFCBRecoSequence += viewAlg
      if "InDetTrigTrackParticleCreatorAlg" in viewAlg.name():
        TrackCollection = viewAlg.TrackName

  else:
    TrackCollection="TrigFastTrackFinder_Tracks_Muon" # this is hacking, please FIX IT
    ViewVerifyTrk = CfgMgr.AthViews__ViewDataVerifier("muonCBIDViewDataVerifier")
    ViewVerifyTrk.DataObjects = [( 'xAOD::TrackParticleContainer' , 'StoreGateSvc+'+TrackParticlesName ),
                                 ( 'TrackCollection' , 'StoreGateSvc+'+TrackCollection ),
                                 ( 'SCT_FlaggedCondData' , 'StoreGateSvc+SCT_FlaggedCondData_TRIG' ),
                                 ( 'xAOD::IParticleContainer' , 'StoreGateSvc+'+TrackParticlesName )]

    if globalflags.InputFormat.is_bytestream():
      ViewVerifyTrk.DataObjects += [( 'InDetBSErrContainer' , 'StoreGateSvc+PixelByteStreamErrs' ),
                                    ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' ) ]
    muEFCBRecoSequence += ViewVerifyTrk


  #Precision Tracking
  PTAlgs = [] #List of precision tracking algs
  PTTracks = [] #List of TrackCollectionKeys
  PTTrackParticles = [] #List of TrackParticleKeys

  from TrigInDetConfig.InDetPT import makeInDetPrecisionTracking
  #When run in a different view than FTF some data dependencies needs to be loaded through verifier
  #Pass verifier as an argument and it will automatically append necessary DataObjects
  #@NOTE: Don't provide any verifier if loaded in the same view as FTF
  if 'FS' in name:
    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( "muonFS", rois = RoIs, verifier = False, inputFTFtracks=TrackCollection)
    PTSeq = parOR("precisionTrackingInMuonsFS", PTAlgs  )
  else:
    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( "muon", rois = RoIs,  verifier = ViewVerifyTrk, inputFTFtracks= TrackCollection)
    PTSeq = parOR("precisionTrackingInMuons", PTAlgs  )
  #Get last tracks from the list as input for other alg

  muEFCBRecoSequence += PTSeq


  #Default from FTF
  #trackParticles = "IDTrack"
  #TODO: change according to what needs to be done here
  #Last key in the list is for the TrackParticles after all PT stages (so far only one :) )
  trackParticles = PTTrackParticles[-1]

  #Make InDetCandidates
  theIndetCandidateAlg = MuonCombinedInDetCandidateAlg("TrigMuonCombinedInDetCandidateAlg_"+name,TrackParticleLocation = [trackParticles],ForwardParticleLocation=trackParticles, 
                                                       InDetCandidateLocation="InDetCandidates_"+name)
  #No easy way to access AtlasHoleSearchTool in theIndetCandidateAlg
  from AthenaCommon.AppMgr import ToolSvc
  from InDetTrigRecExample.InDetTrigConditionsAccess import SCT_ConditionsSetup
  from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_ConditionsSummaryTool
  ToolSvc.AtlasHoleSearchTool.SctSummaryTool = SCT_ConditionsSummaryTool(SCT_ConditionsSetup.instanceName('InDetSCT_ConditionsSummaryToolWithoutFlagged'))

  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigSCTConditionsSummaryTool
  ToolSvc.CombinedMuonIDHoleSearch.SctSummaryTool = InDetTrigSCTConditionsSummaryTool

  #MS ID combination
  candidatesName = "MuonCandidates"
  if 'FS' in name:
    candidatesName = "MuonCandidates_FS"
  theMuonCombinedAlg = MuonCombinedAlg("TrigMuonCombinedAlg_"+name, MuonCandidateLocation=candidatesName, InDetCandidateLocation="InDetCandidates_"+name)


  cbMuonName = muNames.EFCBOutInName
  if 'FS' in name:
    cbMuonName = muNamesFS.EFCBName

  themuoncbcreatoralg = MuonCreatorAlg("TrigMuonCreatorAlgCB_"+name, MuonCandidateLocation=candidatesName, TagMaps=["muidcoTagMap"], InDetCandidateLocation="InDetCandidates_"+name,
                                       MuonContainerLocation = cbMuonName, SegmentContainerName = "CBSegments", ExtrapolatedLocation = "CBExtrapolatedMuons",
                                       MSOnlyExtrapolatedLocation = "CBMSonlyExtrapolatedMuons", CombinedLocation = "HLT_CBCombinedMuon_"+name,
                                       MonTool = MuonCreatorAlgMonitoring("MuonCreatorAlgCB_"+name))

  #Add all algorithms
  efAlgs.append(theIndetCandidateAlg)
  efAlgs.append(theMuonCombinedAlg)
  efAlgs.append(themuoncbcreatoralg)


  # setup muEFMsonly algs
  for efAlg in efAlgs:
    muEFCBRecoSequence += efAlg
  sequenceOut = themuoncbcreatoralg.MuonContainerLocation


  return muEFCBRecoSequence, sequenceOut


def muEFInsideOutRecoSequence(RoIs, name):

  from AthenaCommon.CFElements import parOR
  from AthenaCommon import CfgMgr

  from MuonRecExample.MuonStandalone import MooSegmentFinderAlg
  from MuonCombinedRecExample.MuonCombinedAlgs import MuonCombinedInDetCandidateAlg, MuonInsideOutRecoAlg, MuGirlStauAlg, MuonCreatorAlg, StauCreatorAlg
  from MuonCombinedAlgs.MuonCombinedAlgsMonitoring import MuonCreatorAlgMonitoring

  efAlgs = []

  viewNodeName="efmuInsideOutViewNode"
  if "Late" in name:
    viewNodeName = "latemuInsideOutViewNode"
  efmuInsideOutRecoSequence = parOR(viewNodeName)

  if "Late" in name:

    #Need to run hough transform at start of late muon chain
    #need MdtCondDbAlg for the MuonStationIntersectSvc (required by segment and track finding)
    from AthenaCommon.AlgSequence import AthSequencer
    from MuonCondAlg.MuonTopCondAlgConfigRUN2 import MdtCondDbAlg
    if not athenaCommonFlags.isOnline:
      condSequence = AthSequencer("AthCondSeq")
      if not hasattr(condSequence,"MdtCondDbAlg"):
        condSequence += MdtCondDbAlg("MdtCondDbAlg")
      # Sets up and configures the muon alignment:
      from MuonRecExample import MuonAlignConfig # noqa: F401

    theSegmentFinderAlg = MooSegmentFinderAlg("TrigLateMuonSegmentMaker_"+name)
    efAlgs.append(theSegmentFinderAlg)

    # need to run precisions tracking for late muons, since we don't run it anywhere else
    TrackCollection="TrigFastTrackFinder_Tracks_MuonLate" 

    #Precision Tracking
    PTAlgs = [] #List of precision tracking algs
    PTTracks = [] #List of TrackCollectionKeys
    PTTrackParticles = [] #List of TrackParticleKeys

    from TrigInDetConfig.InDetPT import makeInDetPrecisionTracking
    #When run in a different view than FTF some data dependencies needs to be loaded through verifier
    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( "muonLate", rois=RoIs, inputFTFtracks= TrackCollection)
    PTSeq = parOR("precisionTrackingInLateMuons", PTAlgs  )

    efmuInsideOutRecoSequence += PTSeq
    trackParticles = PTTrackParticles[-1]

    #Make InDetCandidates
    theIndetCandidateAlg = MuonCombinedInDetCandidateAlg("TrigMuonCombinedInDetCandidateAlg_"+name,TrackParticleLocation = [trackParticles],ForwardParticleLocation=trackParticles, 
                                                         InDetCandidateLocation="InDetCandidates_"+name)
    from AthenaCommon.AppMgr import ToolSvc
    from InDetTrigRecExample.InDetTrigConditionsAccess import SCT_ConditionsSetup
    from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_ConditionsSummaryTool
    ToolSvc.CombinedMuonIDHoleSearch.SctSummaryTool = SCT_ConditionsSummaryTool(SCT_ConditionsSetup.instanceName('InDetSCT_ConditionsSummaryToolWithoutFlagged'))

    efAlgs.append(theIndetCandidateAlg)


  else:
    # for non-latemu chains, the decoding/hough transform is run in an earlier step
    #Need PRD containers for inside-out reco
    ViewVerifyInsideOut = CfgMgr.AthViews__ViewDataVerifier("muonInsideOutViewDataVerifier")
    ViewVerifyInsideOut.DataObjects = [( 'Muon::CscPrepDataContainer' , 'StoreGateSvc+CSC_Clusters' ),
                                       ( 'Muon::RpcPrepDataContainer' , 'StoreGateSvc+RPC_Measurements' ),
                                       ( 'Muon::TgcPrepDataContainer' , 'StoreGateSvc+TGC_Measurements' ),
                                       ( 'Muon::HoughDataPerSectorVec' , 'StoreGateSvc+HoughDataPerSectorVec')]
    efmuInsideOutRecoSequence += ViewVerifyInsideOut


  #Inside-out reconstruction

  cbMuonName = muNames.EFCBInOutName
  if 'Late' in name:
    cbMuonName = cbMuonName+"_Late"
    theInsideOutRecoAlg = MuGirlStauAlg("TrigMuonLateInsideOutRecoAlg_"+name,InDetCandidateLocation="InDetCandidates_"+name)
    insideoutcreatoralg = StauCreatorAlg("TrigLateMuonCreatorAlg_"+name, TagMaps=["stauTagMap"],InDetCandidateLocation="InDetCandidates_"+name,
                                         MuonContainerLocation = cbMuonName, MonTool = MuonCreatorAlgMonitoring("LateMuonCreatorAlg_"+name))
  else:
    theInsideOutRecoAlg = MuonInsideOutRecoAlg("TrigMuonInsideOutRecoAlg_"+name,InDetCandidateLocation="InDetCandidates_"+name)
    insideoutcreatoralg = MuonCreatorAlg("TrigMuonCreatorAlgInsideOut_"+name, TagMaps=["muGirlTagMap"],InDetCandidateLocation="InDetCandidates_"+name,
                                         MuonContainerLocation = cbMuonName, SegmentContainerName = "InsideOutCBSegments", ExtrapolatedLocation = "InsideOutCBExtrapolatedMuons",
                                         MSOnlyExtrapolatedLocation = "InsideOutCBMSOnlyExtrapolatedMuons", CombinedLocation = "InsideOutCBCombinedMuon", MonTool = MuonCreatorAlgMonitoring("MuonCreatorAlgInsideOut_"+name))

  efAlgs.append(theInsideOutRecoAlg)
  efAlgs.append(insideoutcreatoralg)

  # setup inside-out algs
  for efAlg in efAlgs:
      if "RoIs" in efAlg.properties():
        efAlg.RoIs = RoIs
      efmuInsideOutRecoSequence += efAlg
  sequenceOut = cbMuonName

  return efmuInsideOutRecoSequence, sequenceOut



def efmuisoRecoSequence( RoIs, Muons ):

  # ATR-20453
  # Until such time as FS and RoI collections do not interfere, a hacky fix
  from AthenaCommon.CFElements import seqAND,parOR

  # ATR-20453
  # Until such time as FS and RoI collections do not interfere, a hacky fix
  #efmuisoRecoSequence = parOR("efmuIsoViewNode")
  efmuisoRecoSequence = seqAND("efmuIsoViewNode")

  from TrigInDetConfig.InDetSetup import makeInDetAlgs
  viewAlgs = makeInDetAlgs(whichSignature="MuonIso",rois = RoIs)

  for viewAlg in viewAlgs:
    efmuisoRecoSequence += viewAlg
    if "InDetTrigTrackParticleCreatorAlg" in viewAlg.name():
        TrackCollection = viewAlg.TrackName

  #Precision Tracking
  PTAlgs = [] #List of precision tracking algs
  PTTracks = [] #List of TrackCollectionKeys
  PTTrackParticles = [] #List of TrackParticleKeys
  
  from TrigInDetConfig.InDetPT import makeInDetPrecisionTracking
  PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( "muonIso", rois=RoIs, inputFTFtracks=TrackCollection)

  PTSeq = parOR("precisionTrackingInMuonsIso", PTAlgs  )
  efmuisoRecoSequence += PTSeq

  # set up algs
  from TrigMuonEF.TrigMuonEFConfig import TrigMuonEFTrackIsolationMTConfig
  trigEFmuIso = TrigMuonEFTrackIsolationMTConfig("TrigEFMuIso")
  trigEFmuIso.MuonEFContainer = Muons
  trackParticles = PTTrackParticles[-1]
  trigEFmuIso.IdTrackParticles = trackParticles
  trigEFmuIso.MuonContName = muNames.EFIsoMuonName

  efmuisoRecoSequence += trigEFmuIso

  sequenceOut = trigEFmuIso.MuonContName

  return efmuisoRecoSequence, sequenceOut


def efLateMuRoISequence(): 

  from AthenaCommon.CFElements import parOR
  efLateMuRoISequence = parOR("efLateMuRoIViewNode")

  from TrigmuRoI.TrigmuRoIConfig import TrigmuRoIMT
  roiAlg = TrigmuRoIMT("TrigmuRoIMT")
  sequenceOut = "LateMuRoIs"
  roiAlg.RoisWriteHandleKey=sequenceOut

  efLateMuRoISequence+=roiAlg

  return efLateMuRoISequence, sequenceOut
