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


TrackParticlesName = recordable("HLT_IDTrack_Muon_FTF") #FIXME: Unify this with ID Trig configuration ? config.FT().trkTracksFTF( doRecord = config.isRecordable )
theFTF_name = "FTFTracks_Muons" #Obsolete?
CBTPname = recordable("HLT_CBCombinedMuon_RoITrackParticles")
CBTPnameFS = recordable("HLT_CBCombinedMuon_FSTrackParticles")
ExtrpTPname = recordable("HLT_MSExtrapolatedMuons_RoITrackParticles")
ExtrpTPnameFS = recordable("HLT_MSExtrapolatedMuons_FSTrackParticles")
MSextrpTPname = recordable("HLT_MSOnlyExtrapolatedMuons_FSTrackParticles")
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.MuonSlice.doTrigMuonConfig=True

from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

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

  # Make sure required objects are still available at whole-event level
  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()

  # Load data into the view
  import AthenaCommon.CfgMgr as CfgMgr
  muDataPrepVDV = CfgMgr.AthViews__ViewDataVerifier( "muDataPrepVDV" + postFix )
  muDataPrepVDV.DataObjects = [( 'MdtPrepDataCollection_Cache' , MuonPrdCacheNames.MdtCache ),
                               ( 'RpcPrepDataCollection_Cache' , MuonPrdCacheNames.RpcCache ),
                               ( 'RpcCoinDataCollection_Cache' , MuonPrdCacheNames.RpcCoinCache ),
                               ( 'TagInfo' , 'DetectorStore+ProcessingTags' )]

  topSequence.SGInputLoader.Load += [ ( 'TagInfo' , 'DetectorStore+ProcessingTags' )]

  if MuonGeometryFlags.hasCSC():
    muDataPrepVDV.DataObjects += [( 'CscStripPrepDataCollection_Cache' , MuonPrdCacheNames.CscStripCache )]

  viewAlgs_MuonPRD.append( muDataPrepVDV )


  from AthenaCommon.AppMgr import ToolSvc

  if (MuonGeometryFlags.hasSTGC() and MuonGeometryFlags.hasMM()):
    ### sTGC RDO data ###
    from MuonSTGC_CnvTools.MuonSTGC_CnvToolsConf import Muon__STGC_RDO_Decoder
    STGCRodDecoder =  Muon__STGC_RDO_Decoder(name            = "STGC_RDO_Decoder")

    ToolSvc += STGCRodDecoder

    from MuonSTGC_CnvTools.MuonSTGC_CnvToolsConf import Muon__sTgcRdoToPrepDataToolMT
    sTgcRdoToPrepDataTool = Muon__sTgcRdoToPrepDataToolMT(name           = "sTgcRdoToPrepDataTool")

    ToolSvc += sTgcRdoToPrepDataTool

    from MuonRdoToPrepData.MuonRdoToPrepDataConf import StgcRdoToStgcPrepData
    StgcRdoToStgcPrepData = StgcRdoToStgcPrepData(name                    = "StgcRdoToStgcPrepData" + postFix)

    muDataPrepVDV.DataObjects += [( 'Muon::STGC_RawDataContainer' , 'StoreGateSvc+sTGCRDO' )]
    topSequence.SGInputLoader.Load += [( 'Muon::STGC_RawDataContainer' , 'StoreGateSvc+sTGCRDO' )]

    viewAlgs_MuonPRD.append( StgcRdoToStgcPrepData )

    ### MM RDO data ###
    from MuonMM_CnvTools.MuonMM_CnvToolsConf import Muon__MM_RDO_Decoder
    MMRodDecoder =  Muon__MM_RDO_Decoder(name            = "MM_RDO_Decoder")

    ToolSvc +=  MMRodDecoder

    from MuonMM_CnvTools.MuonMM_CnvToolsConf import Muon__MmRdoToPrepDataToolMT
    MmRdoToPrepDataTool = Muon__MmRdoToPrepDataToolMT(name           = "MmRdoToPrepDataTool")

    ToolSvc += MmRdoToPrepDataTool

    from MuonRdoToPrepData.MuonRdoToPrepDataConf import MM_RdoToMM_PrepData
    MM_RdoToMM_PrepData = MM_RdoToMM_PrepData(name                    = "MMRdoToMMPrepData" + postFix,
                                            PrintInputRdo = True  )

    muDataPrepVDV.DataObjects += [( 'Muon::MM_RawDataContainer' , 'StoreGateSvc+MMRDO' )]
    topSequence.SGInputLoader.Load += [( 'Muon::MM_RawDataContainer' , 'StoreGateSvc+MMRDO' )]

    viewAlgs_MuonPRD.append(  MM_RdoToMM_PrepData )

  if MuonGeometryFlags.hasCSC():
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

    from RegionSelector.RegSelToolConfig import makeRegSelTool_CSC
    CscRdoToCscPrepData.RegSel_CSC = makeRegSelTool_CSC()

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


  if MuonGeometryFlags.hasCSC():
    if globalflags.InputFormat.is_bytestream():
      viewAlgs_MuonPRD.append( CscRawDataProvider )
      muDataPrepVDV.DataObjects += [( 'CscRawDataCollection_Cache' , MuonCacheNames.CscCache )]
    else:
      muDataPrepVDV.DataObjects += [( 'CscRawDataContainer' , 'StoreGateSvc+CSCRDO' )]
      topSequence.SGInputLoader.Load += [( 'CscRawDataContainer' , 'StoreGateSvc+CSCRDO' )]
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
    muDataPrepVDV.DataObjects += [( 'MdtCsm_Cache' , MuonCacheNames.MdtCsmCache )]
  else:
    muDataPrepVDV.DataObjects += [( 'MdtCsmContainer' , 'StoreGateSvc+MDTCSM' )]
    topSequence.SGInputLoader.Load += [( 'MdtCsmContainer' , 'StoreGateSvc+MDTCSM' )]
  viewAlgs_MuonPRD.append( MdtRdoToMdtPrepData )


  ### RPC RDO data ###
  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcROD_Decoder
  RPCRodDecoder = Muon__RpcROD_Decoder(name	     = "RpcROD_Decoder" )
  ToolSvc += RPCRodDecoder

  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RPC_RawDataProviderToolMT
  MuonRpcRawDataProviderTool = Muon__RPC_RawDataProviderToolMT(name    = "RPC_RawDataProviderToolMT",
                                                               RpcContainerCacheKey = MuonCacheNames.RpcCache,
                                                               WriteOutRpcSectorLogic = False, 
                                                               # we don't need the RPC sector logic when running the trigger and 
                                                               #can't write it out if we want to use the IDC cache for the RDOs
                                                               Decoder = RPCRodDecoder )
  ToolSvc += MuonRpcRawDataProviderTool

  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRdoToPrepDataToolMT
  RpcRdoToRpcPrepDataTool = Muon__RpcRdoToPrepDataToolMT(name                         = "RpcRdoToPrepDataTool",
                                                         RpcPrdContainerCacheKey      = MuonPrdCacheNames.RpcCache,
                                                         RpcCoinDataContainerCacheKey = MuonPrdCacheNames.RpcCoinCache)

  if athenaCommonFlags.isOnline: 
      RpcRdoToRpcPrepDataTool.ReadKey = ""

  ToolSvc += RpcRdoToRpcPrepDataTool

  from MuonRdoToPrepData.MuonRdoToPrepDataConf import RpcRdoToRpcPrepData
  RpcRdoToRpcPrepData = RpcRdoToRpcPrepData(name             = "RpcRdoToRpcPrepData" + postFix,
                                            DecodingTool     = RpcRdoToRpcPrepDataTool,
                                            PrintPrepData    = False,
                                            DoSeededDecoding = not forFullScan,
                                            RoIs             = RoIs)

  from RegionSelector.RegSelToolConfig import makeRegSelTool_RPC
  RpcRdoToRpcPrepData.RegSel_RPC = makeRegSelTool_RPC()

  from MuonByteStream.MuonByteStreamConf import Muon__RpcRawDataProvider
  RpcRawDataProvider = Muon__RpcRawDataProvider(name         = "RpcRawDataProvider" + postFix,
                                                ProviderTool = MuonRpcRawDataProviderTool,
                                                DoSeededDecoding = not forFullScan,
                                                RoIs = RoIs)

  if globalflags.InputFormat.is_bytestream():
    viewAlgs_MuonPRD.append( RpcRawDataProvider )
    muDataPrepVDV.DataObjects += [( 'RpcPad_Cache' , MuonCacheNames.RpcCache )]
  else:
    muDataPrepVDV.DataObjects += [( 'RpcPadContainer' , 'StoreGateSvc+RPCPAD' )]
    topSequence.SGInputLoader.Load += [( 'RpcPadContainer' , 'StoreGateSvc+RPCPAD' )]
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
  from RegionSelector.RegSelToolConfig import makeRegSelTool_TGC
  TgcRdoToTgcPrepData.RegSel_TGC = makeRegSelTool_TGC()

  from MuonByteStream.MuonByteStreamConf import Muon__TgcRawDataProvider
  TgcRawDataProvider = Muon__TgcRawDataProvider(name         = "TgcRawDataProvider" + postFix,                                                
                                                ProviderTool = MuonTgcRawDataProviderTool,
                                                DoSeededDecoding = not forFullScan,
                                                RoIs             = RoIs )

  if globalflags.InputFormat.is_bytestream():
    viewAlgs_MuonPRD.append( TgcRawDataProvider )
    muDataPrepVDV.DataObjects += [( 'TgcRdo_Cache' , MuonCacheNames.TgcCache )]
  else:
    muDataPrepVDV.DataObjects += [( 'TgcRdoContainer' , 'StoreGateSvc+TGCRDO' )]
    topSequence.SGInputLoader.Load += [( 'TgcRdoContainer' , 'StoreGateSvc+TGCRDO' )]
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


def muFastRecoSequence( RoIs, doFullScanID = False ):

  from AthenaCommon.AppMgr import ToolSvc
  from AthenaCommon.CFElements import parOR

  muFastRecoSequence = parOR("l2MuViewNode")

  import AthenaCommon.CfgMgr as CfgMgr
  muFastRecoVDV = CfgMgr.AthViews__ViewDataVerifier("muFastRecoVDV")
  muFastRecoVDV.DataObjects = [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+'+RoIs ),
                               ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                               ( 'DataVector< LVL1::RecMuonRoI >' , 'StoreGateSvc+HLT_RecMURoIs' )]

  muFastRecoSequence += muFastRecoVDV

  if MuonGeometryFlags.hasCSC():
    # Configure the L2 CSC data preparator - we can turn off the data decoding here
    from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__CscDataPreparator
    L2CscDataPreparator = TrigL2MuonSA__CscDataPreparator(name = "L2MuonSACscDataPreparator",
                                                          DecodeBS = False,
                                                          DoDecoding = False,
                                                          CscRawDataProvider   = "",
                                                          CscPrepDataProvider  = "",
                                                          CscClusterProvider   = "")
    from RegionSelector.RegSelToolConfig import makeRegSelTool_CSC
    L2CscDataPreparator.RegSel_CSC = makeRegSelTool_CSC()
    ToolSvc += L2CscDataPreparator
 
  # Configure the L2 MDT data preparator - we can turn off the data decoding here
  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__MdtDataPreparator
  L2MdtDataPreparator = TrigL2MuonSA__MdtDataPreparator(name = "L2MuonSAMdtDataPreparator",
                                                        DecodeBS = False,
                                                        DoDecoding = False,
                                                        MDT_RawDataProvider = "",
                                                        MdtPrepDataProvider = "")
  from RegionSelector.RegSelToolConfig import makeRegSelTool_MDT
  L2MdtDataPreparator.RegSel_MDT = makeRegSelTool_MDT()


  ### RPC RDO data - turn off the data decoding here ###
  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__RpcDataPreparator
  L2RpcDataPreparator = TrigL2MuonSA__RpcDataPreparator(name = "L2MuonSARpcDataPreparator",
                                                        RpcPrepDataProvider  = "",
                                                        RpcPrepDataContainer = "RPC_Measurements",
                                                        RpcRawDataProvider   = "",
                                                        DoDecoding           = False,
                                                        DecodeBS             = False)
  ToolSvc += L2RpcDataPreparator
  from RegionSelector.RegSelToolConfig import makeRegSelTool_RPC
  L2RpcDataPreparator.RegSel_RPC = makeRegSelTool_RPC()


  ### TGC data preparation - turn off the data decoding here ###
  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__TgcDataPreparator
  L2TgcDataPreparator = TrigL2MuonSA__TgcDataPreparator(name = "L2MuonSATgcDataPreparator",
                                                        DecodeBS = False,
                                                        DoDecoding = False,
                                                        TgcPrepDataProvider  = "",
                                                        TgcRawDataProvider = "")
  from RegionSelector.RegSelToolConfig import makeRegSelTool_TGC
  L2TgcDataPreparator.RegSel_TGC = makeRegSelTool_TGC()


  ### sTGC RDO data - turn off the data decoding here ###
  if MuonGeometryFlags.hasSTGC():
    from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__StgcDataPreparator
    L2StgcDataPreparator = TrigL2MuonSA__StgcDataPreparator(name = "L2MuonSAStgcDataPreparator",
                                                            StgcPrepDataProvider  = "",
                                                            StgcPrepDataContainer = "STGC_Measurements",
                                                            StgcRawDataProvider   = "",
                                                            DoDecoding           = False,
                                                            DecodeBS             = False)
    from RegionSelector.RegSelToolConfig import makeRegSelTool_sTGC
    L2StgcDataPreparator.RegSel_STGC = makeRegSelTool_sTGC()
    ToolSvc += L2StgcDataPreparator


  ### MM RDO data - turn off the data decoding here ###
  if MuonGeometryFlags.hasMM():
    from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__MmDataPreparator
    L2MmDataPreparator = TrigL2MuonSA__MmDataPreparator(name = "L2MuonSAMmDataPreparator",
                                                        MmPrepDataProvider  = "",
                                                        MmPrepDataContainer = "MM_Measurements",
                                                        # MmRawDataProvider   = "",
                                                        DoDecoding           = False,
                                                        DecodeBS             = False)
    from RegionSelector.RegSelToolConfig import makeRegSelTool_MM
    L2MmDataPreparator.RegSel_MM = makeRegSelTool_MM()
    ToolSvc += L2MmDataPreparator

  ### set up MuFastSteering ###
  from TrigL2MuonSA.TrigL2MuonSAConfig import TrigL2MuonSAMTConfig
  muFastAlg = TrigL2MuonSAMTConfig("Muon")

  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__MuFastDataPreparator
  MuFastDataPreparator = TrigL2MuonSA__MuFastDataPreparator()
  if MuonGeometryFlags.hasCSC():
    MuFastDataPreparator.CSCDataPreparator  = L2CscDataPreparator
  else:
    MuFastDataPreparator.CSCDataPreparator  = ""
  MuFastDataPreparator.MDTDataPreparator  = L2MdtDataPreparator
  MuFastDataPreparator.RPCDataPreparator  = L2RpcDataPreparator
  MuFastDataPreparator.TGCDataPreparator  = L2TgcDataPreparator
  if MuonGeometryFlags.hasSTGC():
    MuFastDataPreparator.STGCDataPreparator = L2StgcDataPreparator
  else:
    MuFastDataPreparator.STGCDataPreparator = ""
  if MuonGeometryFlags.hasMM():
    MuFastDataPreparator.MMDataPreparator   = L2MmDataPreparator
  else:
    MuFastDataPreparator.MMDataPreparator   = ""
  
  muFastAlg.DataPreparator = MuFastDataPreparator

  muFastAlg.RecMuonRoI = "HLT_RecMURoIs"
  muFastAlg.MuRoIs = RoIs
  muFastAlg.MuonL2SAInfo = muNames.L2SAName
  muFastAlg.MuonCalibrationStream = "MuonCalibrationStream"
  muFastAlg.forID = muNames.L2forIDName
  muFastAlg.forMS = "forMS" 
  muFastAlg.FILL_FSIDRoI = doFullScanID

  muFastRecoSequence += muFastAlg
  sequenceOut = muFastAlg.MuonL2SAInfo

  return muFastRecoSequence, sequenceOut

def muonIDFastTrackingSequence( RoIs, name, extraLoads=None ):

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
  from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
  IDTrigConfig = getInDetTrigConfig( "muon"+name ) 

  from TrigInDetConfig.InDetSetup import makeInDetAlgs
  viewAlgs, viewVerify = makeInDetAlgs( config = IDTrigConfig, rois = RoIs )
  viewVerify.DataObjects += [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+'+RoIs )]
  if extraLoads:
    viewVerify.DataObjects += extraLoads

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

  from IOVDbSvc.CondDB import conddb
  if not conddb.folderRequested( '/TDAQ/Resources/ATLAS/PIXEL/Modules' ):
    ViewVerify.DataObjects += [( 'CondAttrListCollection', 'ConditionStore+/TDAQ/Resources/ATLAS/PIXEL/Modules' )]
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

  from MuonRecExample.MuonRecFlags import muonRecFlags

  muEFSARecoSequence = parOR("efmsViewNode_"+name)

  efAlgs = []

  EFMuonViewDataVerifier = CfgMgr.AthViews__ViewDataVerifier( "EFMuonViewDataVerifier_" + name )
  EFMuonViewDataVerifier.DataObjects = [( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                                        ( 'CaloCellContainer' , 'StoreGateSvc+AllCalo' ),
                                        ( 'TrackCollection' , 'StoreGateSvc+Tracks' ),
                                        ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+'+RoIs )]
  efAlgs.append( EFMuonViewDataVerifier )

  # Only load these objects if they aren't available in conddb
  from IOVDbSvc.CondDB import conddb
  if not conddb.folderRequested( "/MDT/DQMF/DEAD_ELEMENT" ):
    EFMuonViewDataVerifier.DataObjects += [( 'CondAttrListCollection' , 'ConditionStore+/MDT/DQMF/DEAD_ELEMENT' )]
  if not conddb.folderRequested( "/MDT/TUBE_STATUS/DEAD_TUBE" ):
    EFMuonViewDataVerifier.DataObjects += [( 'CondAttrListCollection' , 'ConditionStore+/MDT/TUBE_STATUS/DEAD_TUBE' )]

  if name != 'FS':
    # we now try to share the data preparation algorithms with L2, so we tell the view that it should expect the MDT, TGC, CSC and RPC PRDs to be available
    EFMuonViewDataVerifier.DataObjects += [( 'Muon::MdtPrepDataContainer' , 'StoreGateSvc+MDT_DriftCircles' ),
                                           ( 'Muon::TgcPrepDataContainer' , 'StoreGateSvc+TGC_Measurements' ),
                                           ( 'Muon::RpcPrepDataContainer' , 'StoreGateSvc+RPC_Measurements' )]
    if MuonGeometryFlags.hasCSC():
      EFMuonViewDataVerifier.DataObjects += [( 'Muon::CscStripPrepDataContainer' , 'StoreGateSvc+CSC_Measurements' ),
                                             ( 'Muon::CscPrepDataContainer' , 'StoreGateSvc+CSC_Clusters' )]
    if (MuonGeometryFlags.hasSTGC() and MuonGeometryFlags.hasMM()): 
      EFMuonViewDataVerifier.DataObjects += [( 'Muon::MMPrepDataContainer'       , 'StoreGateSvc+MM_Measurements'),
                                             ( 'Muon::sTgcPrepDataContainer'     , 'StoreGateSvc+STGC_Measurements')]

  #need MdtCondDbAlg for the MuonStationIntersectSvc (required by segment and track finding)
  from AthenaCommon.AlgSequence import AthSequencer
  from MuonCondAlg.MuonTopCondAlgConfigRUN2 import MdtCondDbAlg
  if not athenaCommonFlags.isOnline:
    condSequence = AthSequencer("AthCondSeq")
    if not hasattr(condSequence,"MdtCondDbAlg"):
      condSequence += MdtCondDbAlg("MdtCondDbAlg")
    # Sets up and configures the muon alignment:
    from MuonRecExample import MuonAlignConfig # noqa: F401

  if (MuonGeometryFlags.hasSTGC() and MuonGeometryFlags.hasMM()):
      theMuonLayerHough = CfgMgr.MuonLayerHoughAlg( "MuonLayerHoughAlg")
      efAlgs.append(theMuonLayerHough)
      SegmentFinder = CfgGetter.getPublicTool("MuonClusterSegmentFinderTool")
      Cleaner = CfgGetter.getPublicToolClone("MuonTrackCleaner_seg","MuonTrackCleaner")
      Cleaner.Extrapolator = CfgGetter.getPublicTool("MuonStraightLineExtrapolator")
      Cleaner.Fitter = CfgGetter.getPublicTool("MCTBSLFitterMaterialFromTrack")
      Cleaner.PullCut = 3
      Cleaner.PullCutPhi = 3
      SegmentFinder.TrackCleaner = Cleaner
      
      theSegmentFinderAlg = CfgMgr.MuonSegmentFinderAlg( "TrigMuonSegmentMaker_"+name,SegmentCollectionName="MuonSegments",
                                                       MuonPatternCalibration = CfgGetter.getPublicTool("MuonPatternCalibration"), 
                                                       MuonPatternSegmentMaker = CfgGetter.getPublicTool("MuonPatternSegmentMaker"), 
                                                       MuonTruthSummaryTool = None)
    # we check whether the layout contains any CSC chamber and if yes, we check that the user also wants to use the CSCs in reconstruction
      if MuonGeometryFlags.hasCSC() and muonRecFlags.doCSCs():
          CfgGetter.getPublicTool("CscSegmentUtilTool")
          CfgGetter.getPublicTool("Csc2dSegmentMaker")
          CfgGetter.getPublicTool("Csc4dSegmentMaker")
      else:
          theSegmentFinderAlg.Csc2dSegmentMaker = ""
          theSegmentFinderAlg.Csc4dSegmentMaker = ""

  else:
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
  ViewVerifyMS = CfgMgr.AthViews__ViewDataVerifier("muonCBViewDataVerifier_"+name)
  ViewVerifyMS.DataObjects = [( 'Muon::MdtPrepDataContainer' , 'StoreGateSvc+MDT_DriftCircles' ),  
                              ( 'Muon::TgcPrepDataContainer' , 'StoreGateSvc+TGC_Measurements' ),
                              ( 'Muon::RpcPrepDataContainer' , 'StoreGateSvc+RPC_Measurements' ),
                              ( 'MuonCandidateCollection' , 'StoreGateSvc+MuonCandidates'),
                              ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+'+RoIs ),
                              ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_FlaggedCondData' ),
                              ( 'MuonCandidateCollection' , 'StoreGateSvc+MuonCandidates_FS' ),
                              ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                              ( 'TrackCollection' , 'StoreGateSvc+Tracks' ),
                              ( 'CaloCellContainer' , 'StoreGateSvc+AllCalo' )]

  if MuonGeometryFlags.hasCSC():
    ViewVerifyMS.DataObjects += [( 'Muon::CscStripPrepDataContainer' , 'StoreGateSvc+CSC_Measurements' )]
  if (MuonGeometryFlags.hasSTGC() and MuonGeometryFlags.hasMM()): 
    ViewVerifyMS.DataObjects += [( 'Muon::MMPrepDataContainer' , 'StoreGateSvc+MM_Measurements'),
                                ( 'Muon::sTgcPrepDataContainer' , 'StoreGateSvc+STGC_Measurements') ]

  muEFCBRecoSequence += ViewVerifyMS

  # Add conditions data if not already available
  from IOVDbSvc.CondDB import conddb
  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()
  if not conddb.folderRequested( "/PIXEL/PixdEdx" ):
    ViewVerifyMS.DataObjects += [( 'AthenaAttributeList' , 'ConditionStore+/PIXEL/PixdEdx' )]

  if not globalflags.InputFormat.is_bytestream():
    topSequence.SGInputLoader.Load += [( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' )]
    ViewVerifyMS.DataObjects += [( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' )]


  from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
  signatureName = 'muon{}'.format( 'FS' if 'FS' in name else '' ) 
  IDTrigConfig = getInDetTrigConfig( signatureName )

  if "FS" in name:
    #Need to run tracking for full scan chains
    from TrigInDetConfig.InDetSetup import makeInDetAlgs
    viewAlgs, viewVerify = makeInDetAlgs(config = IDTrigConfig, rois = RoIs) 

    for viewAlg in viewAlgs:
      muEFCBRecoSequence += viewAlg

  else:
    ViewVerifyTrk = CfgMgr.AthViews__ViewDataVerifier("muonCBIDViewDataVerifier")
    ViewVerifyTrk.DataObjects = [( 'xAOD::TrackParticleContainer' , 'StoreGateSvc+'+TrackParticlesName ),
                                 ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_FlaggedCondData_TRIG' ),
                                 ( 'xAOD::IParticleContainer' , 'StoreGateSvc+'+TrackParticlesName ),
                                 ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+PixelByteStreamErrs' ),
                                 ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' )] #seems to be necessary, despite the load below

    if globalflags.InputFormat.is_bytestream():
      ViewVerifyTrk.DataObjects += [( 'IDCInDetBSErrContainer' , 'StoreGateSvc+PixelByteStreamErrs' ),
                                    ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' )]
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
    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( config = IDTrigConfig, rois = RoIs, verifier = False)
    PTSeq = parOR("precisionTrackingInMuonsFS", PTAlgs  )
  else:
    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( config = IDTrigConfig, rois = RoIs,  verifier = ViewVerifyTrk )
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
  ToolSvc.AtlasHoleSearchTool.BoundaryCheckTool.SctSummaryTool = SCT_ConditionsSummaryTool(SCT_ConditionsSetup.instanceName('InDetSCT_ConditionsSummaryToolWithoutFlagged'))

  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigSCTConditionsSummaryTool
  ToolSvc.CombinedMuonIDHoleSearch.BoundaryCheckTool.SctSummaryTool = InDetTrigSCTConditionsSummaryTool

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
  from MuonRecExample.MuonRecFlags import muonRecFlags

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
    import AthenaCommon.CfgGetter as CfgGetter
    if not athenaCommonFlags.isOnline:
      condSequence = AthSequencer("AthCondSeq")
      if not hasattr(condSequence,"MdtCondDbAlg"):
        condSequence += MdtCondDbAlg("MdtCondDbAlg")
      # Sets up and configures the muon alignment:
      from MuonRecExample import MuonAlignConfig # noqa: F401

      if (MuonGeometryFlags.hasSTGC() and MuonGeometryFlags.hasMM()):
        theMuonLayerHough = CfgMgr.MuonLayerHoughAlg( "MuonLayerHoughAlg")
        efAlgs.append(theMuonLayerHough)
        SegmentFinder = CfgGetter.getPublicTool("MuonClusterSegmentFinderTool")
        Cleaner = CfgGetter.getPublicToolClone("MuonTrackCleaner_seg","MuonTrackCleaner")
        Cleaner.Extrapolator = CfgGetter.getPublicTool("MuonStraightLineExtrapolator")
        Cleaner.Fitter = CfgGetter.getPublicTool("MCTBSLFitterMaterialFromTrack")
        Cleaner.PullCut = 3
        Cleaner.PullCutPhi = 3
        SegmentFinder.TrackCleaner = Cleaner
      
        theSegmentFinderAlg = CfgMgr.MuonSegmentFinderAlg( "TrigMuonSegmentMaker_"+name,SegmentCollectionName="MuonSegments",
                                                           MuonPatternCalibration = CfgGetter.getPublicTool("MuonPatternCalibration"), 
                                                           MuonPatternSegmentMaker = CfgGetter.getPublicTool("MuonPatternSegmentMaker"), 
                                                           MuonTruthSummaryTool = None)
        # we check whether the layout contains any CSC chamber and if yes, we check that the user also wants to use the CSCs in reconstruction
        if MuonGeometryFlags.hasCSC() and muonRecFlags.doCSCs():
          CfgGetter.getPublicTool("CscSegmentUtilTool")
          CfgGetter.getPublicTool("Csc2dSegmentMaker")
          CfgGetter.getPublicTool("Csc4dSegmentMaker")
        else:
          theSegmentFinderAlg.Csc2dSegmentMaker = ""
          theSegmentFinderAlg.Csc4dSegmentMaker = ""

      else:
        theSegmentFinderAlg = MooSegmentFinderAlg("TrigLateMuonSegmentMaker_"+name)

      efAlgs.append(theSegmentFinderAlg)

    # need to run precisions tracking for late muons, since we don't run it anywhere else

    #Precision Tracking
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( 'muonLate' )


    PTAlgs = [] #List of precision tracking algs
    PTTracks = [] #List of TrackCollectionKeys
    PTTrackParticles = [] #List of TrackParticleKeys

    from TrigInDetConfig.InDetPT import makeInDetPrecisionTracking
    #When run in a different view than FTF some data dependencies needs to be loaded through verifier
    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( config = IDTrigConfig, rois=RoIs)
    PTSeq = parOR("precisionTrackingInLateMuons", PTAlgs  )

    efmuInsideOutRecoSequence += PTSeq
    trackParticles = PTTrackParticles[-1]

    #Make InDetCandidates
    theIndetCandidateAlg = MuonCombinedInDetCandidateAlg("TrigMuonCombinedInDetCandidateAlg_"+name,TrackParticleLocation = [trackParticles],ForwardParticleLocation=trackParticles, 
                                                         InDetCandidateLocation="InDetCandidates_"+name)
    from AthenaCommon.AppMgr import ToolSvc
    from InDetTrigRecExample.InDetTrigConditionsAccess import SCT_ConditionsSetup
    from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_ConditionsSummaryTool
    ToolSvc.CombinedMuonIDHoleSearch.BoundaryCheckTool.SctSummaryTool = SCT_ConditionsSummaryTool(SCT_ConditionsSetup.instanceName('InDetSCT_ConditionsSummaryToolWithoutFlagged'))

    efAlgs.append(theIndetCandidateAlg)


  else:
    # for non-latemu chains, the decoding/hough transform is run in an earlier step
    #Need PRD containers for inside-out reco
    ViewVerifyInsideOut = CfgMgr.AthViews__ViewDataVerifier("muonInsideOutViewDataVerifier")
    ViewVerifyInsideOut.DataObjects = [( 'Muon::RpcPrepDataContainer' , 'StoreGateSvc+RPC_Measurements' ),
                                       ( 'Muon::TgcPrepDataContainer' , 'StoreGateSvc+TGC_Measurements' ),
                                       ( 'Muon::HoughDataPerSectorVec' , 'StoreGateSvc+HoughDataPerSectorVec')]
    if MuonGeometryFlags.hasCSC():
      ViewVerifyInsideOut.DataObjects += [( 'Muon::CscPrepDataContainer' , 'StoreGateSvc+CSC_Clusters' )]
    if (MuonGeometryFlags.hasSTGC() and MuonGeometryFlags.hasMM()): 
      ViewVerifyInsideOut.DataObjects += [( 'Muon::MMPrepDataContainer'       , 'StoreGateSvc+MM_Measurements'),
                                          ( 'Muon::sTgcPrepDataContainer'     , 'StoreGateSvc+STGC_Measurements') ]

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

  from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
  IDTrigConfig = getInDetTrigConfig( 'muonIso' )

  from TrigInDetConfig.InDetSetup import makeInDetAlgs
  viewAlgs, viewVerify = makeInDetAlgs( config = IDTrigConfig, rois = RoIs )
  viewVerify.DataObjects += [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+MUEFIsoRoIs' ),
                             ( 'xAOD::MuonContainer' , 'StoreGateSvc+IsoViewMuons' )]

  # Make sure required objects are still available at whole-event level
  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()
  if not globalflags.InputFormat.is_bytestream():
    viewVerify.DataObjects += [( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' )]
    topSequence.SGInputLoader.Load += [( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' )]

  for viewAlg in viewAlgs:
    efmuisoRecoSequence += viewAlg

  #Precision Tracking
  PTAlgs = [] #List of precision tracking algs
  PTTracks = [] #List of TrackCollectionKeys
  PTTrackParticles = [] #List of TrackParticleKeys
  
  from TrigInDetConfig.InDetPT import makeInDetPrecisionTracking
  PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( config = IDTrigConfig, rois=RoIs )

  PTSeq = parOR("precisionTrackingInMuonsIso", PTAlgs  )
  efmuisoRecoSequence += PTSeq

  # set up algs
  from TrigMuonEF.TrigMuonEFConfig import TrigMuonEFTrackIsolationMTConfig
  trigEFmuIso = TrigMuonEFTrackIsolationMTConfig("TrigEFMuIso")
  trigEFmuIso.MuonEFContainer = Muons
  trackParticles = PTTrackParticles[-1]
  trigEFmuIso.IdTrackParticles = trackParticles
  trigEFmuIso.MuonContName = muNames.EFIsoMuonName
  trigEFmuIso.ptcone02Name = Muons+".ptcone02"
  trigEFmuIso.ptcone03Name = Muons+".ptcone03"

  efmuisoRecoSequence += trigEFmuIso

  sequenceOut = trigEFmuIso.MuonContName

  return efmuisoRecoSequence, sequenceOut


def efLateMuRoISequence(): 

  from AthenaCommon.CFElements import parOR
  efLateMuRoISequence = parOR("efLateMuRoIViewNode")

  import AthenaCommon.CfgMgr as CfgMgr
  efLateMuRoIVDV = CfgMgr.AthViews__ViewDataVerifier( "efLateMuRoIVDV" )
  efLateMuRoIVDV.DataObjects = [( 'MuCTPI_RDO' , 'StoreGateSvc+MUCTPI_RDO' )]
  efLateMuRoISequence += efLateMuRoIVDV

  # Make sure the RDOs are still available at whole-event level
  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()
  topSequence.SGInputLoader.Load += [( 'MuCTPI_RDO' , 'StoreGateSvc+MUCTPI_RDO' )]

  from TrigmuRoI.TrigmuRoIConfig import TrigmuRoIMTConfig
  roiAlg = TrigmuRoIMTConfig("TrigmuRoIMT")
  sequenceOut = "LateMuRoIs"
  roiAlg.RoisWriteHandleKey=sequenceOut

  efLateMuRoISequence+=roiAlg

  return efLateMuRoISequence, sequenceOut
