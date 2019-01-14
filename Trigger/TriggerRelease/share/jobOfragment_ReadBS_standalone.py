from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.GlobalFlags import globalflags
if TriggerFlags.doCalo():
    from TileRecUtils.TileDQstatusAlgDefault import TileDQstatusAlgDefault
    dqstatus = TileDQstatusAlgDefault()


# This is only needed in case we do the (obsolete) per-event muon data preparation (bug #84525)
if TriggerFlags.doMuon() and TriggerFlags.doEF() and 'forceMuonDataPrep' in dir():
    include ("MuonCnvExample/MuonReadBS_jobOptions.py")

if globalflags.DataSource=='data':
    svcMgr.ByteStreamCnvSvc.GetDetectorMask=True
    from IOVDbSvc.CondDB import conddb
    conddb.addFolder('TDAQ', '/TDAQ/RunCtrl/SOR_Params')

svcMgr.ByteStreamCnvSvc.InitCnvs += [ "EventInfo",
                                      "ROIB::RoIBResult",
                                      "HLT::HLTResult" ]

# -------------------------------------------------------------
# RIO Convertors
# -------------------------------------------------------------
ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
if TriggerFlags.doEF() or TriggerFlags.doHLT():
    ByteStreamAddressProviderSvc.TypeNames += [ 
        "TileCellIDC/TileCellIDC",
        "MdtDigitContainer/MDT_DIGITS",
        "RpcDigitContainer/RPC_DIGITS",
        "TgcDigitContainer/TGC_DIGITS",
        "CscDigitContainer/CSC_DIGITS",
        "ROIB::RecRoIBResult/RecRoIBResult",
        "MuCTPI_RIO/MUCTPI_RIO",
        "CTP_RIO/CTP_RIO"
    ] 

# -------------------------------------------------------------
# RDO Convertors
# -------------------------------------------------------------
#include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )
ByteStreamAddressProviderSvc.TypeNames += [ 
    "LArRawChannelContainer/LArRawChannels", 
    "TileRawChannelContainer/TileRawChannelCnt", 
    "ROIB::RoIBResult/RoIBResult",
    "MuCTPI_RDO/MUCTPI_RDO",        
    "HLT::HLTResult/HLTResult_L2",
    "HLT::HLTResult/HLTResult_EF",
    "CTP_RDO/CTP_RDO",
    "L1TopoRDOCollection/L1TopoRDOCollection"
    ] 

if TriggerFlags.doFTK():
    ByteStreamAddressProviderSvc.TypeNames += [ "FTK_RawTrackContainer/FTK_RDO_Tracks"]
