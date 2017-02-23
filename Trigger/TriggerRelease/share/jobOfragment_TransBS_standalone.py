#--------------------------------------------------------------
# Write to Transient BS
#--------------------------------------------------------------
include( "ByteStreamCnvSvc/RDP_ByteStream_jobOptions.py" )
        
StreamBS = AthenaOutputStream("StreamBS",
                             EvtConversionSvc = "ByteStreamCnvSvc")
StreamBS.ForceRead=True
if not TriggerFlags.fakeLVL1():
   #LVL1
   theApp.Dlls += [ "TrigT1ResultByteStream" ]
   StreamBS.ItemList   += [ "ROIB::RoIBResult#*" ]
      
if TriggerFlags.doID():
   # Make RDOs and Write BS  for all 3 InDet detectors 
      
   # TRT 
   StreamBS.ItemList +=["2542#*"]      
   # SCT
   StreamBS.ItemList +=["2541#*"]   
   # Pixel
   StreamBS.ItemList +=["2540#*"]
      
if TriggerFlags.doCalo():
   #LAr
   theApp.Dlls   += [ "LArCalibUtils" ]
   theApp.Dlls   += [ "LArByteStream" ]
   #        StreamBS.ItemList +=["LArRawChannels#*"]
   StreamBS.ItemList +=["2721#*"]
   #Tile
   theApp.Dlls   += [ "TileByteStream" ]
   #        StreamBS.ItemList +=["TileRawChannelCnt#*"]
   StreamBS.ItemList +=["2927#*"]

if TriggerFlags.doMuon():
   # Muon 
   theApp.Dlls += [ "MuonByteStream" ]
   StreamBS.ItemList +=["MdtCsmContainer#*"]
   StreamBS.ItemList +=["RpcPadContainer#*"]
   StreamBS.ItemList +=["TgcRdoContainer#*"]
   StreamBS.ItemList +=["CscRawDataContainer#*"]

#--------------------------------------------------------------
# BSCnv Svc: Provider Service for all RIO classes
#--------------------------------------------------------------
# temp fix:
theApp.CreateSvc  += ["StoreGateSvc/StoreGateSvc" ]
#include ( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )
#include ("ByteStreamCnvSvcBase/BSAddProvSvc_RIO_jobOptions.py")

if not hasattr( ServiceMgr, "ByteStreamAddressProviderSvc" ):
   from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
   ServiceMgr += ByteStreamAddressProviderSvc()

if TriggerFlags.doCalo():
   ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ 
#        "LArCellIDC/LArCellIDC",
        "TileCellIDC/TileCellIDC"
        ]
   from AthenaCommon.AppMgr import ToolSvc
   from LArByteStream.LArByteStreamConfig import LArRawDataContByteStreamToolConfig
   ToolSvc+=LArRawDataContByteStreamToolConfig(InitializeForWriting=True) 

if TriggerFlags.doID():
   ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ 
      "InDet::PixelClusterContainer/PixelOnlineClusters",
      "InDet::SCT_ClusterContainer/SCT_OnlineClusters",
      "InDet::TRT_DriftCircleContainer/TRT_DriftCircle"
      ]

#if TriggerFlags.doMuon():
# needed for LVL1 simulation
ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ 
      "MdtDigitContainer/MDT_DIGITS",
      "RpcDigitContainer/RPC_DIGITS",
      "TgcDigitContainer/TGC_DIGITS",
      "CscDigitContainer/CSC_DIGITS",
      ]
if TriggerFlags.doMuon():
   ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ 
      # RDO
      "RpcPadContainer/RPCPAD",
      "MdtCsmContainer/MDTCSM",
      "TgcRdoContainer/TGCRDO",
      "CscRawDataContainer/CSCRDO",
      "MuCTPI_RDO/MUCTPI_RDO"
      ]
      
ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "ROIB::RoIBResult/RoIBResult" ]
ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "ROIB::RecRoIBResult/RecRoIBResult" ]

# this must be in a fragment:
if not hasattr(ServiceMgr,"ProxyProviderSvc"):                           
   from SGComps.SGCompsConf import ProxyProviderSvc
   ServiceMgr += ProxyProviderSvc()
ServiceMgr.ProxyProviderSvc.ProviderNames += [ "ByteStreamAddressProviderSvc" ]
      
#--------------------------------------------------------------- 
# BS to RIO for all Det
#---------------------------------------------------------------
#if TriggerFlags.doID():
#   include( "TRT_DriftFunctionTool/TRT_DriftFunctionTool_options.py" )
#   theApp.Dlls += [ "TRT_ConditionsAlgs" ]

#if TriggerFlags.doCalo():
#   from AthenaCommon.AppMgr import ToolSvc
#   ToolSvc.LArRoI_Map.Print=FALSE
#   if TriggerFlags.doEF():
#      ToolSvc.LArRodDecoder.LArCell=TRUE
   # LArCell Correction Tools
#   ToolSvc.LArRodDecoder.LArCellEthreshold = -10.030;  # threshold in GeV 
#   theApp.Dlls += [ "LArRawUtils", "LArROD" ]
#   theApp.Dlls += [ "LArCellRec" ]
#   theApp.DLLs += ["TileRecUtils"]
#   theApp.Dlls += [ "TileRecAlgs" ]
#   include( "TileByteStream/TileCellIDC_FromBS_jobOptions.py" )      
