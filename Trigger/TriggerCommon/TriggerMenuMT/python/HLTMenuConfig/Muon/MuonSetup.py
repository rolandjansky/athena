#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
from AthenaCommon.GlobalFlags import globalflags
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
log = logging.getLogger('MuonSetup')

### Output data name ###
from TrigEDMConfig.TriggerEDMRun3 import recordable
from MuonConfig.MuonBytestreamDecodeConfig import MuonCacheNames
from MuonConfig.MuonRdoDecodeConfig import MuonPrdCacheNames


theFTF_name = "FTFTracks_Muons" #Obsolete?
CBTPname = recordable("HLT_CBCombinedMuon_RoITrackParticles")
CBTPnameFS = recordable("HLT_CBCombinedMuon_FSTrackParticles")
ExtrpTPname = recordable("HLT_MSExtrapolatedMuons_RoITrackParticles")
ExtrpTPnameFS = recordable("HLT_MSExtrapolatedMuons_FSTrackParticles")
MSextrpTPname = recordable("HLT_MSOnlyExtrapolatedMuons_FSTrackParticles")
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.MuonSlice.doTrigMuonConfig=True

from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam

class muonNames(object):
  def __init__(self):
    #EFSA and EFCB containers have different names 
    #for RoI and FS running. Other containers are 
    #produced in RoIs only.

    self.L2SAName = recordable("HLT_MuonL2SAInfo")
    self.L2CBName = recordable("HLT_MuonL2CBInfo")
    self.EFSAName = "Muons"
    self.EFCBName = "MuonsCB"
    self.EFCBOutInName = "MuonsCBOutsideIn"
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

def isCosmic():
  #FIXME: this might not be ideal criteria to determine if this is cosmic chain but used to work in Run2 and will do for now, ATR-22758
  return (beamFlags.beamType() == 'cosmics')

def isLRT(name):
  return "LRT" in name

#Returns relevant track collection name
def getIDTracks(name=''):
  if isCosmic():
      return recordable("HLT_IDTrack_Cosmic_IDTrig") #IDTrig as we are already retrieving PT collection
  else:
      ### TODO This should be probably set in the IDTrigConfig, ATR-22755
      if isLRT(name): return recordable("HLT_IDTrack_MuonLRT_FTF") 
      else:           return recordable("HLT_IDTrack_Muon_FTF") 



### ==================== Data prepartion needed for the EF and L2 SA ==================== ###
def makeMuonPrepDataAlgs(RoIs="MURoIs", forFullScan=False):

  from AthenaCommon.CFElements import parOR

  postFix = "_%s" % RoIs
  muDecodeRecoSequence = parOR("decodeMuViewNode"+postFix)

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
                                                  RoIs                    = RoIs,
                                                  RegionSelectionTool=makeRegSelTool_CSC())

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
  from MuonCnvExample import MuonCalibConfig

  MdtRdoToMdtPrepDataTool = Muon__MdtRdoToPrepDataToolMT(name                     = "MdtRdoToPrepDataTool",
                                                         MdtPrdContainerCacheKey = MuonPrdCacheNames.MdtCache,
                                                         CalibrationTool=MuonCalibConfig.MdtCalibrationTool())

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
                                                RoIs = RoIs,
                                                RegionSelectionTool=makeRegSelTool_MDT())

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
                                                RoIs = RoIs,
                                                RegionSelectionTool=makeRegSelTool_RPC())

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
                                                RoIs             = RoIs,
                                                RegionSelectionTool = makeRegSelTool_TGC() )

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


def muFastRecoSequence( RoIs, doFullScanID = False, InsideOutMode=False, extraLoads=None, l2mtmode=False ):

  from AthenaCommon.AppMgr import ToolSvc
  from AthenaCommon.CFElements import parOR

  # muFastRecoSequence = parOR("l2MuViewNode")
  postFix = ""
  if InsideOutMode:
    postFix = "IOmode"
  elif l2mtmode:
    postFix = "l2mtmode"
  muFastRecoSequence = parOR("l2Mu"+postFix+"ViewNode")

  # In insideout mode, need to inherit muon decoding objects for TGC, RPC, MDT, CSC
  import AthenaCommon.CfgMgr as CfgMgr
  ViewVerify = CfgMgr.AthViews__ViewDataVerifier("muFastRecoVDV"+postFix)
  if InsideOutMode:
    ViewVerify.DataObjects = [('Muon::TgcPrepDataContainer','StoreGateSvc+TGC_Measurements'),
                              ('TgcRdoContainer' , 'StoreGateSvc+TGCRDO'),
                              ('Muon::RpcPrepDataContainer','StoreGateSvc+RPC_Measurements'),
                              ('Muon::MdtPrepDataContainer','StoreGateSvc+MDT_DriftCircles')]
    if MuonGeometryFlags.hasCSC():
      ViewVerify.DataObjects += [('Muon::CscPrepDataContainer','StoreGateSvc+CSC_Clusters')]
    if MuonGeometryFlags.hasSTGC():
      ViewVerify.DataObjects += [('Muon::sTgcPrepDataContainer','StoreGateSvc+STGC_Measurements')]
    if MuonGeometryFlags.hasMM():
      ViewVerify.DataObjects += [('Muon::MMPrepDataContainer','StoreGateSvc+MM_Measurements')]
    #muFastRecoSequence+=ViewVerify
  else:
    ViewVerify.DataObjects += [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % RoIs )]
  ViewVerify.DataObjects += [( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' )]
  if ConfigFlags.Trigger.enableL1Phase1:
    ViewVerify.DataObjects += [( 'xAOD::MuonRoIContainer' , 'StoreGateSvc+LVL1MuonRoIs' )]
  else:
    ViewVerify.DataObjects += [( 'DataVector< LVL1::RecMuonRoI >' , 'StoreGateSvc+HLT_RecMURoIs' )]

  #For L2 multi-track SA mode
  if extraLoads:
    ViewVerify.DataObjects += extraLoads
  
  muFastRecoSequence += ViewVerify

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

  from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConf import LVL1__TrigT1RPCRecRoiTool
  trigRpcRoiTool = LVL1__TrigT1RPCRecRoiTool("RPCRecRoiTool", UseRun3Config=ConfigFlags.Trigger.enableL1Phase1)

  ### RPC RDO data - turn off the data decoding here ###
  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__RpcDataPreparator
  L2RpcDataPreparator = TrigL2MuonSA__RpcDataPreparator(name = "L2MuonSARpcDataPreparator",
                                                        RpcPrepDataProvider  = "",
                                                        RpcPrepDataContainer = "RPC_Measurements",
                                                        RpcRawDataProvider   = "",
                                                        DoDecoding           = False,
                                                        DecodeBS             = False,
                                                        TrigT1RPCRecRoiTool  = trigRpcRoiTool)
  ToolSvc += L2RpcDataPreparator
  from RegionSelector.RegSelToolConfig import makeRegSelTool_RPC
  L2RpcDataPreparator.RegSel_RPC = makeRegSelTool_RPC()

  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__RpcClusterPreparator
  L2RpcClusterPreparator = TrigL2MuonSA__RpcClusterPreparator(name = "L2RpcClusterPreparator",
                                                              TrigT1RPCRecRoiTool = trigRpcRoiTool)
  ToolSvc += L2RpcClusterPreparator

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
  muFastAlg = TrigL2MuonSAMTConfig("Muon"+postFix)

  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__MuFastDataPreparator
  MuFastDataPreparator = TrigL2MuonSA__MuFastDataPreparator(TrigT1RPCRecRoiTool = trigRpcRoiTool)
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

  muFastAlg.Run2RecMuonRoI = "HLT_RecMURoIs"
  muFastAlg.RecMuonRoI = "LVL1MuonRoIs"
  muFastAlg.MuRoIs = RoIs
  muFastAlg.MuonL2SAInfo = muNames.L2SAName+postFix
  muFastAlg.L2IOCB = muNames.L2CBName+postFix
  muFastAlg.MuonCalibrationStream = "MuonCalibrationStream"+postFix
  muFastAlg.forID = muNames.L2forIDName+postFix
  muFastAlg.forMS = "forMS"+postFix
  muFastAlg.FILL_FSIDRoI = doFullScanID
  muFastAlg.InsideOutMode = InsideOutMode
  muFastAlg.TrackParticlesContainerName = getIDTracks()
  #Do not run topo road and inside-out mode at the same time
  if InsideOutMode:
    muFastAlg.topoRoad = False
  
  if l2mtmode:
    muFastAlg.multitrackMode = True
    muFastAlg.doEndcapForl2mt = False

  if ConfigFlags.Trigger.enableL1Phase1:
    muFastAlg.UseRun3Config = True
  else:
    muFastAlg.UseRun3Config = False

  muFastRecoSequence += muFastAlg
  sequenceOut = muFastAlg.MuonL2SAInfo

  return muFastRecoSequence, sequenceOut


def muonIDFastTrackingSequence( RoIs, name, extraLoads=None, extraLoadsForl2mtmode=None, doLRT=False ):

  from AthenaCommon.CFElements import parOR

  viewNodeName=name+"FastIDViewNode"

  muonIDFastTrackingSequence = parOR(viewNodeName)

  ### Define input data of Inner Detector algorithms  ###
  ### and Define EventViewNodes to run the algorithms ###
  from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
  IDTrigConfig = getInDetTrigConfig( "muon"+name ) 

  from TrigInDetConfig.InDetSetup import makeInDetAlgs
  viewAlgs, viewVerify = makeInDetAlgs( config = IDTrigConfig, rois = RoIs )
  viewVerify.DataObjects += [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % RoIs )]
  if extraLoads:
    viewVerify.DataObjects += extraLoads

  if extraLoadsForl2mtmode:
    viewVerify.DataObjects += extraLoadsForl2mtmode

  for viewAlg in viewAlgs:
      muonIDFastTrackingSequence += viewAlg

  return muonIDFastTrackingSequence

def muonIDCosmicTrackingSequence( RoIs, name, extraLoads=None ):

  from AthenaCommon.CFElements import parOR
  viewNodeName=name+"IDTrackingViewNode"

  trackingSequence = parOR(viewNodeName)

  from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
  IDTrigConfig = getInDetTrigConfig( "cosmics" )

  from TrigInDetConfig.InDetSetup import makeInDetAlgs
  dataPreparationAlgs, dataVerifier = makeInDetAlgs( config = IDTrigConfig, rois = RoIs, doFTF = False)
   
  dataVerifier.DataObjects += [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s'%RoIs )]

  from TrigInDetConfig.EFIDTracking import makeInDetPatternRecognition
  trackingAlgs, _ = makeInDetPatternRecognition( config  = IDTrigConfig, verifier = 'VDVCosmicIDTracking' )

  if extraLoads:
    dataVerifier.DataObjects += extraLoads

  for alg in dataPreparationAlgs:
      trackingSequence += alg

  for alg in trackingAlgs:
      trackingSequence += alg

  return trackingSequence




def muCombRecoSequence( RoIs, name, l2mtmode=False ):

  from AthenaCommon.CFElements import parOR
  postFix = ""
  if l2mtmode:
    postFix = "l2mtmode"

  muCombRecoSequence = parOR("l2muCombViewNode_"+name+postFix)
  ### A simple algorithm to confirm that data has been inherited from parent view ###
  ### Required to satisfy data dependencies                                       ###
  import AthenaCommon.CfgMgr as CfgMgr
  ViewVerify = CfgMgr.AthViews__ViewDataVerifier("muFast"+postFix+"ViewDataVerifier_"+name)
  ViewVerify.DataObjects = [('xAOD::L2StandAloneMuonContainer','StoreGateSvc+%s' % muNames.L2SAName+postFix)]

  muCombRecoSequence+=ViewVerify

  ### please read out TrigmuCombMTConfig file ###
  ### and set up to run muCombMT algorithm    ###
  from TrigmuComb.TrigmuCombMTConfig import TrigmuCombMTConfig
  muCombAlg = TrigmuCombMTConfig("Muon"+postFix,name)
  muCombAlg.L2StandAloneMuonContainerName = muNames.L2SAName+postFix
  muCombAlg.L2CombinedMuonContainerName   = muNames.L2CBName+postFix
  if l2mtmode:
    muCombAlg.TrackParticlesContainerName   = getIDTracks()
  else:
    muCombAlg.TrackParticlesContainerName   = getIDTracks(name)

  muCombRecoSequence += muCombAlg
  sequenceOut = muCombAlg.L2CombinedMuonContainerName

  return muCombRecoSequence, sequenceOut


def muEFSARecoSequence( RoIs, name ):

  import AthenaCommon.CfgGetter as CfgGetter

  from AthenaCommon import CfgMgr
  from AthenaCommon.CFElements import parOR
  from MuonRecExample.MuonStandalone import MooSegmentFinderAlg, MuonStandaloneTrackParticleCnvAlg, MuonSegmentFinderAlg
  from MuonCombinedRecExample.MuonCombinedAlgs import MuonCombinedMuonCandidateAlg, MuonCreatorAlg
  from MuonCombinedAlgs.MuonCombinedAlgsMonitoring import MuonCreatorAlgMonitoring

  muEFSARecoSequence = parOR("efmsViewNode_"+name)

  efAlgs = []

  EFMuonViewDataVerifier = CfgMgr.AthViews__ViewDataVerifier( "EFMuonViewDataVerifier_" + name )
  EFMuonViewDataVerifier.DataObjects = [( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                                        ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % RoIs )]

  efAlgs.append( EFMuonViewDataVerifier )

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

      theSegmentFinderAlg = MuonSegmentFinderAlg( "TrigMuonSegmentMaker_"+name)
  else:
    theSegmentFinderAlg = MooSegmentFinderAlg("TrigMuonSegmentMaker_"+name)

  from MuonSegmentTrackMaker.MuonTrackMakerAlgsMonitoring import MuPatTrackBuilderMonitoring
  TrackBuilder = CfgMgr.MuPatTrackBuilder("TrigMuPatTrackBuilder_"+name ,MuonSegmentCollection = "TrackMuonSegments", 
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
                              ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % RoIs ),
                              ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' )]
  if "FS" in name:
    ViewVerifyMS.DataObjects +=[( 'MuonCandidateCollection' , 'StoreGateSvc+MuonCandidates_FS' )]
  else:
    ViewVerifyMS.DataObjects +=[( 'MuonCandidateCollection' , 'StoreGateSvc+MuonCandidates')]

  if MuonGeometryFlags.hasCSC():
    ViewVerifyMS.DataObjects += [( 'Muon::CscStripPrepDataContainer' , 'StoreGateSvc+CSC_Measurements' )]
  if (MuonGeometryFlags.hasSTGC() and MuonGeometryFlags.hasMM()): 
    ViewVerifyMS.DataObjects += [( 'Muon::MMPrepDataContainer' , 'StoreGateSvc+MM_Measurements'),
                                ( 'Muon::sTgcPrepDataContainer' , 'StoreGateSvc+STGC_Measurements') ]

  muEFCBRecoSequence += ViewVerifyMS

  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()

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
    ViewVerifyTrk.DataObjects = [( 'xAOD::TrackParticleContainer' , 'StoreGateSvc+'+getIDTracks() ),
                                 ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_FlaggedCondData_TRIG' ),
                                 ( 'xAOD::IParticleContainer' , 'StoreGateSvc+'+ getIDTracks() )]

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
    muEFCBRecoSequence += PTSeq
    trackParticles = PTTrackParticles[-1]
  #In case of cosmic Precision Tracking has been already called before hence no need to call here just retrieve the correct collection of tracks
  elif isCosmic():
    trackParticles = getIDTracks() 
  else:
    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( config = IDTrigConfig, rois = RoIs,  verifier = ViewVerifyTrk )
    PTSeq = parOR("precisionTrackingInMuons", PTAlgs  )
    muEFCBRecoSequence += PTSeq
    trackParticles = PTTrackParticles[-1]


  #Make InDetCandidates
  theIndetCandidateAlg = MuonCombinedInDetCandidateAlg("TrigMuonCombinedInDetCandidateAlg_"+name,TrackParticleLocation = [trackParticles], 
                                                       InDetCandidateLocation="InDetCandidates_"+name, TrackSelector="")
  #No easy way to access AtlasHoleSearchTool in theIndetCandidateAlg
  from AthenaCommon.AppMgr import ToolSvc
  from InDetTrigRecExample.InDetTrigConditionsAccess import SCT_ConditionsSetup
  from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
  sct_ConditionsSummaryToolSetupWithoutFlagged = SCT_ConditionsSummaryToolSetup(SCT_ConditionsSetup.instanceName('InDetSCT_ConditionsSummaryToolWithoutFlagged'))
  sct_ConditionsSummaryToolSetupWithoutFlagged.setup()
  ToolSvc.AtlasHoleSearchTool.BoundaryCheckTool.SctSummaryTool = sct_ConditionsSummaryToolSetupWithoutFlagged.getTool()
  ToolSvc.CombinedMuonIDHoleSearch.BoundaryCheckTool.SctSummaryTool = sct_ConditionsSummaryToolSetupWithoutFlagged.getTool()

  #MS ID combination
  candidatesName = "MuonCandidates"
  if 'FS' in name:
    candidatesName = "MuonCandidates_FS"
  theMuonCombinedAlg = MuonCombinedAlg("TrigMuonCombinedAlg_"+name, MuonCandidateLocation=candidatesName, InDetCandidateLocation="InDetCandidates_"+name)


  cbMuonName = muNames.EFCBOutInName
  if 'FS' in name:
    cbMuonName = muNamesFS.EFCBName

  import AthenaCommon.CfgGetter as CfgGetter
  themuoncbcreatoralg = MuonCreatorAlg("TrigMuonCreatorAlgCB_"+name, MuonCandidateLocation=candidatesName, TagMaps=["muidcoTagMap"], InDetCandidateLocation="InDetCandidates_"+name,
                                       MuonContainerLocation = cbMuonName, SegmentContainerName = "xaodCBSegments", TrackSegmentContainerName = "TrkCBSegments", ExtrapolatedLocation = "CBExtrapolatedMuons",
                                       MSOnlyExtrapolatedLocation = "CBMSonlyExtrapolatedMuons", CombinedLocation = "HLT_CBCombinedMuon_"+name,
                                       MuonCreatorTool = CfgGetter.getPrivateToolClone("TrigMuonCreatorToolCB", "MuonCreatorTool", TrackSummaryTool = CfgGetter.getPublicTool("MuonTrackSummaryTool")),
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

  from MuonRecExample.MuonStandalone import MooSegmentFinderAlg, MuonSegmentFinderAlg
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

      if (MuonGeometryFlags.hasSTGC() and MuonGeometryFlags.hasMM()):
        theMuonLayerHough = CfgMgr.MuonLayerHoughAlg( "MuonLayerHoughAlg")
        efAlgs.append(theMuonLayerHough)
        theSegmentFinderAlg = MuonSegmentFinderAlg( "TrigMuonSegmentMaker_"+name)
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
    from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
    sct_ConditionsSummaryToolSetupWithoutFlagged = SCT_ConditionsSummaryToolSetup(SCT_ConditionsSetup.instanceName('InDetSCT_ConditionsSummaryToolWithoutFlagged'))
    sct_ConditionsSummaryToolSetupWithoutFlagged.setup()
    ToolSvc.AtlasHoleSearchTool.BoundaryCheckTool.SctSummaryTool = sct_ConditionsSummaryToolSetupWithoutFlagged.getTool()
    ToolSvc.CombinedMuonIDHoleSearch.BoundaryCheckTool.SctSummaryTool = sct_ConditionsSummaryToolSetupWithoutFlagged.getTool()

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
                                         MuonContainerLocation = cbMuonName, SegmentContainerName = "xaodInsideOutCBSegments", TrackSegmentContainerName = "TrkInsideOutCBSegments", ExtrapolatedLocation = "InsideOutCBExtrapolatedMuons",
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

  from AthenaCommon.CFElements import parOR

  efmuisoRecoSequence = parOR("efmuIsoViewNode")

  from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
  IDTrigConfig = getInDetTrigConfig( 'muonIso' )

  from TrigInDetConfig.InDetSetup import makeInDetAlgs
  viewAlgs, viewVerify = makeInDetAlgs( config = IDTrigConfig, rois = RoIs )
  viewVerify.DataObjects += [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+MUEFIsoRoIs' ),
                             ( 'xAOD::MuonContainer' , 'StoreGateSvc+IsoViewMuons' )]

  # Make sure required objects are still available at whole-event level
  if not globalflags.InputFormat.is_bytestream():
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
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
  trigEFmuIso.ptcone02Name = "%s.ptcone02" % Muons
  trigEFmuIso.ptcone03Name = "%s.ptcone03" % Muons

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
