svcMgr = theApp.serviceMgr()
if not hasattr( svcMgr, "ByteStreamAddressProviderSvc" ):
    from ByteStreamCnvSvcBase. ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
    svcMgr += ByteStreamAddressProviderSvc()

from AthenaCommon.DetFlags import DetFlags

if DetFlags.readRDOBS.pixel_on():
    svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
        "PixelRDO_Container/PixelRDOs",
        ]

if DetFlags.readRDOBS.SCT_on():
    svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
        "SCT_RDO_Container/SCT_RDOs",
        ]

if DetFlags.readRDOBS.TRT_on():
    svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
        "TRT_RDO_Container/TRT_RDOs",
        ]

if DetFlags.readRDOBS.LAr_on():
    svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
        "LArRawChannelContainer/LArRawChannels",
        ]

if DetFlags.readRDOBS.Tile_on():
    svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
        "TileRawChannelContainer/TileRawChannelCnt",
        "TileRawChannelContainer/MuRcvRawChCnt",
        ]

if DetFlags.readRDOBS.RPC_on():
    svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
        "RpcPadContainer/RPCPAD",
        ]

if DetFlags.readRDOBS.MDT_on():
    svcMgr.ByteStreamAddressProviderSvc.TypeNames += [    
        "MdtCsmContainer/MDTCSM",
        ]

if DetFlags.readRDOBS.TGC_on():
    svcMgr.ByteStreamAddressProviderSvc.TypeNames += [        
        "TgcRdoContainer/TGCRDO",
        ]

if DetFlags.readRDOBS.CSC_on():
    svcMgr.ByteStreamAddressProviderSvc.TypeNames += [            
        "CscRawDataContainer/CSCRDO",
        ]

if DetFlags.readRDOBS.LVL1_on():
    svcMgr.ByteStreamAddressProviderSvc.TypeNames += [                
        "ROIB::RoIBResult/RoIBResult",
        "MuCTPI_RDO/MUCTPI_RDO",        
        "L2Result/L2Result",
        "CTP_RDO/CTP_RDO",
        ] 
