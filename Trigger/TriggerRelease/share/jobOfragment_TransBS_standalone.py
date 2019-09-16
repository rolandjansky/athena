#--------------------------------------------------------------
# Write to Transient BS
#--------------------------------------------------------------
include( "ByteStreamCnvSvc/RDP_ByteStream_jobOptions.py" )
#
# add here in ExtraInputs all conditions data which are needed for BS encoding
#        
StreamBS = AthenaOutputStream("StreamBS",
                              EvtConversionSvc = "ByteStreamCnvSvc",
                              ExtraInputs = [('xAOD::EventInfo','StoreGateSvc+EventInfo'),
                                             ('TileBadChannels','ConditionStore+TileBadChannels'),
                                             ('SCT_CablingData','ConditionStore+SCT_CablingData'), 
                                             ('PixelHitDiscCnfgData','ConditionStore+PixelHitDiscCnfgData'),
                                             ('MuonMDT_CablingMap','ConditionStore+MuonMDT_CablingMap') 
                                             ])
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

# this must be in a fragment:
if not hasattr(ServiceMgr,"ProxyProviderSvc"):                           
   from SGComps.SGCompsConf import ProxyProviderSvc
   ServiceMgr += ProxyProviderSvc()
ServiceMgr.ProxyProviderSvc.ProviderNames += [ "ByteStreamAddressProviderSvc" ]
