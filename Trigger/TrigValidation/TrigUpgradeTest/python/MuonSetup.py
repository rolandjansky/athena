#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#


### ==================== Data prepartion needed for the EF and L2 SA #######################333
def makeMuonPrepDataAlgs():

  eventAlgs_Muon = []
  viewAlgs_MuL2SA = []
  viewAlgs_MuEFSA = []

  from MuonRecExample.MuonRecFlags import muonRecFlags
  from AthenaCommon.AppMgr import ToolSvc

  print " ***************** muonRecFlags status ***************** "
  print "                   doCSCs() == %s " %muonRecFlags.doCSCs()
  print "                   doMDTs() == %s " %muonRecFlags.doMDTs()
  print "                   doRPCs() == %s " %muonRecFlags.doRPCs()
  print "                   doTGCs() == %s " %muonRecFlags.doTGCs()
  print " ******************************************************* "

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
  CscRdoToCscPrepDataTool = Muon__CscRdoToCscPrepDataTool(name                = "CscRdoToCscPrepDataTool",
                                                          RawDataProviderTool = MuonCscRawDataProviderTool,
                                                          useBStoRdoTool      = True)
  ToolSvc += CscRdoToCscPrepDataTool

  from MuonRdoToPrepData.MuonRdoToPrepDataConf import CscRdoToCscPrepData
  CscRdoToCscPrepData = CscRdoToCscPrepData(name                    = "CscRdoToCscPrepData",
                                            CscRdoToCscPrepDataTool = CscRdoToCscPrepDataTool,
                                            PrintPrepData	      = False )
  CscRdoToCscPrepData.DoSeededDecoding = True
  CscRdoToCscPrepData.RoIs = "MURoIs"

  eventAlgs_Muon.append( CscRdoToCscPrepData )
 
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
  
  viewAlgs_MuEFSA.append( CscRdoToCscPrepData )  
  viewAlgs_MuEFSA.append( CscClusterBuilder ) 
  viewAlgs_MuL2SA.append( CscRdoToCscPrepDataTool )  
 
  ### MDT RDO data ###
  from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import MdtROD_Decoder
  MDTRodDecoder = MdtROD_Decoder(name	   = "MdtROD_Decoder" )
  ToolSvc += MDTRodDecoder

  from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MDT_RawDataProviderTool
  MuonMdtRawDataProviderTool = Muon__MDT_RawDataProviderTool(name        = "MDT_RawDataProviderTool",
                                                             Decoder     = MDTRodDecoder )
  ToolSvc += MuonMdtRawDataProviderTool

  from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MdtRdoToPrepDataTool
  MdtRdoToMdtPrepDataTool = Muon__MdtRdoToPrepDataTool(name                = "MdtRdoToPrepDataTool",
                                                       RawDataProviderTool = MuonMdtRawDataProviderTool,
                                                       useBStoRdoTool      = True)
  ToolSvc += MdtRdoToMdtPrepDataTool

  from MuonRdoToPrepData.MuonRdoToPrepDataConf import MdtRdoToMdtPrepData
  MdtRdoToMdtPrepData = MdtRdoToMdtPrepData(name          = "MdtRdoToMdtPrepData",
                                            DecodingTool  = MdtRdoToMdtPrepDataTool,
                                            PrintPrepData = False )
  MdtRdoToMdtPrepData.DoSeededDecoding = True
  MdtRdoToMdtPrepData.RoIs = "MURoIs"
  
  eventAlgs_Muon.append( MdtRdoToMdtPrepData )
  
  from MuonByteStream.MuonByteStreamConf import Muon__MdtRawDataProvider
  MdtRawDataProvider = Muon__MdtRawDataProvider(name         = "MdtRawDataProvider",
                                                ProviderTool = MuonMdtRawDataProviderTool )
  viewAlgs_MuEFSA.append( MdtRdoToMdtPrepData )
  viewAlgs_MuL2SA.append( MdtRdoToMdtPrepDataTool )
  viewAlgs_MuL2SA.append( MuonMdtRawDataProviderTool )

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
                                                       #RawDataProviderTool = MuonRpcRawDataProviderTool,
                                                       #useBStoRdoTool      = True)
  ToolSvc += RpcRdoToRpcPrepDataTool

  from MuonRdoToPrepData.MuonRdoToPrepDataConf import RpcRdoToRpcPrepData
  RpcRdoToRpcPrepData = RpcRdoToRpcPrepData(name          = "RpcRdoToRpcPrepData",
                                            DecodingTool  = RpcRdoToRpcPrepDataTool,
                                            PrintPrepData = False )
  RpcRdoToRpcPrepData.DoSeededDecoding = True
  RpcRdoToRpcPrepData.RoIs = "MURoIs"

  eventAlgs_Muon.append( RpcRdoToRpcPrepData )

  from MuonByteStream.MuonByteStreamConf import Muon__RpcRawDataProvider
  RpcRawDataProvider = Muon__RpcRawDataProvider(name         = "RpcRawDataProvider",
                                                ProviderTool = MuonRpcRawDataProviderTool,
                                                DoSeededDecoding = True,
                                                RoIs = "MURoIs")
  viewAlgs_MuEFSA.append( RpcRawDataProvider )
  viewAlgs_MuEFSA.append( RpcRdoToRpcPrepData )
  viewAlgs_MuL2SA.append( RpcRdoToRpcPrepDataTool )
  viewAlgs_MuL2SA.append( MuonRpcRawDataProviderTool )

  ### TGC RDO data ###
  from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TGC_RodDecoderReadout
  TGCRodDecoder = Muon__TGC_RodDecoderReadout(name	    = "TGC_RodDecoderReadout" )
  ToolSvc += TGCRodDecoder

  from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TGC_RawDataProviderTool
  MuonTgcRawDataProviderTool = Muon__TGC_RawDataProviderTool(name    = "TGC_RawDataProviderTool",
                                                             Decoder = TGCRodDecoder )
  ToolSvc += MuonTgcRawDataProviderTool
    
  from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TgcRdoToPrepDataTool
  TgcRdoToTgcPrepDataTool = Muon__TgcRdoToPrepDataTool(name                = "TgcRdoToPrepDataTool",
                                                       RawDataProviderTool = MuonTgcRawDataProviderTool,
                                                       useBStoRdoTool      = True)

  ToolSvc += TgcRdoToTgcPrepDataTool

  from MuonRdoToPrepData.MuonRdoToPrepDataConf import TgcRdoToTgcPrepData
  TgcRdoToTgcPrepData = TgcRdoToTgcPrepData(name          = "TgcRdoToTgcPrepData",
                                            DecodingTool  = TgcRdoToTgcPrepDataTool,
                                            PrintPrepData = False )
  TgcRdoToTgcPrepData.DoSeededDecoding = True
  TgcRdoToTgcPrepData.RoIs = "MURoIs"

  eventAlgs_Muon.append( TgcRdoToTgcPrepData )

  from MuonByteStream.MuonByteStreamConf import Muon__TgcRawDataProvider
  TgcRawDataProvider = Muon__TgcRawDataProvider(name         = "TgcRawDataProvider",
                                                ProviderTool = MuonTgcRawDataProviderTool )
  viewAlgs_MuEFSA.append( TgcRdoToTgcPrepData )
  viewAlgs_MuL2SA.append( TgcRdoToTgcPrepDataTool )
  viewAlgs_MuL2SA.append( MuonTgcRawDataProviderTool )


  if muonRecFlags.doCreateClusters():  
    #Run clustering
    from MuonClusterization.MuonClusterizationConf import MuonClusterizationTool
    MuonClusterTool = MuonClusterizationTool(name        = "MuonClusterizationTool" )
    ToolSvc += MuonClusterTool
    
    from MuonClusterization.MuonClusterizationConf import MuonClusterizationAlg
    MuonClusterAlg = MuonClusterizationAlg(name                 = "MuonClusterizationAlg",
                                           ClusterTool          = MuonClusterTool,
                                           TgcPrepDataContainer = "TGC_MeasurementsAllBCs")
    
    viewAlgs_MuEFSA.append( MuonClusterAlg )


  return ( eventAlgs_Muon, viewAlgs_MuL2SA, viewAlgs_MuEFSA )


# This def should be removed in the future. 
# We have not yet integrated muon decoding tool, so this def is used now.
def makeMuFastAlgs():
   

  from MuonRecExample.MuonRecFlags import muonRecFlags
  from AthenaCommon.AppMgr import ToolSvc
  from AthenaCommon.DetFlags import DetFlags

  ### CSC RDO data ###
  from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscROD_Decoder
  CSCRodDecoder = Muon__CscROD_Decoder(name		= "CscROD_Decoder_L2SA",
                                       IsCosmics	= False,
                                       IsOldCosmics 	= False )
  ToolSvc += CSCRodDecoder


  from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CSC_RawDataProviderTool
  MuonCscRawDataProviderTool = Muon__CSC_RawDataProviderTool(name        = "CSC_RawDataProviderTool_L2SA",
                                                             RdoLocation = "CSCRDO_L2SA",
                                                             Decoder     = CSCRodDecoder )
  ToolSvc += MuonCscRawDataProviderTool

  from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscRdoToCscPrepDataTool
  CscRdoToCscPrepDataTool = Muon__CscRdoToCscPrepDataTool(name                = "CscRdoToCscPrepDataTool_L2SA",
                                                          RawDataProviderTool = MuonCscRawDataProviderTool,
                                                          RDOContainer        = MuonCscRawDataProviderTool.RdoLocation,
                                                          OutputCollection    = "CSC_Measurements_L2SA",
                                                          useBStoRdoTool      = True)
  ToolSvc += CscRdoToCscPrepDataTool


  from CscClusterization.CscClusterizationConf import CscThresholdClusterBuilderTool
  CscClusterBuilderTool = CscThresholdClusterBuilderTool(name        = "CscThesholdClusterBuilderTool_L2SA",
                                                         digit_key   = CscRdoToCscPrepDataTool.OutputCollection,
                                                         cluster_key = "CSC_Clusters_L2SA" )

  ToolSvc += CscClusterBuilderTool

  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__CscDataPreparator
  L2CscDataPreparator = TrigL2MuonSA__CscDataPreparator(#OutputLevel = DEBUG,
                                                        CscPrepDataProvider  = CscRdoToCscPrepDataTool,
                                                        CscClusterProvider   = CscClusterBuilderTool,
                                                        CSCPrepDataContainer = CscClusterBuilderTool.cluster_key)
  ToolSvc += L2CscDataPreparator

 
  ### MDT RDO data ###
  from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import MdtROD_Decoder
  MDTRodDecoder = MdtROD_Decoder(name	   = "MdtROD_Decoder_L2SA" )

  ToolSvc += MDTRodDecoder

  from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MDT_RawDataProviderTool
  MuonMdtRawDataProviderTool = Muon__MDT_RawDataProviderTool(name        = "MDT_RawDataProviderTool_L2SA",
                                                             RdoLocation = "MDTCSM_L2SA",
                                                             Decoder     = MDTRodDecoder )
  ToolSvc += MuonMdtRawDataProviderTool

  from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MdtRdoToPrepDataTool
  MdtRdoToMdtPrepDataTool = Muon__MdtRdoToPrepDataTool(name                = "MdtRdoToPrepDataTool_L2SA",
                                                       RawDataProviderTool = MuonMdtRawDataProviderTool,
                                                       RDOContainer        = MuonMdtRawDataProviderTool.RdoLocation,
                                                       OutputCollection    = "MDT_DriftCircles_L2SA",
                                                       useBStoRdoTool      = True)
  ToolSvc += MdtRdoToMdtPrepDataTool

 
  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__MdtDataPreparator
  L2MdtDataPreparator = TrigL2MuonSA__MdtDataPreparator(#OutputLevel = DEBUG,
                                                        MdtPrepDataProvider  = MdtRdoToMdtPrepDataTool,
                                                        MDTPrepDataContainer = MdtRdoToMdtPrepDataTool.OutputCollection,
                                                        MDT_RawDataProvider  = MuonMdtRawDataProviderTool,
                                                        MDTCSMContainer      = MuonMdtRawDataProviderTool.RdoLocation )
  ToolSvc += L2MdtDataPreparator
  

  ### RPC RDO data ###
  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcROD_Decoder
  RPCRodDecoder = Muon__RpcROD_Decoder(name  = "RpcROD_Decoder_L2SA" )

  ToolSvc += RPCRodDecoder

  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RPC_RawDataProviderTool
  MuonRpcRawDataProviderTool = Muon__RPC_RawDataProviderTool(name        = "RPC_RawDataProviderTool_L2SA",
                                                             RdoLocation = "RPCPAD_L2SA",
                                                             RPCSec      = "RPC_SECTORLOGIC_L2SA",
                                                             Decoder     = RPCRodDecoder )
  ToolSvc += MuonRpcRawDataProviderTool

  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRdoToPrepDataTool
  RpcRdoToRpcPrepDataTool = Muon__RpcRdoToPrepDataTool(name                    = "RpcRdoToPrepDataTool_L2SA",
                                                       OutputCollection        = MuonRpcRawDataProviderTool.RdoLocation,
                                                       TriggerOutputCollection = "RPC_Measurements_L2SA",
                                                       InputCollection         = "RPC_triggerHits_L2SA")
  ToolSvc += RpcRdoToRpcPrepDataTool

  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__RpcDataPreparator
  L2RpcDataPreparator = TrigL2MuonSA__RpcDataPreparator(#OutputLevel = DEBUG,
                                                        RpcPrepDataProvider  = RpcRdoToRpcPrepDataTool,
                                                        RpcPrepDataContainer = RpcRdoToRpcPrepDataTool.TriggerOutputCollection,
                                                        RpcRawDataProvider   = MuonRpcRawDataProviderTool,
                                                        DecodeBS = DetFlags.readRDOBS.RPC_on() )
  ToolSvc += L2RpcDataPreparator


  ### TGC RDO data ###
  from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TGC_RodDecoderReadout
  TGCRodDecoder = Muon__TGC_RodDecoderReadout(name	    = "TGC_RodDecoderReadout_L2SA" )

  ToolSvc += TGCRodDecoder

  from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TGC_RawDataProviderTool
  MuonTgcRawDataProviderTool = Muon__TGC_RawDataProviderTool(name        = "TGC_RawDataProviderTool_L2SA",
                                                             RdoLocation = "TGCRDO_L2SA",
                                                             Decoder     = TGCRodDecoder )
  ToolSvc += MuonTgcRawDataProviderTool
    
  from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TgcRdoToPrepDataTool
  TgcRdoToTgcPrepDataTool = Muon__TgcRdoToPrepDataTool(name                 = "TgcRdoToPrepDataTool_L2SA",
                                                       RawDataProviderTool  = MuonTgcRawDataProviderTool,
                                                       RDOContainer         = MuonTgcRawDataProviderTool.RdoLocation,
                                                       OutputCollection     = "TGC_Measurements_L2SA",
                                                       OutputCoinCollection = "TrigT1CoinDataCollection_L2SA",
                                                       useBStoRdoTool       = True)

  ToolSvc += TgcRdoToTgcPrepDataTool

  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__TgcDataPreparator
  L2TgcDataPreparator = TrigL2MuonSA__TgcDataPreparator(#OutputLevel          = DEBUG,
                                                        TgcPrepDataProvider  = TgcRdoToTgcPrepDataTool,
                                                        TGCPrepDataContainer = TgcRdoToTgcPrepDataTool.OutputCollection )
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

  return muFastAlg
