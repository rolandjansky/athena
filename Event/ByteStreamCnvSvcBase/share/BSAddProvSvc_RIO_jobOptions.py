svcMgr = theApp.serviceMgr()
if not hasattr( svcMgr, "ByteStreamAddressProviderSvc" ):
     from ByteStreamCnvSvcBase. ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
     svcMgr += ByteStreamAddressProviderSvc()

from AthenaCommon.DetFlags import DetFlags

if DetFlags.readRIOBS.LAr_on():
     svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ 
          "LArCellIDC/LArCellIDC",
          ]

if DetFlags.readRIOBS.Tile_on():
     svcMgr.ByteStreamAddressProviderSvc.TypeNames += [      
          "TileCellIDC/TileCellIDC",
          ]

if DetFlags.readRIOBS.pixel_on():
     svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
          "Trk::PixelClusterContainer/PixelOnlineClusters",
          ]

if DetFlags.readRIOBS.SCT_on():
     svcMgr.ByteStreamAddressProviderSvc.TypeNames += [          
          "Trk::SCT_ClusterContainer/SCT_OnlineClusters",
          ]

if DetFlags.readRIOBS.TRT_on():
     svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
          "Trk::TRT_DriftCircleContainer/TRT_DriftCircle",
          ]

if DetFlags.readRIOBS.MDT_on():
     svcMgr.ByteStreamAddressProviderSvc.TypeNames += [     
          "MdtDigitContainer/MDT_DIGITS",
          "Muon::MdtPrepDataContainer/MDT_DriftCircles",
          ]

if DetFlags.readRIOBS.RPC_on():
     svcMgr.ByteStreamAddressProviderSvc.TypeNames += [          
          "RpcDigitContainer/RPC_DIGITS",
          "Muon::RpcPrepDataContainer/RPC_Measurements",
          ]

if DetFlags.readRIOBS.TGC_on():
     svcMgr.ByteStreamAddressProviderSvc.TypeNames += [               
          "TgcDigitContainer/TGC_DIGITS",
          "Muon::TgcPrepDataContainer/TGC_Measurements",
          ]

if DetFlags.readRIOBS.CSC_on():
     svcMgr.ByteStreamAddressProviderSvc.TypeNames += [                    
          "CscDigitContainer/CSC_DIGITS",
          "Muon::CscPrepDataContainer/CSC_Clusters",
          ]

if DetFlags.readRIOBS.LVL1_on():
     svcMgr.ByteStreamAddressProviderSvc.TypeNames += [                         
          "ROIB::RecRoIBResult/RecRoIBResult",
          "MuCTPI_RIO/MUCTPI_RIO",
          "CTP_RIO/CTP_RIO"
          ] 

from RecExConfig.RecFlags import rec
if rec.doTrigger() :
     svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
          "HLT::HLTResult/HLTResult_L2",
          "HLT::HLTResult/HLTResult_EF"
          ]
         
