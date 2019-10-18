#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
log = logging.getLogger('MuonSetup')

### Output data name ###
from TrigEDMConfig.TriggerEDMRun3 import recordable
from AthenaCommon.DetFlags import DetFlags
from MuonConfig.MuonBytestreamDecodeConfig import MuonCacheNames

from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags
from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

TrackParticlesName = recordable("HLT_xAODTracks_Muon")
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
def makeMuonPrepDataAlgs(forFullScan=False):

  postFix = ""
  if forFullScan:
    postFix = "FS"

  eventAlgs_MuonPRD = [] # These algs should be prepared for configuring RoIs same as muon RoIs used in viewAlg.
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

  from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscRdoToCscPrepDataTool
  CscRdoToCscPrepDataTool = Muon__CscRdoToCscPrepDataTool(name                = "CscRdoToCscPrepDataTool")

  ToolSvc += CscRdoToCscPrepDataTool

  from MuonRdoToPrepData.MuonRdoToPrepDataConf import CscRdoToCscPrepData
  CscRdoToCscPrepData = CscRdoToCscPrepData(name                    = "CscRdoToCscPrepData" + postFix,
                                            CscRdoToCscPrepDataTool = CscRdoToCscPrepDataTool,
                                            PrintPrepData           = False,
                                            DoSeededDecoding        = not forFullScan,
                                            RoIs                    = "MURoIs" )


  from MuonByteStream.MuonByteStreamConf import Muon__CscRawDataProvider
  CscRawDataProvider = Muon__CscRawDataProvider(name         = "CscRawDataProvider" + postFix,
                                                ProviderTool = MuonCscRawDataProviderTool,
                                                DoSeededDecoding        = not forFullScan,
                                                RoIs                    = "MURoIs")

  from CscClusterization.CscClusterizationConf import CscThresholdClusterBuilderTool
  CscClusterBuilderTool = CscThresholdClusterBuilderTool(name        = "CscThresholdClusterBuilderTool" )
  ToolSvc += CscClusterBuilderTool

  #CSC cluster building
  from CscClusterization.CscClusterizationConf import CscThresholdClusterBuilder
  CscClusterBuilder = CscThresholdClusterBuilder(name            = "CscThresholdClusterBuilder",
                                                 cluster_builder = CscClusterBuilderTool)

  eventAlgs_MuonPRD.append( CscRdoToCscPrepData )
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

  from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MdtRdoToPrepDataTool
  MdtRdoToMdtPrepDataTool = Muon__MdtRdoToPrepDataTool(name                = "MdtRdoToPrepDataTool")

  ToolSvc += MdtRdoToMdtPrepDataTool

  from MuonRdoToPrepData.MuonRdoToPrepDataConf import MdtRdoToMdtPrepData
  MdtRdoToMdtPrepData = MdtRdoToMdtPrepData(name             = "MdtRdoToMdtPrepData" + postFix,
                                            DecodingTool     = MdtRdoToMdtPrepDataTool,
                                            PrintPrepData    = False,
                                            DoSeededDecoding = not forFullScan,
                                            RoIs             = "MURoIs")


  from MuonByteStream.MuonByteStreamConf import Muon__MdtRawDataProvider
  MdtRawDataProvider = Muon__MdtRawDataProvider(name         = "MdtRawDataProvider" + postFix,
                                                ProviderTool = MuonMdtRawDataProviderTool,
                                                DoSeededDecoding = not forFullScan,
                                                RoIs = "MURoIs"
                                                )

  eventAlgs_MuonPRD.append( MdtRdoToMdtPrepData )
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
  if athenaCommonFlags.isOnline: 
      RpcRdoToRpcPrepDataTool.ReadKey = ""

  ToolSvc += RpcRdoToRpcPrepDataTool

  from MuonRdoToPrepData.MuonRdoToPrepDataConf import RpcRdoToRpcPrepData
  RpcRdoToRpcPrepData = RpcRdoToRpcPrepData(name             = "RpcRdoToRpcPrepData" + postFix,
                                            DecodingTool     = RpcRdoToRpcPrepDataTool,
                                            PrintPrepData    = False,
                                            DoSeededDecoding = not forFullScan,
                                            RoIs             = "MURoIs")

  from MuonByteStream.MuonByteStreamConf import Muon__RpcRawDataProvider
  RpcRawDataProvider = Muon__RpcRawDataProvider(name         = "RpcRawDataProvider" + postFix,
                                                ProviderTool = MuonRpcRawDataProviderTool,
                                                DoSeededDecoding = not forFullScan,
                                                RoIs = "MURoIs")

  if globalflags.InputFormat.is_bytestream():
    eventAlgs_MuonPRD.append( RpcRawDataProvider )
    viewAlgs_MuonPRD.append( RpcRawDataProvider )
  eventAlgs_MuonPRD.append( RpcRdoToRpcPrepData )
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
                                            RoIs             = "MURoIs")

  from MuonByteStream.MuonByteStreamConf import Muon__TgcRawDataProvider
  TgcRawDataProvider = Muon__TgcRawDataProvider(name         = "TgcRawDataProvider" + postFix,                                                
                                                ProviderTool = MuonTgcRawDataProviderTool,
                                                DoSeededDecoding = not forFullScan,
                                                RoIs             = "MURoIs" )

  if globalflags.InputFormat.is_bytestream():
    eventAlgs_MuonPRD.append( TgcRawDataProvider )
    viewAlgs_MuonPRD.append( TgcRawDataProvider )
  eventAlgs_MuonPRD.append( TgcRdoToTgcPrepData )
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


  return ( eventAlgs_MuonPRD, viewAlgs_MuonPRD )


# This def should be removed in the future.
# We have not yet integrated muon decoding tool, so this def is used now.
def muFastRecoSequence( RoIs ):

  from AthenaCommon.AppMgr import ToolSvc
  from AthenaCommon.DetFlags import DetFlags
  from AthenaCommon.CFElements import parOR

  muFastRecoSequence = parOR("l2MuViewNode")

  # Get the common data preparation algorithms
  (eventDPalgs, viewDPalgs) =  makeMuonPrepDataAlgs()

  for alg in viewDPalgs:
    # This is an ugly way to get hold of the algorithms we want to schedule
    # It will be improved once we migrate to the new job options
    if 'RpcRawData' in alg.name():
      RpcRawDataProviderAlg = alg
    if 'RpcRdoToRpc' in alg.name():
      RpcRdoToPrdAlg = alg
    if 'MdtRawData' in alg.name():
      MdtRawDataProviderAlg = alg
    if 'MdtRdoToMdt' in alg.name():
      MdtRdoToPrdAlg = alg
    if 'TgcRawData' in alg.name():
      TgcRawDataProviderAlg = alg
    if 'TgcRdoToTgc' in alg.name():
      TgcRdoToPrdAlg = alg
    if 'CscRawData' in alg.name():
      CscRawDataProviderAlg = alg
    if 'CscRdoToCsc' in alg.name():
      CscRdoToPrdAlg = alg
    if 'CscThresholdClusterBuilder' in alg.name():
      CscClusterAlg = alg

  # Schedule BS->RDO only if needed (not needed on MC RDO files)
  if DetFlags.readRDOBS.RPC_on():
    muFastRecoSequence += RpcRawDataProviderAlg
  if DetFlags.readRDOBS.MDT_on():
    muFastRecoSequence += MdtRawDataProviderAlg
  if DetFlags.readRDOBS.TGC_on():
    muFastRecoSequence += TgcRawDataProviderAlg
  if DetFlags.readRDOBS.CSC_on():
    muFastRecoSequence += CscRawDataProviderAlg
  # Always need RDO->PRD
  muFastRecoSequence += RpcRdoToPrdAlg
  muFastRecoSequence += MdtRdoToPrdAlg
  muFastRecoSequence += TgcRdoToPrdAlg
  muFastRecoSequence += CscRdoToPrdAlg
  # and Csc clustering alg
  muFastRecoSequence += CscClusterAlg

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

  muFastAlg.RecMuonRoI = "RecMURoIs"
  muFastAlg.MuRoIs = RoIs
  muFastAlg.MuonL2SAInfo = muNames.L2SAName
  muFastAlg.MuonCalibrationStream = "MuonCalibrationStream"
  muFastAlg.forID = "forID"
  muFastAlg.forMS = "forMS"

  muFastRecoSequence += muFastAlg
  sequenceOut = muFastAlg.MuonL2SAInfo

  return muFastRecoSequence, sequenceOut

def muonIDFastTrackingSequence( RoIs, name ):

  from AthenaCommon.CFElements import parOR

  viewNodeName=name+"FastIDViewNode"
  muonIDFastTrackingSequence = parOR(viewNodeName)

  ### Define input data of Inner Detector algorithms  ###
  ### and Define EventViewNodes to run the algorithms ###
  from TriggerMenuMT.HLTMenuConfig.CommonSequences.InDetSetup import makeInDetAlgs
  (viewAlgs, eventAlgs) = makeInDetAlgs(whichSignature="Muon", rois = RoIs)

  global TrackParticlesName
  global theFTF_name

  #TrackParticlesName = ""
  for viewAlg in viewAlgs:
      muonIDFastTrackingSequence += viewAlg
      if "InDetTrigTrackParticleCreatorAlg" in  viewAlg.name():
          TrackParticlesName = viewAlg.TrackParticlesName
      if "TrigFastTrackFinder" in  viewAlg.name():
          theFTF_name = viewAlg.getName()

  return muonIDFastTrackingSequence, eventAlgs

def muCombRecoSequence( RoIs ):

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
  muCombAlg = TrigmuCombMTConfig("Muon", theFTF_name)
  muCombAlg.L2StandAloneMuonContainerName = muNames.L2SAName
  muCombAlg.TrackParticlesContainerName = TrackParticlesName
  muCombAlg.L2CombinedMuonContainerName = muNames.L2CBName

  muCombRecoSequence += muCombAlg
  sequenceOut = muCombAlg.L2CombinedMuonContainerName

  return muCombRecoSequence, sequenceOut


def l2muisoRecoSequence( RoIs ):
  global TrackParticlesName

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

  from AthenaCommon.AppMgr import ServiceMgr
  import AthenaCommon.CfgGetter as CfgGetter

  from AthenaCommon.CfgGetter import getPublicTool, getPublicToolClone
  from AthenaCommon import CfgMgr
  from AthenaCommon.CFElements import parOR
  from AthenaCommon.AppMgr import ToolSvc
  from MuonRecExample.MuonStandalone import MooSegmentFinderAlg

  muEFSARecoSequence = parOR("efmsViewNode_"+name)

  efAlgs = []

  ### Provide Muon_PrepDataAlgorithms ###
  from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup  import makeMuonPrepDataAlgs
  ( eventAlgs_MuonPRD, viewAlgs_MuonPRD ) = makeMuonPrepDataAlgs( name == 'FS')

  # setup RDO preparator algorithms 
  if name != 'FS':
    # we now try to share the data preparation algorithms with L2, so we tell the view that it should expect the MDT, TGC, CSC and RPC PRDs to be available
    efAlgs.append( CfgMgr.AthViews__ViewDataVerifier(name = "EFMuonViewDataVerifier",
                                                     DataObjects = [( 'Muon::MdtPrepDataContainer'      , 'StoreGateSvc+MDT_DriftCircles' ),
                                                                    ( 'Muon::TgcPrepDataContainer'      , 'StoreGateSvc+TGC_Measurements' ),
                                                                    ( 'Muon::RpcPrepDataContainer'      , 'StoreGateSvc+RPC_Measurements' ),
                                                                    ( 'Muon::CscStripPrepDataContainer' , 'StoreGateSvc+CSC_Measurements'),
                                                                    ( 'Muon::CscPrepDataContainer'      , 'StoreGateSvc+CSC_Clusters') ] )

                 )
  for viewAlg_MuonPRD in viewAlgs_MuonPRD:
    # we now try to share the MDT, CSC and TGC data preparation algorithms with L2, so only add those if we are running full-scan
    # this is slightly ugly, should be improved in new JO setup
    if name == 'FS':
      efAlgs.append( viewAlg_MuonPRD )
   
  from TrkDetDescrSvc.TrkDetDescrSvcConf import Trk__TrackingVolumesSvc
  ServiceMgr += Trk__TrackingVolumesSvc("TrackingVolumesSvc")

  #need MdtCondDbAlg for the MuonStationIntersectSvc (required by segment and track finding)
  from AthenaCommon.AlgSequence import AthSequencer
  from MuonCondAlg.MuonTopCondAlgConfigRUN2 import MdtCondDbAlg
  if not athenaCommonFlags.isOnline:
    condSequence = AthSequencer("AthCondSeq")
    if not hasattr(condSequence,"MdtCondDbAlg"):
        condSequence += MdtCondDbAlg("MdtCondDbAlg")

  theSegmentFinderAlg = MooSegmentFinderAlg("TrigMuonSegmentMaker_"+name)


  TrackBuilder = CfgMgr.MuPatTrackBuilder("MuPatTrackBuilder" ,MuonSegmentCollection="MuonSegments")
  TrackBuilder.TrackSteering=CfgGetter.getPublicToolClone("TrigMuonTrackSteering", "MuonTrackSteering")

  from AthenaCommon.Include import include
  include("InDetBeamSpotService/BeamCondSvc.py" )
  from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg, xAODMaker__TrackCollectionCnvTool, xAODMaker__RecTrackParticleContainerCnvTool

  muonParticleCreatorTool = getPublicTool("MuonParticleCreatorTool")

  muonTrackCollectionCnvTool = xAODMaker__TrackCollectionCnvTool( name = "MuonTrackCollectionCnvTool", TrackParticleCreator = muonParticleCreatorTool )

  muonRecTrackParticleContainerCnvTool = xAODMaker__RecTrackParticleContainerCnvTool(name = "MuonRecTrackParticleContainerCnvTool", TrackParticleCreator = muonParticleCreatorTool )

  xAODTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg( name = "MuonStandaloneTrackParticleCnvAlg_"+name,
                                                            TrackParticleCreator = muonParticleCreatorTool,
                                                            TrackCollectionCnvTool=muonTrackCollectionCnvTool,
                                                            RecTrackParticleContainerCnvTool = muonRecTrackParticleContainerCnvTool,
                                                            TrackContainerName = "MuonSpectrometerTracks",
                                                            xAODTrackParticlesFromTracksContainerName = "MuonSpectrometerTrackParticles",
                                                            ConvertTrackParticles = False,
                                                            ConvertTracks = True)
  theCaloEnergyTool = getPublicToolClone("TrigCaloEnergyToolSA_"+name, "MuidCaloEnergyTool", EnergyLossMeasurement=False, MopParametrization=True, TrackIsolation=False)

  theCaloTSOS = getPublicToolClone("TrigCaloTrackStateOnSurfaceSA_"+name, "MuidCaloTrackStateOnSurface", CaloEnergyDeposit=theCaloEnergyTool, CaloEnergyParam=theCaloEnergyTool, Propagator ='TMEF_Propagator', MinRemainingEnergy= 200, ParamPtCut= 3000)
  from MuidCaloScatteringTools.MuidCaloScatteringToolsConf import Rec__MuidMaterialEffectsOnTrackProvider
  Rec__MuidMaterialEffectsOnTrackProvider.TSOSTool=theCaloTSOS


  theTrackCleaner = getPublicToolClone("TrigMuonTrackCleanerSA_"+name, "MuonTrackCleaner", Fitter='TMEF_iPatFitter', SLFitter='TMEF_iPatFitter')

  from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
  trkSummaryTool =  Trk__TrackSummaryTool( 'TrigMuonTrackSummarySA',MuonSummaryHelperTool=getPublicTool('MuonTrackSummaryHelperTool'), doSharedHits=False)
  ToolSvc += Trk__TrackSummaryTool('TrigMuonTrackSummarySA')

  theTrackQueryNoFit = getPublicToolClone("TrigMuonTrackQueryNoFitSA_"+name, "MuonTrackQuery", Fitter="")
  theTrackBuilderTool = getPublicToolClone("TrigMuonSATrackBuilderSA_"+name,"CombinedMuonTrackBuilder", UseCaloTG = True, CaloTSOS=theCaloTSOS, CaloMaterialProvider='TMEF_TrkMaterialProviderTool', MuonHoleRecovery="",CaloEnergyParam=theCaloEnergyTool,MuonErrorOptimizer="", Fitter='TMEF_iPatFitter', MaterialAllocator="TMEF_MaterialAllocator", Propagator='TMEF_Propagator', LargeMomentumError=0.5, PerigeeAtSpectrometerEntrance=False, ReallocateMaterial=False, TrackSummaryTool=trkSummaryTool, Cleaner=theTrackCleaner,TrackQuery=theTrackQueryNoFit)


  theCandidateTool = getPublicToolClone("MuonCandidateTool_SA"+name, "MuonCandidateTool", TrackBuilder=theTrackBuilderTool)

  theMuonCandidateAlg=CfgMgr.MuonCombinedMuonCandidateAlg("MuonCandidateAlg_"+name,MuonCandidateTool=theCandidateTool,MuonCandidateLocation="MuonCandidates")


  muonparticlecreator = getPublicToolClone("MuonParticleCreator", "TrackParticleCreatorTool", UseTrackSummaryTool=False, UseMuonSummaryTool=True, KeepAllPerigee=True)
  theTrackQueryNoFit = getPublicToolClone("TrigMuonTrackQueryNoFitSA", "MuonTrackQuery", Fitter="")
  thecreatortool= getPublicToolClone("MuonCreatorTool_SA", "MuonCreatorTool", ScatteringAngleTool="", MuonSelectionTool="", FillTimingInformation=False, UseCaloCells=False, MakeSAMuons=True, MomentumBalanceTool="",  TrackParticleCreator=muonparticlecreator,TrackQuery=theTrackQueryNoFit)

  msMuonName = muNames.EFSAName
  if 'FS' in name:
    msMuonName = muNamesFS.EFSAName
  themuoncreatoralg = CfgMgr.MuonCreatorAlg("MuonCreatorAlg_"+name, MuonCreatorTool=thecreatortool, CreateSAmuons=True, MakeClusters=False, TagMaps=[], MuonContainerLocation=msMuonName,ExtrapolatedLocation = "HLT_MSExtrapolatedMuons_"+name, MSOnlyExtrapolatedLocation = "HLT_MSOnlyExtrapolatedMuons_"+name )

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
          efAlg.RoIs = "FSRoI"
        else:
          efAlg.RoIs = RoIs
      muEFSARecoSequence += efAlg
  sequenceOut = themuoncreatoralg.MuonContainerLocation

  return muEFSARecoSequence, sequenceOut




def muEFCBRecoSequence( RoIs, name ):
  global TrackParticlesName

  from AthenaCommon.AppMgr import ToolSvc
  import AthenaCommon.CfgGetter as CfgGetter

  from AthenaCommon.CfgGetter import getPublicTool, getPublicToolClone
  from AthenaCommon import CfgMgr
  from AthenaCommon.CFElements import parOR, seqAND

  efAlgs = []
  muEFCBRecoSequence = parOR("efcbViewNode_"+name)
  #Need ID tracking related objects and MS tracks from previous steps
  ViewVerifyMS = CfgMgr.AthViews__ViewDataVerifier("muonCBViewDataVerifier")
  ViewVerifyMS.DataObjects = [( 'Muon::CscStripPrepDataContainer' , 'StoreGateSvc+CSC_Measurements' ),  ( 'Muon::MdtPrepDataContainer' , 'StoreGateSvc+MDT_DriftCircles' ),  ( 'MuonCandidateCollection' , 'StoreGateSvc+MuonCandidates') ]
  muEFCBRecoSequence += ViewVerifyMS
  eventAlgs=[]
  if "FS" in name:
    #Need to run tracking for full scan chains
    from TriggerMenuMT.HLTMenuConfig.CommonSequences.InDetSetup import makeInDetAlgs
    (viewAlgs, eventAlgs) = makeInDetAlgs(whichSignature = "MuonFS", rois = RoIs) 

     #TrackParticlesName = ""
    for viewAlg in viewAlgs:
      muEFCBRecoSequence += viewAlg
      if "InDetTrigTrackParticleCreatorAlg" in viewAlg.name():
        TrackParticlesName = viewAlg.TrackParticlesName  # noqa: F841
        TrackCollection = viewAlg.TrackName

  else:
    TrackCollection="TrigFastTrackFinder_Tracks_Muon" # this is hacking, please FIX IT
    ViewVerifyTrk = CfgMgr.AthViews__ViewDataVerifier("muonCBIDViewDataVerifier")
    ViewVerifyTrk.DataObjects = [( 'xAOD::TrackParticleContainer' , 'StoreGateSvc+'+TrackParticlesName ),
                                 ( 'TrackCollection' , 'StoreGateSvc+'+TrackCollection ),
                                 ( 'SCT_FlaggedCondData' , 'StoreGateSvc+SCT_FlaggedCondData' ),
                                 ( 'xAOD::IParticleContainer' , 'StoreGateSvc+'+TrackParticlesName )]

    if globalflags.InputFormat.is_bytestream():
      ViewVerifyTrk.DataObjects += [( 'InDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' ) ]
    muEFCBRecoSequence += ViewVerifyTrk


  #Precision Tracking
  PTAlgs = [] #List of precision tracking algs
  PTTracks = [] #List of TrackCollectionKeys
  PTTrackParticles = [] #List of TrackParticleKeys

  from TrigUpgradeTest.InDetPT import makeInDetPrecisionTracking
  #When run in a different view than FTF some data dependencies needs to be loaded through verifier
  #Pass verifier as an argument and it will automatically append necessary DataObjects
  #@NOTE: Don't provide any verifier if loaded in the same view as FTF
  if 'FS' in name:
    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( "muonsFS", inputFTFtracks=TrackCollection)
    PTSeq = seqAND("precisionTrackingInMuonsFS", PTAlgs  )
  else:
    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( "muons",  ViewVerifyTrk, inputFTFtracks= TrackCollection )
    PTSeq = seqAND("precisionTrackingInMuons", PTAlgs  )
  #Get last tracks from the list as input for other alg

  muEFCBRecoSequence += PTSeq


  #Default from FTF
  #trackParticles = "xAODTracks"
  #TODO: change according to what needs to be done here
  #Last key in the list is for the TrackParticles after all PT stages (so far only one :) )
  trackParticles = PTTrackParticles[-1]

  #Make InDetCandidates
  from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
  from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool
  from MuonTGRecTools.MuonTGRecToolsConf import Muon__MuonSystemExtensionTool
  pcExtensionTool = Trk__ParticleCaloExtensionTool(Extrapolator = AtlasExtrapolator())
  
  muonExtTool = Muon__MuonSystemExtensionTool(Extrapolator = AtlasExtrapolator(),
                                              ParticleCaloExtensionTool = pcExtensionTool)

  theIndetCandidateAlg = CfgMgr.MuonCombinedInDetCandidateAlg("TrigMuonCombinedInDetCandidateAlg_"+name,TrackSelector=getPublicTool("MuonCombinedInDetDetailedTrackSelectorTool"),TrackParticleLocation = [trackParticles],ForwardParticleLocation=trackParticles, MuonSystemExtensionTool=muonExtTool, InDetCandidateLocation="InDetCandidates_"+name)

  theCaloEnergyTool = getPublicToolClone("TrigCaloEnergyTool_"+name, "MuidCaloEnergyTool", EnergyLossMeasurement=False, MopParametrization=True, TrackIsolation=False)

  from TrkExRungeKuttaIntersector.TrkExRungeKuttaIntersectorConf import Trk__IntersectorWrapper as Propagator
  TrigMuonPropagator = Propagator(name = 'TrigMuonPropagator')
  ToolSvc += TrigMuonPropagator

  theCaloTSOS = getPublicToolClone("TrigCaloTrackStateOnSurface_"+name, "MuidCaloTrackStateOnSurface", CaloEnergyDeposit=theCaloEnergyTool, CaloEnergyParam=theCaloEnergyTool, Propagator =TrigMuonPropagator, MinRemainingEnergy= 200, ParamPtCut= 3000)
  from MuidCaloScatteringTools.MuidCaloScatteringToolsConf import Rec__MuidMaterialEffectsOnTrackProvider
  Rec__MuidMaterialEffectsOnTrackProvider.TSOSTool=theCaloTSOS

  theErrorOptimiser = getPublicToolClone("TrigMuonErrorOptimiser_"+name, "MuonErrorOptimisationTool", PrepareForFit=False, RecreateStartingParameters=False,RefitTool=getPublicToolClone("TrigMuidRefitTool_"+name, "MuonRefitTool", AlignmentErrors = False, Fitter = CfgGetter.getPublicTool("iPatFitter")))

  theTrackCleaner = getPublicToolClone("TrigMuonTrackCleaner_"+name, "MuonTrackCleaner", Fitter='TMEF_iPatFitter', SLFitter='TMEF_iPatFitter')

  from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
  trkSummaryTool =  Trk__TrackSummaryTool( 'TrigMuonTrackSummary',MuonSummaryHelperTool=getPublicTool('MuonTrackSummaryHelperTool'), doSharedHits=False)
  if DetFlags.detdescr.ID_on():
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryHelperTool
    trkSummaryTool.InDetSummaryHelperTool=InDetTrigTrackSummaryHelperTool
    trkSummaryTool.doHolesInDet=True
  ToolSvc += Trk__TrackSummaryTool('TrigMuonTrackSummary')

  theTrackQueryNoFit = getPublicToolClone("TrigMuonTrackQueryNoFit_"+name, "MuonTrackQuery", Fitter="")
  theTrackBuilderTool = getPublicToolClone("TrigCombinedMuonTrackBuilder_"+name,"CombinedMuonTrackBuilder", UseCaloTG = True, CaloTSOS=theCaloTSOS, CaloMaterialProvider='TMEF_TrkMaterialProviderTool', MuonHoleRecovery="",CaloEnergyParam=theCaloEnergyTool,MuonErrorOptimizer=theErrorOptimiser, Fitter='TMEF_iPatFitter', MaterialAllocator="TMEF_MaterialAllocator", Propagator=TrigMuonPropagator, LargeMomentumError=0.5, PerigeeAtSpectrometerEntrance=False, ReallocateMaterial=False, TrackSummaryTool=trkSummaryTool, Cleaner=theTrackCleaner,TrackQuery=theTrackQueryNoFit)
  theTrackQuery = getPublicToolClone("TrigMuonTrackQuery_"+name, "MuonTrackQuery", Fitter=theTrackBuilderTool)


  #MS ID combination
  theMuonCombinedFitTagTool = getPublicToolClone("TrigMuonCombinedFitTagTool_"+name, "MuonCombinedFitTagTool",TrackBuilder=theTrackBuilderTool,MuonRecovery=getPublicToolClone("TrigMuonRecovery_"+name,"MuidMuonRecovery", TrackBuilder=theTrackBuilderTool), TrackQuery=theTrackQuery, MatchQuality = getPublicToolClone("TrigMuonMatchQuality_"+name, "MuonMatchQuality", TrackQuery=theTrackQuery))
  tools=[]
  tools.append(theMuonCombinedFitTagTool)
  theMuonCombinedTool = getPublicToolClone("TrigMuonCombinedToolCB_"+name, "MuonCombinedTool", MuonCombinedTagTools=tools)

  candidatesName = "MuonCandidates"
  if 'FS' in name:
    candidatesName = "MuonCandidates_FS"

  theMuonCombinedAlg = CfgMgr.MuonCombinedAlg("TrigMuonCombinedAlg_"+name, MuonCandidateLocation=candidatesName, MuonCombinedTool=theMuonCombinedTool, CombinedTagMaps=["muidcoTagMap"], InDetCandidateLocation="InDetCandidates_"+name)


  #Create xAOD Muons
  muonparticlecreator = getPublicToolClone("MuonParticleCreatorCB_"+name, "TrackParticleCreatorTool", UseTrackSummaryTool=False, UseMuonSummaryTool=True, KeepAllPerigee=True)
  thecreatortoolCB= getPublicToolClone("MuonCreatorTool_triggerCB_"+name, "MuonCreatorTool", ScatteringAngleTool="", CaloMaterialProvider='TMEF_TrkMaterialProviderTool', MuonSelectionTool="", FillTimingInformation=False, UseCaloCells=False,TrackQuery=theTrackQueryNoFit,TrackParticleCreator=muonparticlecreator)

  cbMuonName = muNames.EFCBOutInName
  if 'FS' in name:
    cbMuonName = muNamesFS.EFCBName

  themuoncbcreatoralg = CfgMgr.MuonCreatorAlg("MuonCreatorAlgCB_"+name, MuonCandidateLocation=candidatesName, TagMaps=["muidcoTagMap"], InDetCandidateLocation="InDetCandidates_"+name)
  themuoncbcreatoralg.MuonCreatorTool=thecreatortoolCB
  themuoncbcreatoralg.MakeClusters=False
  themuoncbcreatoralg.ClusterContainerName=""
  themuoncbcreatoralg.MuonContainerLocation = cbMuonName
  themuoncbcreatoralg.SegmentContainerName = "CBSegments"
  themuoncbcreatoralg.ExtrapolatedLocation = "CBExtrapolatedMuons"
  themuoncbcreatoralg.MSOnlyExtrapolatedLocation = "CBMSOnlyExtrapolatedMuons"
  themuoncbcreatoralg.CombinedLocation = "HLT_CBCombinedMuon_"+name

  #Add all algorithms
  efAlgs.append(theIndetCandidateAlg)
  efAlgs.append(theMuonCombinedAlg)
  efAlgs.append(themuoncbcreatoralg)


  # setup muEFMsonly algs
  for efAlg in efAlgs:
    muEFCBRecoSequence += efAlg
  sequenceOut = themuoncbcreatoralg.MuonContainerLocation


  return muEFCBRecoSequence, eventAlgs, sequenceOut


def muEFInsideOutRecoSequence(RoIs, name):

  from AthenaCommon.CFElements import parOR, seqAND
  from AthenaCommon import CfgMgr
  from AthenaCommon.CfgGetter import getPublicTool, getPublicToolClone

  from AthenaCommon.AppMgr import ToolSvc, ServiceMgr
  import AthenaCommon.CfgGetter as CfgGetter
  from MuonRecExample.MuonStandalone import MooSegmentFinderAlg

  efAlgs = []

  viewNodeName="efmuInsideOutViewNode"
  if "Late" in name:
    viewNodeName = "latemuInsideOutViewNode"
  efmuInsideOutRecoSequence = parOR(viewNodeName)

  if "Late" in name:
    #May be able to do this more nicely in the future to run only if we haven't run this in other chains
    #Need PRD containers for inside-out reco
    ViewVerifyInsideOut = CfgMgr.AthViews__ViewDataVerifier("muonInsideOutViewDataVerifier")
    ViewVerifyInsideOut.DataObjects = [( 'Muon::MdtPrepDataContainer' , 'StoreGateSvc+MDT_DriftCircles' ),
                                       ( 'Muon::CscPrepDataContainer' , 'StoreGateSvc+CSC_Clusters' ),
                                       ( 'Muon::CscStripPrepDataContainer' , 'StoreGateSvc+CSC_Measurements' ),
                                       ( 'Muon::RpcPrepDataContainer' , 'StoreGateSvc+RPC_Measurements' ),
                                       ( 'Muon::TgcPrepDataContainer' , 'StoreGateSvc+TGC_Measurements' )]

    efmuInsideOutRecoSequence += ViewVerifyInsideOut

    #Need to run hough transform at start of late muon chain
    from TrkDetDescrSvc.TrkDetDescrSvcConf import Trk__TrackingVolumesSvc
    ServiceMgr += Trk__TrackingVolumesSvc("TrackingVolumesSvc")

    #need MdtCondDbAlg for the MuonStationIntersectSvc (required by segment and track finding)
    from AthenaCommon.AlgSequence import AthSequencer
    from MuonCondAlg.MuonTopCondAlgConfigRUN2 import MdtCondDbAlg
    if not athenaCommonFlags.isOnline:
      condSequence = AthSequencer("AthCondSeq")
      if not hasattr(condSequence,"MdtCondDbAlg"):
        condSequence += MdtCondDbAlg("MdtCondDbAlg")

    theSegmentFinderAlg = MooSegmentFinderAlg("TrigLateMuonSegmentMaker_"+name)
    efAlgs.append(theSegmentFinderAlg)

    # need to run precisions tracking for late muons, since we don't run it anywhere else
    TrackCollection="TrigFastTrackFinder_Tracks_Muon" 
    ViewVerifyTrk = CfgMgr.AthViews__ViewDataVerifier("lateMuonIDViewDataVerifier")

    ViewVerifyTrk.DataObjects = [( 'xAOD::TrackParticleContainer' , 'StoreGateSvc+HLT_xAODTracks_Muon' ),
                                 ( 'TrackCollection' , 'StoreGateSvc+'+TrackCollection ),
                                 ( 'SCT_FlaggedCondData' , 'StoreGateSvc+SCT_FlaggedCondData' ),
                                 ( 'xAOD::IParticleContainer' , 'StoreGateSvc+HLT_xAODTracks_Muon' )]
    
    if globalflags.InputFormat.is_bytestream():
      ViewVerifyTrk.DataObjects += [( 'InDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' ) ]
    efmuInsideOutRecoSequence += ViewVerifyTrk


    #Precision Tracking
    PTAlgs = [] #List of precision tracking algs
    PTTracks = [] #List of TrackCollectionKeys
    PTTrackParticles = [] #List of TrackParticleKeys

    from TrigUpgradeTest.InDetPT import makeInDetPrecisionTracking
    #When run in a different view than FTF some data dependencies needs to be loaded through verifier
    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( "muonsLate",  ViewVerifyTrk, inputFTFtracks= TrackCollection )
    PTSeq = seqAND("precisionTrackingInMuons", PTAlgs  )

    efmuInsideOutRecoSequence += PTSeq
    trackParticles = PTTrackParticles[-1]

    #Make InDetCandidates
    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool
    from MuonTGRecTools.MuonTGRecToolsConf import Muon__MuonSystemExtensionTool
    pcExtensionTool = Trk__ParticleCaloExtensionTool(Extrapolator = AtlasExtrapolator())
  
    muonExtTool = Muon__MuonSystemExtensionTool(Extrapolator = AtlasExtrapolator(),
                                                ParticleCaloExtensionTool = pcExtensionTool)

    theIndetCandidateAlg = CfgMgr.MuonCombinedInDetCandidateAlg("TrigMuonCombinedInDetCandidateAlg_"+name,TrackSelector=getPublicTool("MuonCombinedInDetDetailedTrackSelectorTool"),TrackParticleLocation = [trackParticles],ForwardParticleLocation=trackParticles, MuonSystemExtensionTool=muonExtTool, InDetCandidateLocation="InDetCandidates_"+name)

    efAlgs.append(theIndetCandidateAlg)


  else:
    # for non-latemu chains, the decoding/hough transform is run in an earlier step
    #Need PRD containers for inside-out reco
    ViewVerifyInsideOut = CfgMgr.AthViews__ViewDataVerifier("muonInsideOutViewDataVerifier")
    ViewVerifyInsideOut.DataObjects = [( 'Muon::MdtPrepDataContainer' , 'StoreGateSvc+MDT_DriftCircles' ),
                                       ( 'Muon::CscPrepDataContainer' , 'StoreGateSvc+CSC_Clusters' ),
                                       ( 'Muon::CscStripPrepDataContainer' , 'StoreGateSvc+CSC_Measurements' ),
                                       ( 'Muon::RpcPrepDataContainer' , 'StoreGateSvc+RPC_Measurements' ),
                                       ( 'Muon::TgcPrepDataContainer' , 'StoreGateSvc+TGC_Measurements' ),
                                       ( 'Muon::HoughDataPerSectorVec' , 'StoreGateSvc+HoughDataPerSectorVec')]
    efmuInsideOutRecoSequence += ViewVerifyInsideOut


  #Need to configure tools to avoid accessing calo data in muon trigger
  theCaloEnergyTool = getPublicToolClone("TrigCaloEnergyTool", "MuidCaloEnergyTool", EnergyLossMeasurement=False, MopParametrization=True, TrackIsolation=False)

  from TrkExRungeKuttaIntersector.TrkExRungeKuttaIntersectorConf import Trk__IntersectorWrapper as Propagator
  TrigMuonPropagator = Propagator(name = 'TrigMuonPropagator')
  ToolSvc += TrigMuonPropagator

  theCaloTSOS = getPublicToolClone("TrigCaloTrackStateOnSurface", "MuidCaloTrackStateOnSurface", CaloEnergyDeposit=theCaloEnergyTool, CaloEnergyParam=theCaloEnergyTool, Propagator =TrigMuonPropagator, MinRemainingEnergy= 200, ParamPtCut= 3000)
  from MuidCaloScatteringTools.MuidCaloScatteringToolsConf import Rec__MuidMaterialEffectsOnTrackProvider
  Rec__MuidMaterialEffectsOnTrackProvider.TSOSTool=theCaloTSOS

  theErrorOptimiser = getPublicToolClone("TrigMuonErrorOptimiser", "MuonErrorOptimisationTool", PrepareForFit=False, RecreateStartingParameters=False,RefitTool=getPublicToolClone("TrigMuidRefitTool", "MuonRefitTool", AlignmentErrors = False, Fitter = CfgGetter.getPublicTool("iPatFitter")))

  theTrackCleaner = getPublicToolClone("TrigMuonTrackCleaner", "MuonTrackCleaner", Fitter='TMEF_iPatFitter', SLFitter='TMEF_iPatFitter')


  from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
  trkSummaryTool =  Trk__TrackSummaryTool( 'TrigMuonTrackSummary',MuonSummaryHelperTool=getPublicTool('MuonTrackSummaryHelperTool'), doSharedHits=False)
  if DetFlags.detdescr.ID_on():
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryHelperTool
    trkSummaryTool.InDetSummaryHelperTool=InDetTrigTrackSummaryHelperTool
    trkSummaryTool.doHolesInDet=True
  ToolSvc += Trk__TrackSummaryTool('TrigMuonTrackSummary')
  theTrackQueryNoFit = getPublicToolClone("TrigMuonInsideOutTrackQueryNoFit", "MuonTrackQuery", Fitter="")
  theTrackBuilderTool = getPublicToolClone("TrigCombinedMuonTrackBuilder","CombinedMuonTrackBuilder", UseCaloTG = True, CaloTSOS=theCaloTSOS, CaloMaterialProvider='TMEF_TrkMaterialProviderTool', MuonHoleRecovery="",CaloEnergyParam=theCaloEnergyTool,MuonErrorOptimizer=theErrorOptimiser, Fitter='TMEF_iPatFitter', MaterialAllocator="TMEF_MaterialAllocator", Propagator=TrigMuonPropagator, LargeMomentumError=0.5, PerigeeAtSpectrometerEntrance=False, ReallocateMaterial=False, TrackSummaryTool=trkSummaryTool, Cleaner=theTrackCleaner,TrackQuery=theTrackQueryNoFit)


  #Inside-out reconstruction

  theMuonCandidateTrackBuilderTool = getPublicToolClone("TrigMuonCandidateTrackBuilderTool", "MuonCandidateTrackBuilderTool", MuonTrackBuilder=theTrackBuilderTool,)
  insideOutRecoTool = getPublicToolClone("TrigMuonInsideOutRecoTool", "MuonInsideOutRecoTool",MuonTrackBuilder=theTrackBuilderTool,MuonCandidateTrackBuilderTool=theMuonCandidateTrackBuilderTool)
  if 'Late' in name:
    insideOutRecoTool = getPublicToolClone("TrigMuonStauRecoTool", "MuonStauRecoTool",MuonInsideOutRecoTool="TMEF_MuonStauInsideOutRecoTool")
  theInsideOutRecoAlg = CfgMgr.MuonCombinedInDetExtensionAlg("TrigMuonInsideOutRecoAlg_"+name,InDetCandidateLocation="InDetCandidates_"+name,MuonCombinedInDetExtensionTools=[insideOutRecoTool],usePRDs=True,HasCSC=MuonGeometryFlags.hasCSC(),HasSTgc=(CommonGeometryFlags.Run() in ["RUN3", "RUN4"]),HasMM=(CommonGeometryFlags.Run() in ["RUN3", "RUN4"]))
  if 'Late' in name:
    theInsideOutRecoAlg.TagMap = "stauTagMap"

  #Create the xAOD muons
  cbMuonName = muNames.EFCBInOutName
  if 'Late' in name:
    cbMuonName = cbMuonName+"_Late"

  muonparticlecreator = getPublicToolClone("MuonParticleCreatorInsideOut", "TrackParticleCreatorTool", UseTrackSummaryTool=False, UseMuonSummaryTool=True, KeepAllPerigee=True)
  thecreatortool= getPublicToolClone("MuonCreatorTool_triggerInsideOut", "MuonCreatorTool", ScatteringAngleTool="", CaloMaterialProvider='TMEF_TrkMaterialProviderTool', MuonSelectionTool="", FillTimingInformation=False, UseCaloCells=False,TrackQuery=theTrackQueryNoFit,TrackParticleCreator=muonparticlecreator)
  if 'Late' in name:
    thecreatortool= getPublicToolClone("MuonCreatorTool_LateMu", "MuonCreatorTool", ScatteringAngleTool="", CaloMaterialProvider='TMEF_TrkMaterialProviderTool', MuonSelectionTool="", FillTimingInformation=False, UseCaloCells=False,TrackQuery=theTrackQueryNoFit,TrackParticleCreator=muonparticlecreator,BuildStauContainer=True)
  insideoutcreatoralg = CfgMgr.MuonCreatorAlg("MuonCreatorAlgInsideOut_"+name, TagMaps=["muGirlTagMap"],InDetCandidateLocation="InDetCandidates_"+name)
  if 'Late' in name:
    insideoutcreatoralg.BuildSlowMuon=True
    insideoutcreatoralg.TagMaps = ["stauTagMap"]
  else:
    insideoutcreatoralg.TagMaps = ["muGirlMap"]
  insideoutcreatoralg.MuonCreatorTool=thecreatortool
  insideoutcreatoralg.MakeClusters=False
  insideoutcreatoralg.ClusterContainerName=""
  insideoutcreatoralg.MuonContainerLocation = cbMuonName
  insideoutcreatoralg.SegmentContainerName = "InsideOutCBSegments"
  insideoutcreatoralg.ExtrapolatedLocation = "InsideOutCBExtrapolatedMuons"
  insideoutcreatoralg.MSOnlyExtrapolatedLocation = "InsideOutCBMSOnlyExtrapolatedMuons"
  insideoutcreatoralg.CombinedLocation = "InsideOutCBCombinedMuon"

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

  from AthenaCommon.CFElements import parOR, seqAND

  efmuisoRecoSequence = parOR("efmuIsoViewNode")

  from TriggerMenuMT.HLTMenuConfig.CommonSequences.InDetSetup import makeInDetAlgs
  (viewAlgs, eventAlgs) = makeInDetAlgs(whichSignature="MuonIso",rois = RoIs)

  #TrackParticlesName = ""
  for viewAlg in viewAlgs:
    efmuisoRecoSequence += viewAlg
    if "InDetTrigTrackParticleCreatorAlg" in viewAlg.name():
        TrackParticlesName = viewAlg.TrackParticlesName  # noqa: F841
        TrackCollection = viewAlg.TrackName

  #Precision Tracking
  PTAlgs = [] #List of precision tracking algs
  PTTracks = [] #List of TrackCollectionKeys
  PTTrackParticles = [] #List of TrackParticleKeys
  
  from TrigUpgradeTest.InDetPT import makeInDetPrecisionTracking
  PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( "muonsIso", inputFTFtracks=TrackCollection)

  PTSeq = seqAND("precisionTrackingInMuonsIso", PTAlgs  )
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
