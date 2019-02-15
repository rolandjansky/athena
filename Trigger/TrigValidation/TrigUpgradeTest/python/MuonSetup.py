#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Constants import VERBOSE,DEBUG, INFO

### Output data name ###
muFastInfo = "MuonL2SAInfo"
muCombInfo = "MuonL2CBInfo"
muEFSAInfo = "Muons"
muL2ISInfo = "MuonL2ISInfo"
TrackParticlesName = "xAODTracks"

### ==================== Data prepartion needed for the EF and L2 SA ==================== ###
def makeMuonPrepDataAlgs():

  eventAlgs_MuonPRD = [] # These algs should be prepared for configuring RoIs same as muon RoIs used in viewAlg.
  viewAlgs_MuonPRD = []  # These algs should be executed to prepare muon PRDs for muFast and muEF steps.

  from MuonRecExample.MuonRecFlags import muonRecFlags
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
  CscRdoToCscPrepData = CscRdoToCscPrepData(name                    = "CscRdoToCscPrepData",
                                            CscRdoToCscPrepDataTool = CscRdoToCscPrepDataTool,
                                            PrintPrepData           = False, 
                                            DoSeededDecoding        = True,
                                            RoIs                    = "MURoIs" )

 
  from MuonByteStream.MuonByteStreamConf import Muon__CscRawDataProvider
  CscRawDataProvider = Muon__CscRawDataProvider(name         = "CscRawDataProvider",
                                                ProviderTool = MuonCscRawDataProviderTool )

  from CscClusterization.CscClusterizationConf import CscThresholdClusterBuilderTool
  CscClusterBuilderTool = CscThresholdClusterBuilderTool(name        = "CscThesholdClusterBuilderTool" )
  ToolSvc += CscClusterBuilderTool

  #CSC cluster building
  from CscClusterization.CscClusterizationConf import CscThresholdClusterBuilder
  CscClusterBuilder = CscThresholdClusterBuilder(name            = "CscThesholdClusterBuilder",
                                                 cluster_builder = CscClusterBuilderTool)    

  eventAlgs_MuonPRD.append( CscRdoToCscPrepData )
  viewAlgs_MuonPRD.append( CscRawDataProvider )  
  viewAlgs_MuonPRD.append( CscRdoToCscPrepData )  
  viewAlgs_MuonPRD.append( CscClusterBuilder ) 

 
  ### MDT RDO data ###
  from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import MdtROD_Decoder
  MDTRodDecoder = MdtROD_Decoder(name	   = "MdtROD_Decoder" )
  ToolSvc += MDTRodDecoder

  from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MDT_RawDataProviderTool
  MuonMdtRawDataProviderTool = Muon__MDT_RawDataProviderTool(name        = "MDT_RawDataProviderTool",
                                                             Decoder     = MDTRodDecoder )
  ToolSvc += MuonMdtRawDataProviderTool

  from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MdtRdoToPrepDataTool
  MdtRdoToMdtPrepDataTool = Muon__MdtRdoToPrepDataTool(name                = "MdtRdoToPrepDataTool")
                                                       #OutputLevel         = INFO )
  ToolSvc += MdtRdoToMdtPrepDataTool

  from MuonRdoToPrepData.MuonRdoToPrepDataConf import MdtRdoToMdtPrepData
  MdtRdoToMdtPrepData = MdtRdoToMdtPrepData(name             = "MdtRdoToMdtPrepData",
                                            DecodingTool     = MdtRdoToMdtPrepDataTool,
                                            PrintPrepData    = False,
                                            DoSeededDecoding = True,
                                            RoIs             = "MURoIs")
  
  
  from MuonByteStream.MuonByteStreamConf import Muon__MdtRawDataProvider
  MdtRawDataProvider = Muon__MdtRawDataProvider(name         = "MdtRawDataProvider",
                                                ProviderTool = MuonMdtRawDataProviderTool )

  eventAlgs_MuonPRD.append( MdtRdoToMdtPrepData )
  viewAlgs_MuonPRD.append( MdtRawDataProvider )
  viewAlgs_MuonPRD.append( MdtRdoToMdtPrepData )


  ### RPC RDO data ###
  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcROD_Decoder
  RPCRodDecoder = Muon__RpcROD_Decoder(name	     = "RpcROD_Decoder" )
  ToolSvc += RPCRodDecoder

  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RPC_RawDataProviderTool
  MuonRpcRawDataProviderTool = Muon__RPC_RawDataProviderTool(name    = "RPC_RawDataProviderTool",
                                                             Decoder = RPCRodDecoder )
  ToolSvc += MuonRpcRawDataProviderTool

  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRdoToPrepDataTool
  RpcRdoToRpcPrepDataTool = Muon__RpcRdoToPrepDataTool(name                = "RpcRdoToPrepDataTool")

  ToolSvc += RpcRdoToRpcPrepDataTool

  from MuonRdoToPrepData.MuonRdoToPrepDataConf import RpcRdoToRpcPrepData
  RpcRdoToRpcPrepData = RpcRdoToRpcPrepData(name             = "RpcRdoToRpcPrepData",
                                            DecodingTool     = RpcRdoToRpcPrepDataTool,
                                            PrintPrepData    = False,
                                            DoSeededDecoding = True,
                                            RoIs             = "MURoIs")

  from MuonByteStream.MuonByteStreamConf import Muon__RpcRawDataProvider
  RpcRawDataProvider = Muon__RpcRawDataProvider(name         = "RpcRawDataProvider",
                                                ProviderTool = MuonRpcRawDataProviderTool,
                                                DoSeededDecoding = True,
                                                RoIs = "MURoIs")

  eventAlgs_MuonPRD.append( RpcRawDataProvider )
  eventAlgs_MuonPRD.append( RpcRdoToRpcPrepData )
  viewAlgs_MuonPRD.append( RpcRawDataProvider )
  viewAlgs_MuonPRD.append( RpcRdoToRpcPrepData )


  ### TGC RDO data ###
  from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TGC_RodDecoderReadout
  TGCRodDecoder = Muon__TGC_RodDecoderReadout(name	    = "TGC_RodDecoderReadout" )
  ToolSvc += TGCRodDecoder

  from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TGC_RawDataProviderTool
  MuonTgcRawDataProviderTool = Muon__TGC_RawDataProviderTool(name    = "TGC_RawDataProviderTool",
                                                             Decoder = TGCRodDecoder )
  ToolSvc += MuonTgcRawDataProviderTool
    
  from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TgcRdoToPrepDataTool
  TgcRdoToTgcPrepDataTool = Muon__TgcRdoToPrepDataTool(name                = "TgcRdoToPrepDataTool")
  ToolSvc += TgcRdoToTgcPrepDataTool

  from MuonRdoToPrepData.MuonRdoToPrepDataConf import TgcRdoToTgcPrepData
  TgcRdoToTgcPrepData = TgcRdoToTgcPrepData(name             = "TgcRdoToTgcPrepData",
                                            DecodingTool     = TgcRdoToTgcPrepDataTool,
                                            PrintPrepData    = False,
                                            DoSeededDecoding = True,
                                            RoIs             = "MURoIs")

  from MuonByteStream.MuonByteStreamConf import Muon__TgcRawDataProvider
  TgcRawDataProvider = Muon__TgcRawDataProvider(name         = "TgcRawDataProvider",
                                                ProviderTool = MuonTgcRawDataProviderTool )

  eventAlgs_MuonPRD.append( TgcRawDataProvider )
  eventAlgs_MuonPRD.append( TgcRdoToTgcPrepData )
  viewAlgs_MuonPRD.append( TgcRawDataProvider )
  viewAlgs_MuonPRD.append( TgcRdoToTgcPrepData )

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
def muFastRecoSequence( RoIs, OutputLevel=INFO ):
   

  from MuonRecExample.MuonRecFlags import muonRecFlags
  from AthenaCommon.AppMgr import ToolSvc
  from AthenaCommon.DetFlags import DetFlags
  from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq

  muFastRecoSequence = parOR("l2MuViewNode")

  ### These configurations for decoding tools should be removed. ###
  ### CSC RDO data ###
  from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscROD_Decoder
  CSCRodDecoder = Muon__CscROD_Decoder(name		= "CscROD_Decoder_L2SA",
                                       IsCosmics	= False,
                                       IsOldCosmics 	= False,
                                       OutputLevel      = OutputLevel )
  ToolSvc += CSCRodDecoder


  from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CSC_RawDataProviderTool
  MuonCscRawDataProviderTool = Muon__CSC_RawDataProviderTool(name        = "CSC_RawDataProviderTool_L2SA",
                                                             RdoLocation = "CSCRDO_L2SA",
                                                             Decoder     = CSCRodDecoder,
                                                             OutputLevel = OutputLevel )
  ToolSvc += MuonCscRawDataProviderTool

  from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscRdoToCscPrepDataTool
  CscRdoToCscPrepDataTool = Muon__CscRdoToCscPrepDataTool(name                = "CscRdoToCscPrepDataTool_L2SA",
                                                          RDOContainer        = MuonCscRawDataProviderTool.RdoLocation,
                                                          OutputCollection    = "CSC_Measurements_L2SA",
                                                          OutputLevel         = OutputLevel )
  ToolSvc += CscRdoToCscPrepDataTool


  from CscClusterization.CscClusterizationConf import CscThresholdClusterBuilderTool
  CscClusterBuilderTool = CscThresholdClusterBuilderTool(name        = "CscThesholdClusterBuilderTool_L2SA",
                                                         digit_key   = CscRdoToCscPrepDataTool.OutputCollection,
                                                         cluster_key = "CSC_Clusters_L2SA",
                                                         OutputLevel = OutputLevel )

  ToolSvc += CscClusterBuilderTool

  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__CscDataPreparator
  L2CscDataPreparator = TrigL2MuonSA__CscDataPreparator(CscPrepDataProvider  = CscRdoToCscPrepDataTool,
                                                        CscClusterProvider   = CscClusterBuilderTool,
                                                        CSCPrepDataContainer = CscClusterBuilderTool.cluster_key,
                                                        OutputLevel          = OutputLevel )
  ToolSvc += L2CscDataPreparator

 
  ### MDT RDO data ###
  from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import MdtROD_Decoder
  MDTRodDecoder = MdtROD_Decoder(name	     = "MdtROD_Decoder_L2SA",
                                 OutputLevel = OutputLevel )

  ToolSvc += MDTRodDecoder

  from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MDT_RawDataProviderTool
  MuonMdtRawDataProviderTool = Muon__MDT_RawDataProviderTool(name        = "MDT_RawDataProviderTool_L2SA",
                                                             RdoLocation = "MDTCSM_L2SA",
                                                             Decoder     = MDTRodDecoder,
                                                             OutputLevel = OutputLevel )
  ToolSvc += MuonMdtRawDataProviderTool

  from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MdtRdoToPrepDataTool
  MdtRdoToMdtPrepDataTool = Muon__MdtRdoToPrepDataTool(name                = "MdtRdoToPrepDataTool_L2SA",
                                                       RDOContainer        = MuonMdtRawDataProviderTool.RdoLocation,
                                                       OutputCollection    = "MDT_DriftCircles_L2SA",
                                                       OutputLevel         = OutputLevel )
  ToolSvc += MdtRdoToMdtPrepDataTool

 
  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__MdtDataPreparator
  L2MdtDataPreparator = TrigL2MuonSA__MdtDataPreparator(DecodeBS = DetFlags.readRDOBS.MDT_on(),
                                                        MdtPrepDataProvider  = MdtRdoToMdtPrepDataTool,
                                                        MDTPrepDataContainer = MdtRdoToMdtPrepDataTool.OutputCollection,
                                                        MDT_RawDataProvider  = MuonMdtRawDataProviderTool,
                                                        MDTCSMContainer      = MuonMdtRawDataProviderTool.RdoLocation,
                                                        OutputLevel          = OutputLevel )
  ToolSvc += L2MdtDataPreparator
  

  ### RPC RDO data ###
  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcROD_Decoder
  RPCRodDecoder = Muon__RpcROD_Decoder(name        = "RpcROD_Decoder_L2SA",
                                       OutputLevel = OutputLevel )

  ToolSvc += RPCRodDecoder

  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RPC_RawDataProviderTool
  MuonRpcRawDataProviderTool = Muon__RPC_RawDataProviderTool(name        = "RPC_RawDataProviderTool_L2SA",
                                                             RdoLocation = "RPCPAD_L2SA",
                                                             RPCSec      = "RPC_SECTORLOGIC_L2SA",
                                                             Decoder     = RPCRodDecoder,
                                                             OutputLevel = OutputLevel )
  ToolSvc += MuonRpcRawDataProviderTool

  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRdoToPrepDataTool
  RpcRdoToRpcPrepDataTool = Muon__RpcRdoToPrepDataTool(name                    = "RpcRdoToPrepDataTool_L2SA",
                                                       OutputCollection        = MuonRpcRawDataProviderTool.RdoLocation,
                                                       TriggerOutputCollection = "RPC_Measurements_L2SA",
                                                       InputCollection         = "RPC_triggerHits_L2SA",
                                                       OutputLevel             = OutputLevel )
  ToolSvc += RpcRdoToRpcPrepDataTool

  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__RpcDataPreparator
  L2RpcDataPreparator = TrigL2MuonSA__RpcDataPreparator(RpcPrepDataProvider  = RpcRdoToRpcPrepDataTool,
                                                        RpcPrepDataContainer = RpcRdoToRpcPrepDataTool.TriggerOutputCollection,
                                                        RpcRawDataProvider   = MuonRpcRawDataProviderTool,
                                                        DecodeBS             = DetFlags.readRDOBS.RPC_on(),
                                                        OutputLevel          = OutputLevel )
  ToolSvc += L2RpcDataPreparator


  ### TGC RDO data ###
  from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TGC_RodDecoderReadout
  TGCRodDecoder = Muon__TGC_RodDecoderReadout(name	  = "TGC_RodDecoderReadout_L2SA",
                                              OutputLevel = OutputLevel )

  ToolSvc += TGCRodDecoder

  from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TGC_RawDataProviderTool
  MuonTgcRawDataProviderTool = Muon__TGC_RawDataProviderTool(name        = "TGC_RawDataProviderTool_L2SA",
                                                             RdoLocation = "TGCRDO_L2SA",
                                                             Decoder     = TGCRodDecoder,
                                                             OutputLevel = OutputLevel )
  ToolSvc += MuonTgcRawDataProviderTool
    
  from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TgcRdoToPrepDataTool
  TgcRdoToTgcPrepDataTool = Muon__TgcRdoToPrepDataTool(name                 = "TgcRdoToPrepDataTool_L2SA",
                                                       OutputCollection     = "TGC_Measurements_L2SA",
                                                       OutputCoinCollection = "TrigT1CoinDataCollection_L2SA",
                                                       OutputLevel = OutputLevel )

  ToolSvc += TgcRdoToTgcPrepDataTool

  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__TgcDataPreparator
  L2TgcDataPreparator = TrigL2MuonSA__TgcDataPreparator(TgcPrepDataProvider  = TgcRdoToTgcPrepDataTool,
                                                        TGCPrepDataContainer = TgcRdoToTgcPrepDataTool.OutputCollection,
                                                        OutputLevel          = OutputLevel )
  ToolSvc += L2TgcDataPreparator


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
  muFastAlg.OutputLevel    = OutputLevel
  muFastAlg.OutputLevel = DEBUG
  muFastAlg.RecMuonRoI = "RecMURoIs"
  muFastAlg.MuRoIs = RoIs
  muFastAlg.MuonL2SAInfo = muFastInfo
  muFastAlg.MuonCalibrationStream = "MuonCalibrationStream"
  muFastAlg.forID = "forID"
  muFastAlg.forMS = "forMS"

  muFastRecoSequence += muFastAlg
  sequenceOut = muFastAlg.MuonL2SAInfo

  return muFastRecoSequence, sequenceOut


def muCombRecoSequence( RoIs, OutputLevel=INFO ):
   

  from MuonRecExample.MuonRecFlags import muonRecFlags
  from AthenaCommon.AppMgr import ToolSvc
  from AthenaCommon.DetFlags import DetFlags
  from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq
  import AthenaCommon.CfgMgr as CfgMgr

  muCombRecoSequence = parOR("l2muCombViewNode")

  ### Define input data of Inner Detector algorithms  ###
  ### and Define EventViewNodes to run the algorithms ###
  from TrigUpgradeTest.InDetSetup import makeInDetAlgs
  (viewAlgs, eventAlgs) = makeInDetAlgs()

  from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Muon
  theFTF_Muon = TrigFastTrackFinder_Muon()
  theFTF_Muon.OutputLevel = OutputLevel
  theFTF_Muon.isRoI_Seeded = True
  viewAlgs.append(theFTF_Muon)

  ### A simple algorithm to confirm that data has been inherited from parent view ###
  ### Required to satisfy data dependencies                                       ###
  ViewVerify = CfgMgr.AthViews__ViewDataVerifier("muFastViewDataVerifier")
  ViewVerify.DataObjects = [('xAOD::L2StandAloneMuonContainer','StoreGateSvc+'+muFastInfo)]
  viewAlgs.append(ViewVerify)

  #TrackParticlesName = ""
  for viewAlg in viewAlgs:
      muCombRecoSequence += viewAlg
      viewAlg.OutputLevel = OutputLevel
      if viewAlg.properties().has_key("RoIs"):
          viewAlg.RoIs = RoIs
      if viewAlg.properties().has_key("roiCollectionName"):
          viewAlg.roiCollectionName = RoIs
      if viewAlg.name() == "InDetTrigTrackParticleCreatorAlg":
          TrackParticlesName = viewAlg.TrackParticlesName

  ### please read out TrigmuCombMTConfig file ###
  ### and set up to run muCombMT algorithm    ###
  from TrigmuComb.TrigmuCombMTConfig import TrigmuCombMTConfig
  muCombAlg = TrigmuCombMTConfig("Muon", theFTF_Muon.getName())
  muCombAlg.OutputLevel = OutputLevel
  muCombAlg.L2StandAloneMuonContainerName = muFastInfo
  muCombAlg.TrackParticlesContainerName = TrackParticlesName
  muCombAlg.L2CombinedMuonContainerName = muCombInfo

  muCombRecoSequence += muCombAlg
  sequenceOut = muCombAlg.L2CombinedMuonContainerName

  return muCombRecoSequence, eventAlgs, sequenceOut, TrackParticlesName


def l2muisoRecoSequence( RoIs, OutputLevel=INFO ):

  import AthenaCommon.CfgMgr as CfgMgr

  from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq

  l2muisoRecoSequence = parOR("l2muIsoViewNode")
 
  ViewVerify = CfgMgr.AthViews__ViewDataVerifier("muCombViewDataVerifier")
  ViewVerify.DataObjects = [('xAOD::TrackParticleContainer' , 'StoreGateSvc+'+TrackParticlesName),
                            ('xAOD::L2CombinedMuonContainer','StoreGateSvc+'+muCombInfo)]

  l2muisoRecoSequence += ViewVerify

  # set up algs
  from TrigmuIso.TrigmuIsoConfig import TrigmuIsoMTConfig
  trigL2muIso = TrigmuIsoMTConfig("TrigL2muIso")
  trigL2muIso.OutputLevel = OutputLevel
  trigL2muIso.MuonL2CBInfoName = muCombInfo
  trigL2muIso.TrackParticlesName = TrackParticlesName
  trigL2muIso.MuonL2ISInfoName = muL2ISInfo

  l2muisoRecoSequence += trigL2muIso

  sequenceOut = trigL2muIso.MuonL2ISInfoName

  return l2muisoRecoSequence, sequenceOut


def muEFSARecoSequence( RoIs, OutputLevel=INFO ):

  from MuonRecExample.MuonRecFlags import muonRecFlags
  from AthenaCommon.DetFlags import DetFlags
  from AthenaCommon.AppMgr import ToolSvc
  from AthenaCommon.AppMgr import ServiceMgr
  import AthenaCommon.CfgMgr as CfgMgr
  import AthenaCommon.CfgGetter as CfgGetter

  from AthenaCommon.CfgGetter import getPublicTool, getPublicToolClone
  from AthenaCommon import CfgMgr
  from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq

  muEFSARecoSequence = parOR("efmsViewNode")
 
  efAlgs = [] 
  
  ### Provide Muon_PrepDataAlgorithms ###
  from TrigUpgradeTest.MuonSetup import makeMuonPrepDataAlgs
  ( eventAlgs_MuonPRD, viewAlgs_MuonPRD ) = makeMuonPrepDataAlgs()

  # setup RDO preparator algorithms 
  for viewAlg_MuonPRD in viewAlgs_MuonPRD:
    efAlgs.append( viewAlg_MuonPRD )
   
  from TrkDetDescrSvc.TrkDetDescrSvcConf import Trk__TrackingVolumesSvc
  ServiceMgr += Trk__TrackingVolumesSvc("TrackingVolumesSvc",BuildVolumesFromTagInfo = False, OutputLevel=OutputLevel )
  
  theSegmentFinder = CfgGetter.getPublicToolClone("MuonSegmentFinder","MooSegmentFinder", OutputLevel=OutputLevel )
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
                                                  doRPCClust = muonRecFlags.doRPCClusterSegmentFinding(), OutputLevel=OutputLevel )
  
  
  
  theNCBSegmentFinderAlg=CfgMgr.MooSegmentFinderAlg( "MuonSegmentMaker_NCB",
                                                     OutputLevel = OutputLevel,
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
  TrackBuilder.TrackSteering=CfgGetter.getPublicToolClone("TrigMuonTrackSteering", "MuonTrackSteering")
  
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
  
  
  theCandidateTool = getPublicToolClone("MuonCandidateTool_SA", "MuonCandidateTool", TrackBuilder="",ExtrapolationStrategy=1)

  theMuonCandidateAlg=CfgMgr.MuonCombinedMuonCandidateAlg("MuonCandidateAlg",MuonCandidateTool=theCandidateTool)
  
  
  muonparticlecreator = getPublicToolClone("MuonParticleCreator", "TrackParticleCreatorTool", UseTrackSummaryTool=False, UseMuonSummaryTool=True, KeepAllPerigee=True)
  thecreatortool= getPublicToolClone("MuonCreatorTool_SA", "MuonCreatorTool", ScatteringAngleTool="", MuonSelectionTool="", FillTimingInformation=False, UseCaloCells=False, MakeSAMuons=True, MomentumBalanceTool="",  TrackParticleCreator=muonparticlecreator, OutputLevel=DEBUG)
  
  themuoncreatoralg = CfgMgr.MuonCreatorAlg("MuonCreatorAlg", MuonCreatorTool=thecreatortool, CreateSAmuons=True, MakeClusters=False, TagMaps=[], MuonContainerLocation=muEFSAInfo)
  
  #Algorithms to views
  efAlgs.append( theSegmentFinderAlg )
  #efAlgs.append( theNCBSegmentFinderAlg ) #The configuration still needs some sorting out for this so disabled for now.
  efAlgs.append( TrackBuilder )
  efAlgs.append( xAODTrackParticleCnvAlg )
  efAlgs.append( theMuonCandidateAlg )
  efAlgs.append( themuoncreatoralg )

  # setup muEFMsonly algs
  for efAlg in efAlgs:
      if efAlg.properties().has_key("RoIs"):
          efAlg.RoIs = RoIs
      muEFSARecoSequence += efAlg
  sequenceOut = themuoncreatoralg.MuonContainerLocation

  
  return muEFSARecoSequence, sequenceOut




def muEFCBRecoSequence( RoIs, OutputLevel=INFO ):

  from MuonRecExample.MuonRecFlags import muonRecFlags
  from AthenaCommon.DetFlags import DetFlags
  from AthenaCommon.AppMgr import ToolSvc
  from AthenaCommon.AppMgr import ServiceMgr
  import AthenaCommon.CfgMgr as CfgMgr
  import AthenaCommon.CfgGetter as CfgGetter

  from AthenaCommon.CfgGetter import getPublicTool, getPublicToolClone
  from AthenaCommon import CfgMgr
  from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq

  efAlgs = [] 
  muEFCBRecoSequence = parOR("efcbViewNode")
  #Need ID tracking related objects and MS tracks from previous steps
  ViewVerifyTrk = CfgMgr.AthViews__ViewDataVerifier("muonCBViewDataVerifier")
  ViewVerifyTrk.DataObjects = [( 'xAOD::TrackParticleContainer' , 'StoreGateSvc+xAODTracks' ),( 'SCT_FlaggedCondData' , 'StoreGateSvc+SCT_FlaggedCondData' ), ( 'InDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' ), ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),( 'xAOD::IParticleContainer' , 'StoreGateSvc+xAODTracks' ),( 'SCT_ByteStreamFractionContainer' , 'StoreGateSvc+SCT_ByteStreamFrac' ),( 'Muon::CscStripPrepDataContainer' , 'StoreGateSvc+CSC_Measurements' ),  ( 'Muon::MdtPrepDataContainer' , 'StoreGateSvc+MDT_DriftCircles' ),  ( 'xAOD::TrackParticleContainer' , 'StoreGateSvc+MuonSpectrometerTrackParticles' ) ]
  muEFCBRecoSequence += ViewVerifyTrk

  #Make InDetCandidates
  theIndetCandidateAlg = CfgMgr.MuonCombinedInDetCandidateAlg("TrigMuonCombinedInDetCandidateAlg",TrackSelector=getPublicTool("MuonCombinedInDetDetailedTrackSelectorTool"),TrackParticleLocation = ["xAODTracks"],ForwardParticleLocation="xAODTracks",OutputLevel=DEBUG)

  #MuonCombinedCandidates
  theCaloMeasTool = getPublicToolClone("TrigCaloMeasTool", "MuidCaloEnergyMeas", CaloNoiseTool="", UseCaloNoiseTool=False,CellContainerLocation="")
  theCaloEnergyTool = getPublicToolClone("TrigCaloEnergyTool", "MuidCaloEnergyTool", CaloMeasTool = theCaloMeasTool, EnergyLossMeasurement=False, MopParametrization=True, TrackIsolation=False)

  from TrkExRungeKuttaIntersector.TrkExRungeKuttaIntersectorConf import Trk__IntersectorWrapper as Propagator
  TrigMuonPropagator = Propagator(name = 'TrigMuonPropagator')
  ToolSvc += TrigMuonPropagator

  theCaloTSOS = getPublicToolClone("TrigCaloTrackStateOnSurface", "MuidCaloTrackStateOnSurface", CaloEnergyDeposit=theCaloEnergyTool, CaloEnergyParam=theCaloEnergyTool, Propagator =TrigMuonPropagator, MinRemainingEnergy= 200, ParamPtCut= 3000)
  from MuidCaloScatteringTools.MuidCaloScatteringToolsConf import Rec__MuidMaterialEffectsOnTrackProvider
  Rec__MuidMaterialEffectsOnTrackProvider.TSOSTool=theCaloTSOS

  theErrorOptimiser = getPublicToolClone("TrigMuonErrorOptimiser", "MuonErrorOptimisationTool", PrepareForFit=False, RecreateStartingParameters=False,RefitTool=getPublicToolClone("TrigMuidRefitTool", "MuonRefitTool", AlignmentErrors = False, Fitter = CfgGetter.getPublicTool("iPatFitter")))

  theTrackCleaner = getPublicToolClone("TrigMuonTrackCleaner", "MuonTrackCleaner", Fitter='TMEF_iPatFitter', SLFitter='TMEF_iPatFitter')


  theTrackBuilderTool = getPublicToolClone("TrigCombinedMuonTrackBuilder","CombinedMuonTrackBuilder", UseCaloTG = True, CaloTSOS=theCaloTSOS, CaloMaterialProvider='TMEF_TrkMaterialProviderTool', MuonHoleRecovery="",OutputLevel=DEBUG,CaloEnergyParam=theCaloEnergyTool,MuonErrorOptimizer=theErrorOptimiser, Fitter='TMEF_iPatFitter', MaterialAllocator="TMEF_MaterialAllocator", Propagator=TrigMuonPropagator, LargeMomentumError=0.5, PerigeeAtSpectrometerEntrance=True, ReallocateMaterial=False, TrackSummaryTool=getPublicTool("CombinedMuonTrackSummary"), Cleaner=theTrackCleaner)
  theTrackQuery = getPublicToolClone("TrigMuonTrackQuery", "MuonTrackQuery", Fitter=theTrackBuilderTool)

  theCandidateToolCB = getPublicToolClone("TrigMuonCandidateTool_CB", "MuonCandidateTool", TrackBuilder=theTrackBuilderTool,OutputLevel=DEBUG)
  theMuonCombinedCandidateAlg = CfgMgr.MuonCombinedMuonCandidateAlg("TrigMuonCombinedMuonCandidateAlg",MuonCandidateTool=theCandidateToolCB,MuonCandidateLocation="CombinedMuonCandidates", OutputLevel=DEBUG)

  #MS ID combination
  theMuonCombinedFitTagTool = getPublicToolClone("TrigMuonCombinedFitTagTool", "MuonCombinedFitTagTool",TrackBuilder=theTrackBuilderTool,MuonRecovery=getPublicToolClone("TrigMuonRecovery","MuidMuonRecovery", TrackBuilder=theTrackBuilderTool),OutputLevel=DEBUG, TrackQuery=theTrackQuery, MatchQuality = getPublicToolClone("TrigMuonMatchQuality", "MuonMatchQuality", TrackQuery=theTrackQuery))
  tools=[]
  tools.append(theMuonCombinedFitTagTool)
  theMuonCombinedTool = getPublicToolClone("TrigMuonCombinedToolCB", "MuonCombinedTool", MuonCombinedTagTools=tools, OutputLevel=DEBUG)
  theMuonCombinedAlg = CfgMgr.MuonCombinedAlg("TrigMuonCombinedAlg", MuonCandidateLocation="CombinedMuonCandidates", MuonCombinedTool=theMuonCombinedTool, CombinedTagMaps=["muidcoTagMap"], OutputLevel=DEBUG)

  #Build muon candidates
  theCandidateToolCB = getPublicToolClone("MuonCandidateTool_CB", "MuonCandidateTool", TrackBuilder=theTrackBuilderTool)
  theMuonCandidateAlgCB=CfgMgr.MuonCombinedMuonCandidateAlg("MuonCandidateAlgCB",MuonCandidateTool=theCandidateToolCB,MuonCandidateLocation="MuonCandidates", MSOnlyExtrapolatedTrackLocation="MSOnlyMuonTracksForCB")

  #Create xAOD Muons
  thecreatortoolCB= getPublicToolClone("MuonCreatorTool_triggerCB", "MuonCreatorTool", ScatteringAngleTool="", CaloMaterialProvider='TMEF_TrkMaterialProviderTool', MuonSelectionTool="", FillTimingInformation=False, OutputLevel=DEBUG, DoCaloNoiseCut=False, UseCaloCells=False)

  themuoncbcreatoralg = CfgMgr.MuonCreatorAlg("MuonCreatorAlgCB", OutputLevel=DEBUG, MuonCandidateLocation="CombinedMuonCandidates")
  themuoncbcreatoralg.MuonCreatorTool=thecreatortoolCB
  themuoncbcreatoralg.MakeClusters=False
  themuoncbcreatoralg.ClusterContainerName=""
  themuoncbcreatoralg.MuonContainerLocation = "CBMuons"
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

  
  return muEFCBRecoSequence, sequenceOut


