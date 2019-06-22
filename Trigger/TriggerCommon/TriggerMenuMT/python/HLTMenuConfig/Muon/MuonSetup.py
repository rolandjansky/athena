#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
from AthenaCommon.GlobalFlags import globalflags
log = logging.getLogger('MuonSetup')

### Output data name ###
from TrigEDMConfig.TriggerEDMRun3 import recordable
from AthenaCommon.DetFlags import DetFlags

TrackParticlesName = recordable("HLT_xAODTracks_Muon")

class muonNames(object):
  def __init__(self):
    #EFSA and EFCB containers have different names 
    #for RoI and FS running. Other containers are 
    #produced in RoIs only.

    self.L2SAName = "MuonL2SAInfo"
    self.L2CBName = "MuonL2CBInfo"
    self.EFSAName = "Muons"
    self.EFCBName = "MuonsCB"
    self.EFCBOutInName = "MuonsCBOutsideIn"
    self.EFCBInOutName = "MuonsCBInsideOut"
    self.L2IsoMuonName = "MuonsL2ISInfo"
    self.EFIsoMuonName = "MuonsIso"

  def getNames(self, name):

    if "FS" in name:
      self.EFSAName = "Muons_FS"
      self.EFCBName = "MuonsCB_FS"
    if "RoI" in name:
      self.EFSAName = "Muons_RoI"
      self.EFCBName = "MuonsCB_RoI"
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

  from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CSC_RawDataProviderTool
  MuonCscRawDataProviderTool = Muon__CSC_RawDataProviderTool(name        = "CSC_RawDataProviderTool",
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
                                                             Decoder = RPCRodDecoder )
  ToolSvc += MuonRpcRawDataProviderTool

  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRdoToPrepDataTool
  RpcRdoToRpcPrepDataTool = Muon__RpcRdoToPrepDataTool(name                = "RpcRdoToPrepDataTool")

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
  if DetFlags.readRDOBS.MDT_on():
    muFastRecoSequence += MdtRawDataProviderAlg
  if DetFlags.readRDOBS.TGC_on():
    muFastRecoSequence += TgcRawDataProviderAlg
  if DetFlags.readRDOBS.CSC_on():
    muFastRecoSequence += CscRawDataProviderAlg
  # Always need RDO->PRD
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


  ### RPC RDO data ###
  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcROD_Decoder
  RPCRodDecoder = Muon__RpcROD_Decoder(name        = "RpcROD_Decoder_L2SA")
  ToolSvc += RPCRodDecoder

  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RPC_RawDataProviderToolMT
  MuonRpcRawDataProviderTool = Muon__RPC_RawDataProviderToolMT(name        = "RPC_RawDataProviderToolMT_L2SA",
                                                               RdoLocation = "RPCPAD_L2SA",
                                                               RPCSec      = "RPC_SECTORLOGIC_L2SA",
                                                               Decoder     = RPCRodDecoder)
  ToolSvc += MuonRpcRawDataProviderTool

  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRdoToPrepDataTool
  RpcRdoToRpcPrepDataTool = Muon__RpcRdoToPrepDataTool(name                    = "RpcRdoToPrepDataTool_L2SA",
                                                       OutputCollection        = MuonRpcRawDataProviderTool.RdoLocation,
                                                       TriggerOutputCollection = "RPC_Measurements_L2SA",
                                                       InputCollection         = "RPC_triggerHits_L2SA")
  ToolSvc += RpcRdoToRpcPrepDataTool

  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__RpcDataPreparator
  L2RpcDataPreparator = TrigL2MuonSA__RpcDataPreparator(RpcPrepDataProvider  = RpcRdoToRpcPrepDataTool,
                                                        RpcPrepDataContainer = RpcRdoToRpcPrepDataTool.TriggerOutputCollection,
                                                        RpcRawDataProvider   = MuonRpcRawDataProviderTool,
                                                        DecodeBS             = DetFlags.readRDOBS.RPC_on())
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

def muonIDFastTrackingSequence( RoIs ):

  from AthenaCommon.CFElements import parOR
  import AthenaCommon.CfgMgr as CfgMgr

  muonIDFastTrackingSequence = parOR("l2muCombViewNode")

  ### Define input data of Inner Detector algorithms  ###
  ### and Define EventViewNodes to run the algorithms ###
  from TriggerMenuMT.HLTMenuConfig.CommonSequences.InDetSetup import makeInDetAlgs
  (viewAlgs, eventAlgs) = makeInDetAlgs("Muon")


  from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Muon
  theFTF_Muon = TrigFastTrackFinder_Muon()
  theFTF_Muon.isRoI_Seeded = True
  viewAlgs.append(theFTF_Muon)

  ### A simple algorithm to confirm that data has been inherited from parent view ###
  ### Required to satisfy data dependencies                                       ###
  ViewVerify = CfgMgr.AthViews__ViewDataVerifier("muFastViewDataVerifier")
  ViewVerify.DataObjects = [('xAOD::L2StandAloneMuonContainer','StoreGateSvc+'+muNames.L2SAName)]
  viewAlgs.append(ViewVerify)
  global TrackParticlesName

  #TrackParticlesName = ""
  for viewAlg in viewAlgs:
      muonIDFastTrackingSequence += viewAlg
      if "RoIs" in viewAlg.properties():
          viewAlg.RoIs = RoIs
      if "roiCollectionName" in viewAlg.properties():
          viewAlg.roiCollectionName = RoIs
      if "InDetTrigTrackParticleCreatorAlg" in  viewAlg.name():
          TrackParticlesName = viewAlg.TrackParticlesName
          TrackCollection = viewAlg.TrackName

  theFTF_Muon.TracksName=TrackCollection

  return muonIDFastTrackingSequence, eventAlgs, TrackParticlesName, theFTF_Muon.getName()

def muCombRecoSequence( RoIs ):

  muCombRecoSequence, eventAlgs, TrackParticlesName, theFTF_Muon_Name = muonIDFastTrackingSequence( RoIs )

  ### please read out TrigmuCombMTConfig file ###
  ### and set up to run muCombMT algorithm    ###
  from TrigmuComb.TrigmuCombMTConfig import TrigmuCombMTConfig
  muCombAlg = TrigmuCombMTConfig("Muon", theFTF_Muon_Name)
  muCombAlg.L2StandAloneMuonContainerName = muNames.L2SAName
  muCombAlg.TrackParticlesContainerName = TrackParticlesName
  muCombAlg.L2CombinedMuonContainerName = muNames.L2CBName

  muCombRecoSequence += muCombAlg
  sequenceOut = muCombAlg.L2CombinedMuonContainerName

  return muCombRecoSequence, eventAlgs, sequenceOut, TrackParticlesName


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

  from MuonRecExample.MuonRecFlags import muonRecFlags
  from AthenaCommon.AppMgr import ServiceMgr
  import AthenaCommon.CfgGetter as CfgGetter

  from AthenaCommon.CfgGetter import getPublicTool, getPublicToolClone
  from AthenaCommon import CfgMgr
  from AthenaCommon.CFElements import parOR

  muEFSARecoSequence = parOR("efmsViewNode_"+name)

  efAlgs = []

  ### Provide Muon_PrepDataAlgorithms ###
  from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup  import makeMuonPrepDataAlgs
  ( eventAlgs_MuonPRD, viewAlgs_MuonPRD ) = makeMuonPrepDataAlgs( name == 'FS')

  # setup RDO preparator algorithms 
  if name != 'FS':
    # we now try to share the data preparation algorithms with L2, so we tell the view that it should expect the MDT and TGC PRDs to be available
    efAlgs.append( CfgMgr.AthViews__ViewDataVerifier(name = "EFMuonViewDataVerifier",
                                                     DataObjects = [( 'Muon::MdtPrepDataContainer'      , 'StoreGateSvc+MDT_DriftCircles' ),
                                                                    ( 'Muon::TgcPrepDataContainer'      , 'StoreGateSvc+TGC_Measurements' ),
                                                                    ( 'Muon::CscStripPrepDataContainer' , 'StoreGateSvc+CSC_Measurements'),
                                                                    ( 'Muon::CscPrepDataContainer'      , 'StoreGateSvc+CSC_Clusters') ] )

                 )
  for viewAlg_MuonPRD in viewAlgs_MuonPRD:
    # we now try to share the MDT, CSC and TGC data preparation algorithms with L2, so only add those if we are running full-scane
    # this is slightly ugly, should be improved in new JO setup
    if ('Mdt' not in viewAlg_MuonPRD.name() and 'Tgc' not in  viewAlg_MuonPRD.name() and 'Csc' not in viewAlg_MuonPRD.name()) or name == 'FS':
      efAlgs.append( viewAlg_MuonPRD )
   
  from TrkDetDescrSvc.TrkDetDescrSvcConf import Trk__TrackingVolumesSvc
  ServiceMgr += Trk__TrackingVolumesSvc("TrackingVolumesSvc",BuildVolumesFromTagInfo = False)

  theSegmentFinder = CfgGetter.getPublicToolClone("MuonSegmentFinder","MooSegmentFinder")
  CfgGetter.getPublicTool("MuonLayerHoughTool").DoTruth=False
  theSegmentFinderAlg=CfgMgr.MooSegmentFinderAlg( "MuonSegmentMaker_"+name,
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
                                                  doRPCClust = muonRecFlags.doRPCClusterSegmentFinding())



  #theNCBSegmentFinderAlg=CfgMgr.MooSegmentFinderAlg( "MuonSegmentMaker_NCB_"+name,
  #                                                   SegmentFinder = getPublicToolClone("MooSegmentFinder_NCB","MuonSegmentFinder",
  #                                                                                      DoSummary=False,
  #                                                                                      Csc2dSegmentMaker = getPublicToolClone("Csc2dSegmentMaker_NCB","Csc2dSegmentMaker",
  #                                                                                                                             segmentTool = getPublicToolClone("CscSegmentUtilTool_NCB",
  #                                                                                                                                                              "CscSegmentUtilTool",
  #                                                                                                                                                              TightenChi2 = False,
  #                                                                                                                                                              IPconstraint=False)),
  #                                                                                      Csc4dSegmentMaker = getPublicToolClone("Csc4dSegmentMaker_NCB","Csc4dSegmentMaker",
  #                                                                                                                             segmentTool = getPublicTool("CscSegmentUtilTool_NCB")),
  #                                                                                      DoMdtSegments=False,DoSegmentCombinations=False,DoSegmentCombinationCleaning=False),
  #                                                   MuonPatternCombinationLocation = "NCB_MuonHoughPatternCombinations",
  #                                                   MuonSegmentOutputLocation = "NCB_MuonSegments",
  #                                                   UseCSC = muonRecFlags.doCSCs(),
  #                                                   UseMDT = False,
  #                                                   UseRPC = False,
  #                                                   UseTGC = False,
  #                                                   UseTGCPriorBC = False,
  #                                                   UseTGCNextBC  = False,
  #                                                   doTGCClust = False,
  #                                                   doRPCClust = False)

  TrackBuilder = CfgMgr.MuPatTrackBuilder("MuPatTrackBuilder" )
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


  theCandidateTool = getPublicToolClone("MuonCandidateTool_SA", "MuonCandidateTool", TrackBuilder="",ExtrapolationStrategy=1)

  theMuonCandidateAlg=CfgMgr.MuonCombinedMuonCandidateAlg("MuonCandidateAlg_"+name,MuonCandidateTool=theCandidateTool)


  muonparticlecreator = getPublicToolClone("MuonParticleCreator", "TrackParticleCreatorTool", UseTrackSummaryTool=False, UseMuonSummaryTool=True, KeepAllPerigee=True)
  theTrackQueryNoFit = getPublicToolClone("TrigMuonTrackQueryNoFitSA", "MuonTrackQuery", Fitter="")
  thecreatortool= getPublicToolClone("MuonCreatorTool_SA", "MuonCreatorTool", ScatteringAngleTool="", MuonSelectionTool="", FillTimingInformation=False, UseCaloCells=False, MakeSAMuons=True, MomentumBalanceTool="",  TrackParticleCreator=muonparticlecreator,TrackQuery=theTrackQueryNoFit)

  msMuonName = muNames.EFSAName
  if 'FS' in name:
    msMuonName = muNamesFS.EFSAName
  themuoncreatoralg = CfgMgr.MuonCreatorAlg("MuonCreatorAlg_"+name, MuonCreatorTool=thecreatortool, CreateSAmuons=True, MakeClusters=False, TagMaps=[], MuonContainerLocation=msMuonName )

  #Algorithms to views
  efAlgs.append( theSegmentFinderAlg )
  #efAlgs.append( theNCBSegmentFinderAlg ) #The configuration still needs some sorting out for this so disabled for now.
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
  ViewVerifyMS.DataObjects = [( 'Muon::CscStripPrepDataContainer' , 'StoreGateSvc+CSC_Measurements' ),  ( 'Muon::MdtPrepDataContainer' , 'StoreGateSvc+MDT_DriftCircles' ),  ( 'xAOD::TrackParticleContainer' , 'StoreGateSvc+MuonSpectrometerTrackParticles' ) ]
  muEFCBRecoSequence += ViewVerifyMS
  eventAlgs=[]
  if "FS" in name:
    #Need to run tracking for full scan chains
    from TriggerMenuMT.HLTMenuConfig.CommonSequences.InDetSetup import makeInDetAlgs
    (viewAlgs, eventAlgs) = makeInDetAlgs("MuonFS") 

    from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_MuonFS
    theFTF_Muon = TrigFastTrackFinder_MuonFS()
    theFTF_Muon.isRoI_Seeded = True
    viewAlgs.append(theFTF_Muon)

     #TrackParticlesName = ""
    for viewAlg in viewAlgs:
      muEFCBRecoSequence += viewAlg
      if "RoIs" in viewAlg.properties():
        viewAlg.RoIs = RoIs
      if "roiCollectionName" in viewAlg.properties():
        viewAlg.roiCollectionName = RoIs
      if "InDetTrigTrackParticleCreatorAlg" in viewAlg.name():
        TrackParticlesName = viewAlg.TrackParticlesName  # noqa: F841
        TrackCollection = viewAlg.TrackName

    theFTF_Muon.TracksName=TrackCollection
  else:
    TrackCollection="TrigFastTrackFinder_Tracks_Muon" # this is hacking, please FIX IT
    ViewVerifyTrk = CfgMgr.AthViews__ViewDataVerifier("muonCBIDViewDataVerifier")
    ViewVerifyTrk.DataObjects = [( 'xAOD::TrackParticleContainer' , 'StoreGateSvc+'+TrackParticlesName ),
                                 ( 'TrackCollection' , 'StoreGateSvc+'+TrackCollection ),
                                 ( 'SCT_FlaggedCondData' , 'StoreGateSvc+SCT_FlaggedCondData' ),
                                 ( 'xAOD::IParticleContainer' , 'StoreGateSvc+'+TrackParticlesName )]

    if globalflags.InputFormat.is_bytestream():
      ViewVerifyTrk.DataObjects += [( 'InDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' ),
                                    ( 'SCT_ByteStreamFractionContainer' , 'StoreGateSvc+SCT_ByteStreamFrac' ) ]
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

  #MuonCombinedCandidates
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
  trkSummaryTool =  Trk__TrackSummaryTool( 'TrigMuonTrackSummary',MuonSummaryHelperTool=getPublicTool('MuonTrackSummaryHelper'), doSharedHits=False)
  if DetFlags.detdescr.ID_on():
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryHelperTool
    trkSummaryTool.InDetSummaryHelperTool=InDetTrigTrackSummaryHelperTool
    trkSummaryTool.doHolesInDet=True
  ToolSvc += Trk__TrackSummaryTool('TrigMuonTrackSummary')

  theTrackQueryNoFit = getPublicToolClone("TrigMuonTrackQueryNoFit_"+name, "MuonTrackQuery", Fitter="")
  theTrackBuilderTool = getPublicToolClone("TrigCombinedMuonTrackBuilder_"+name,"CombinedMuonTrackBuilder", UseCaloTG = True, CaloTSOS=theCaloTSOS, CaloMaterialProvider='TMEF_TrkMaterialProviderTool', MuonHoleRecovery="",CaloEnergyParam=theCaloEnergyTool,MuonErrorOptimizer=theErrorOptimiser, Fitter='TMEF_iPatFitter', MaterialAllocator="TMEF_MaterialAllocator", Propagator=TrigMuonPropagator, LargeMomentumError=0.5, PerigeeAtSpectrometerEntrance=True, ReallocateMaterial=False, TrackSummaryTool=trkSummaryTool, Cleaner=theTrackCleaner,TrackQuery=theTrackQueryNoFit)
  theTrackQuery = getPublicToolClone("TrigMuonTrackQuery_"+name, "MuonTrackQuery", Fitter=theTrackBuilderTool)

  theCandidateToolCB = getPublicToolClone("TrigMuonCandidateTool_CB_"+name, "MuonCandidateTool", TrackBuilder=theTrackBuilderTool)
  theMuonCombinedCandidateAlg = CfgMgr.MuonCombinedMuonCandidateAlg("TrigMuonCombinedMuonCandidateAlg_"+name,MuonCandidateTool=theCandidateToolCB,MuonCandidateLocation="CombinedMuonCandidates")

  #MS ID combination
  theMuonCombinedFitTagTool = getPublicToolClone("TrigMuonCombinedFitTagTool_"+name, "MuonCombinedFitTagTool",TrackBuilder=theTrackBuilderTool,MuonRecovery=getPublicToolClone("TrigMuonRecovery_"+name,"MuidMuonRecovery", TrackBuilder=theTrackBuilderTool), TrackQuery=theTrackQuery, MatchQuality = getPublicToolClone("TrigMuonMatchQuality_"+name, "MuonMatchQuality", TrackQuery=theTrackQuery))
  tools=[]
  tools.append(theMuonCombinedFitTagTool)
  theMuonCombinedTool = getPublicToolClone("TrigMuonCombinedToolCB_"+name, "MuonCombinedTool", MuonCombinedTagTools=tools)
  theMuonCombinedAlg = CfgMgr.MuonCombinedAlg("TrigMuonCombinedAlg_"+name, MuonCandidateLocation="CombinedMuonCandidates", MuonCombinedTool=theMuonCombinedTool, CombinedTagMaps=["muidcoTagMap"], InDetCandidateLocation="InDetCandidates_"+name)

  #Build muon candidates
  theCandidateToolCB = getPublicToolClone("MuonCandidateTool_CB_"+name, "MuonCandidateTool", TrackBuilder=theTrackBuilderTool)
  theMuonCandidateAlgCB=CfgMgr.MuonCombinedMuonCandidateAlg("MuonCandidateAlgCB_"+name,MuonCandidateTool=theCandidateToolCB,MuonCandidateLocation="MuonCandidates", MSOnlyExtrapolatedTrackLocation="MSOnlyMuonTracksForCB")

  #Create xAOD Muons
  muonparticlecreator = getPublicToolClone("MuonParticleCreatorCB_"+name, "TrackParticleCreatorTool", UseTrackSummaryTool=False, UseMuonSummaryTool=True, KeepAllPerigee=True)
  thecreatortoolCB= getPublicToolClone("MuonCreatorTool_triggerCB_"+name, "MuonCreatorTool", ScatteringAngleTool="", CaloMaterialProvider='TMEF_TrkMaterialProviderTool', MuonSelectionTool="", FillTimingInformation=False, UseCaloCells=False,TrackQuery=theTrackQueryNoFit,TrackParticleCreator=muonparticlecreator)

  cbMuonName = muNames.EFCBOutInName
  if 'FS' in name:
    cbMuonName = muNamesFS.EFCBName

  themuoncbcreatoralg = CfgMgr.MuonCreatorAlg("MuonCreatorAlgCB_"+name, MuonCandidateLocation="CombinedMuonCandidates", TagMaps=["muidcoTagMap"], InDetCandidateLocation="InDetCandidates_"+name)
  themuoncbcreatoralg.MuonCreatorTool=thecreatortoolCB
  themuoncbcreatoralg.MakeClusters=False
  themuoncbcreatoralg.ClusterContainerName=""
  themuoncbcreatoralg.MuonContainerLocation = cbMuonName
  themuoncbcreatoralg.SegmentContainerName = "CBSegments"
  themuoncbcreatoralg.ExtrapolatedLocation = "CBExtrapolatedMuons"
  themuoncbcreatoralg.MSOnlyExtrapolatedLocation = "CBMSOnlyExtrapolatedMuons"
  themuoncbcreatoralg.CombinedLocation = "CBCombinedMuon"

  #Add all algorithms
  efAlgs.append(theIndetCandidateAlg)
  efAlgs.append(theMuonCombinedCandidateAlg)
  efAlgs.append(theMuonCombinedAlg)
  efAlgs.append(theMuonCandidateAlgCB)
  efAlgs.append(themuoncbcreatoralg)


  # setup muEFMsonly algs
  for efAlg in efAlgs:
    muEFCBRecoSequence += efAlg
  sequenceOut = themuoncbcreatoralg.MuonContainerLocation


  return muEFCBRecoSequence, eventAlgs, sequenceOut


def muEFInsideOutRecoSequence(RoIs, name):

  from AthenaCommon.CFElements import parOR
  from AthenaCommon import CfgMgr
  from AthenaCommon.CfgGetter import getPublicTool, getPublicToolClone

  from AthenaCommon.AppMgr import ToolSvc
  import AthenaCommon.CfgGetter as CfgGetter

  efAlgs = []

  efmuInsideOutRecoSequence = parOR("efmuInsideOutViewNode")

  #Need PRD containers for inside-out reco
  ViewVerifyInsideOut = CfgMgr.AthViews__ViewDataVerifier("muonInsideOutViewDataVerifier")
  ViewVerifyInsideOut.DataObjects = [( 'Muon::MdtPrepDataContainer' , 'StoreGateSvc+MDT_DriftCircles' ),
                              ( 'Muon::CscPrepDataContainer' , 'StoreGateSvc+CSC_Clusters' ),
                              ( 'Muon::CscStripPrepDataContainer' , 'StoreGateSvc+CSC_Measurements' ),
                              ( 'Muon::RpcPrepDataContainer' , 'StoreGateSvc+RPC_Measurements' ),
                              ( 'Muon::TgcPrepDataContainer' , 'StoreGateSvc+TGC_Measurements' )]
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
  trkSummaryTool =  Trk__TrackSummaryTool( 'TrigMuonTrackSummary',MuonSummaryHelperTool=getPublicTool('MuonTrackSummaryHelper'), doSharedHits=False)
  if DetFlags.detdescr.ID_on():
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryHelperTool
    trkSummaryTool.InDetSummaryHelperTool=InDetTrigTrackSummaryHelperTool
    trkSummaryTool.doHolesInDet=True
  ToolSvc += Trk__TrackSummaryTool('TrigMuonTrackSummary')
  theTrackBuilderTool = getPublicToolClone("TrigCombinedMuonTrackBuilder","CombinedMuonTrackBuilder", UseCaloTG = True, CaloTSOS=theCaloTSOS, CaloMaterialProvider='TMEF_TrkMaterialProviderTool', MuonHoleRecovery="",CaloEnergyParam=theCaloEnergyTool,MuonErrorOptimizer=theErrorOptimiser, Fitter='TMEF_iPatFitter', MaterialAllocator="TMEF_MaterialAllocator", Propagator=TrigMuonPropagator, LargeMomentumError=0.5, PerigeeAtSpectrometerEntrance=True, ReallocateMaterial=False, TrackSummaryTool=trkSummaryTool, Cleaner=theTrackCleaner)


  #Inside-out reconstruction

  theMuonCandidateTrackBuilderTool = getPublicToolClone("TrigMuonCandidateTrackBuilderTool", "MuonCandidateTrackBuilderTool", MuonTrackBuilder=theTrackBuilderTool,)
  insideOutRecoTool = getPublicToolClone("TrigMuonInsideOutRecoTool", "MuonInsideOutRecoTool",MuonTrackBuilder=theTrackBuilderTool,MuonCandidateTrackBuilderTool=theMuonCandidateTrackBuilderTool)

  theInsideOutRecoAlg = CfgMgr.MuonCombinedInDetExtensionAlg("TrigMuonInsideOutRecoAlg",InDetCandidateLocation="InDetCandidates_"+name,MuonCombinedInDetExtensionTools=[insideOutRecoTool],usePRDs=True)

  #Create the xAOD muons
  cbMuonName = muNames.EFCBInOutName

  muonparticlecreator = getPublicToolClone("MuonParticleCreatorInsideOut", "TrackParticleCreatorTool", UseTrackSummaryTool=False, UseMuonSummaryTool=True, KeepAllPerigee=True)
  theTrackQueryNoFit = getPublicToolClone("TrigMuonInsideOutTrackQueryNoFit", "MuonTrackQuery", Fitter="")
  thecreatortool= getPublicToolClone("MuonCreatorTool_triggerInsideOut", "MuonCreatorTool", ScatteringAngleTool="", CaloMaterialProvider='TMEF_TrkMaterialProviderTool', MuonSelectionTool="", FillTimingInformation=False, UseCaloCells=False,TrackQuery=theTrackQueryNoFit,TrackParticleCreator=muonparticlecreator)
  insideoutcreatoralg = CfgMgr.MuonCreatorAlg("MuonCreatorAlgInsideOut",BuildSlowMuon=True, TagMaps=["muGirlTagMap"],InDetCandidateLocation="InDetCandidates_RoI_"+name)
  insideoutcreatoralg.MuonCreatorTool=thecreatortool
  insideoutcreatoralg.MakeClusters=False
  insideoutcreatoralg.ClusterContainerName=""
  insideoutcreatoralg.MuonContainerLocation = cbMuonName
  insideoutcreatoralg.SegmentContainerName = "CBSegments_"+name
  insideoutcreatoralg.ExtrapolatedLocation = "CBExtrapolatedMuons_"+name
  insideoutcreatoralg.MSOnlyExtrapolatedLocation = "CBMSOnlyExtrapolatedMuons_"+name
  insideoutcreatoralg.CombinedLocation = "CBCombinedMuon_"+name

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
  (viewAlgs, eventAlgs) = makeInDetAlgs("MuonIso")

  from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_MuonIso
  theFTF_Muon = TrigFastTrackFinder_MuonIso()
  theFTF_Muon.isRoI_Seeded = True
  viewAlgs.append(theFTF_Muon)

  #TrackParticlesName = ""
  for viewAlg in viewAlgs:
    efmuisoRecoSequence += viewAlg
    if "RoIs" in viewAlg.properties():
      viewAlg.RoIs = RoIs
    if "roiCollectionName" in viewAlg.properties():
      viewAlg.roiCollectionName = RoIs
    if "InDetTrigTrackParticleCreatorAlg" in viewAlg.name():
        TrackParticlesName = viewAlg.TrackParticlesName  # noqa: F841
        TrackCollection = viewAlg.TrackName

  theFTF_Muon.TracksName=TrackCollection
  
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
