#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#


### ==================== Data prepartion needed for the EF and L2 SA #######################333
def makeMuonPrepDataAlgs( doL2SA, doEFSA ):

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
                                       #OutputLevel 	= INFO )
  ToolSvc += CSCRodDecoder

  from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CSC_RawDataProviderTool
  MuonCscRawDataProviderTool = Muon__CSC_RawDataProviderTool(name        = "CSC_RawDataProviderTool",
                                                             Decoder     = CSCRodDecoder )
                                                             #OutputLevel = INFO )
  ToolSvc += MuonCscRawDataProviderTool

  from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscRdoToCscPrepDataTool
  CscRdoToCscPrepDataTool = Muon__CscRdoToCscPrepDataTool(name                = "CscRdoToCscPrepDataTool",
                                                          #OutputLevel         = INFO,
                                                          RawDataProviderTool = MuonCscRawDataProviderTool,
                                                          useBStoRdoTool      = True)
  ToolSvc += CscRdoToCscPrepDataTool

  from MuonRdoToPrepData.MuonRdoToPrepDataConf import CscRdoToCscPrepData
  CscRdoToCscPrepData = CscRdoToCscPrepData(name                    = "CscRdoToCscPrepData",
                                            CscRdoToCscPrepDataTool = CscRdoToCscPrepDataTool,
                                            PrintPrepData	      = False )
                                            #OutputLevel             = INFO)
  CscRdoToCscPrepData.DoSeededDecoding = True
  CscRdoToCscPrepData.RoIs = "MURoIs"

  eventAlgs_Muon.append( CscRdoToCscPrepData )
 
  from MuonByteStream.MuonByteStreamConf import Muon__CscRawDataProvider
  CscRawDataProvider = Muon__CscRawDataProvider(name         = "CscRawDataProvider",
                                                ProviderTool = MuonCscRawDataProviderTool )
                                                #OutputLevel  = INFO)

  from CscClusterization.CscClusterizationConf import CscThresholdClusterBuilderTool
  CscClusterBuilderTool = CscThresholdClusterBuilderTool(name        = "CscThesholdClusterBuilderTool" )
                                                         #OutputLevel = INFO)
  ToolSvc += CscClusterBuilderTool

  #CSC cluster building
  from CscClusterization.CscClusterizationConf import CscThresholdClusterBuilder
  CscClusterBuilder = CscThresholdClusterBuilder(name            = "CscThesholdClusterBuilder",
                                                 #OutputLevel     = INFO,
                                                 cluster_builder = CscClusterBuilderTool)    
  
  if doEFSA == True: 
    viewAlgs_MuEFSA.append( CscRdoToCscPrepData )  
    viewAlgs_MuEFSA.append( CscClusterBuilder ) 
  if doL2SA == True:
    viewAlgs_MuL2SA.append( CscRdoToCscPrepDataTool )  
 
  ### MDT RDO data ###
  from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import MdtROD_Decoder
  MDTRodDecoder = MdtROD_Decoder(name	   = "MdtROD_Decoder" )
                                 #OutputLevel = INFO )
  ToolSvc += MDTRodDecoder

  from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MDT_RawDataProviderTool
  MuonMdtRawDataProviderTool = Muon__MDT_RawDataProviderTool(name        = "MDT_RawDataProviderTool",
                                                             Decoder     = MDTRodDecoder )
                                                             #OutputLevel = INFO )
  ToolSvc += MuonMdtRawDataProviderTool

  from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MdtRdoToPrepDataTool
  MdtRdoToMdtPrepDataTool = Muon__MdtRdoToPrepDataTool(name                = "MdtRdoToPrepDataTool")
                                                       #OutputLevel         = INFO )
  ToolSvc += MdtRdoToMdtPrepDataTool

  from MuonRdoToPrepData.MuonRdoToPrepDataConf import MdtRdoToMdtPrepData
  MdtRdoToMdtPrepData = MdtRdoToMdtPrepData(name          = "MdtRdoToMdtPrepData",
                                            DecodingTool  = MdtRdoToMdtPrepDataTool,
                                            PrintPrepData = False )
                                            #OutputLevel   = INFO)  
  MdtRdoToMdtPrepData.DoSeededDecoding = True
  MdtRdoToMdtPrepData.RoIs = "MURoIs"
  
  eventAlgs_Muon.append( MdtRdoToMdtPrepData )
  
  from MuonByteStream.MuonByteStreamConf import Muon__MdtRawDataProvider
  MdtRawDataProvider = Muon__MdtRawDataProvider(name         = "MdtRawDataProvider",
                                                ProviderTool = MuonMdtRawDataProviderTool )
                                                #OutputLevel  = INFO)
  if doEFSA == True:
    viewAlgs_MuEFSA.append( MdtRawDataProvider )
    viewAlgs_MuEFSA.append( MdtRdoToMdtPrepData )
  if doL2SA == True:
    viewAlgs_MuL2SA.append( MdtRdoToMdtPrepDataTool )
    viewAlgs_MuL2SA.append( MuonMdtRawDataProviderTool )

  ### RPC RDO data ###
  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcROD_Decoder
  RPCRodDecoder = Muon__RpcROD_Decoder(name	     = "RpcROD_Decoder" )
                                       #OutputLevel = INFO )
  ToolSvc += RPCRodDecoder

  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RPC_RawDataProviderTool
  MuonRpcRawDataProviderTool = Muon__RPC_RawDataProviderTool(name    = "RPC_RawDataProviderTool",
                                                             Decoder = RPCRodDecoder )
  ToolSvc += MuonRpcRawDataProviderTool

  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRdoToPrepDataTool
  RpcRdoToRpcPrepDataTool = Muon__RpcRdoToPrepDataTool(name                = "RpcRdoToPrepDataTool")
                                                       #OutputLevel         = INFO,
                                                       #RawDataProviderTool = MuonRpcRawDataProviderTool,
                                                       #useBStoRdoTool      = True)
  ToolSvc += RpcRdoToRpcPrepDataTool

  from MuonRdoToPrepData.MuonRdoToPrepDataConf import RpcRdoToRpcPrepData
  RpcRdoToRpcPrepData = RpcRdoToRpcPrepData(name          = "RpcRdoToRpcPrepData",
                                            DecodingTool  = RpcRdoToRpcPrepDataTool,
                                            PrintPrepData = False )
                                            #OutputLevel   = INFO)    
  RpcRdoToRpcPrepData.DoSeededDecoding = True
  RpcRdoToRpcPrepData.RoIs = "MURoIs"

  eventAlgs_Muon.append( RpcRdoToRpcPrepData )

  from MuonByteStream.MuonByteStreamConf import Muon__RpcRawDataProvider
  RpcRawDataProvider = Muon__RpcRawDataProvider(name         = "RpcRawDataProvider",
                                                ProviderTool = MuonRpcRawDataProviderTool,
                                                DoSeededDecoding = True,
                                                RoIs = "MURoIs")
                                                #OutputLevel  = INFO)
  if doEFSA == True:
    viewAlgs_MuEFSA.append( RpcRawDataProvider )
    viewAlgs_MuEFSA.append( RpcRdoToRpcPrepData )
  if doL2SA == True:
    viewAlgs_MuL2SA.append( RpcRdoToRpcPrepDataTool )
    viewAlgs_MuL2SA.append( MuonRpcRawDataProviderTool )

  ### TGC RDO data ###
  from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TGC_RodDecoderReadout
  TGCRodDecoder = Muon__TGC_RodDecoderReadout(name	    = "TGC_RodDecoderReadout" )
                                              #OutputLevel = INFO )
  ToolSvc += TGCRodDecoder

  from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TGC_RawDataProviderTool
  MuonTgcRawDataProviderTool = Muon__TGC_RawDataProviderTool(name    = "TGC_RawDataProviderTool",
                                                             Decoder = TGCRodDecoder )
  ToolSvc += MuonTgcRawDataProviderTool
    
  from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TgcRdoToPrepDataTool
  TgcRdoToTgcPrepDataTool = Muon__TgcRdoToPrepDataTool(name                = "TgcRdoToPrepDataTool",
                                                       #OutputLevel         = INFO,
                                                       RawDataProviderTool = MuonTgcRawDataProviderTool,
                                                       useBStoRdoTool      = True)

  ToolSvc += TgcRdoToTgcPrepDataTool

  from MuonRdoToPrepData.MuonRdoToPrepDataConf import TgcRdoToTgcPrepData
  TgcRdoToTgcPrepData = TgcRdoToTgcPrepData(name          = "TgcRdoToTgcPrepData",
                                            DecodingTool  = TgcRdoToTgcPrepDataTool,
                                            PrintPrepData = False )
                                            #OutputLevel   = INFO)
  TgcRdoToTgcPrepData.DoSeededDecoding = True
  TgcRdoToTgcPrepData.RoIs = "MURoIs"

  eventAlgs_Muon.append( TgcRdoToTgcPrepData )

  from MuonByteStream.MuonByteStreamConf import Muon__TgcRawDataProvider
  TgcRawDataProvider = Muon__TgcRawDataProvider(name         = "TgcRawDataProvider",
                                                ProviderTool = MuonTgcRawDataProviderTool )
                                                #OutputLevel  = INFO)
  if doEFSA == True:
    viewAlgs_MuEFSA.append( TgcRdoToTgcPrepData )
  if doL2SA == True:
    viewAlgs_MuL2SA.append( TgcRdoToTgcPrepDataTool )
    viewAlgs_MuL2SA.append( MuonTgcRawDataProviderTool )


  if muonRecFlags.doCreateClusters():  
    #Run clustering
    from MuonClusterization.MuonClusterizationConf import MuonClusterizationTool
    MuonClusterTool = MuonClusterizationTool(name        = "MuonClusterizationTool" )
                                             #OutputLevel = INFO)
    ToolSvc += MuonClusterTool
    
    from MuonClusterization.MuonClusterizationConf import MuonClusterizationAlg
    MuonClusterAlg = MuonClusterizationAlg(name                 = "MuonClusterizationAlg",
                                           #OutputLevel          = INFO,
                                           ClusterTool          = MuonClusterTool,
                                           TgcPrepDataContainer = "TGC_MeasurementsAllBCs")
    
    if doEFSA == True:
      viewAlgs_MuEFSA.append( MuonClusterAlg )


  return ( eventAlgs_Muon, viewAlgs_MuL2SA, viewAlgs_MuEFSA )



