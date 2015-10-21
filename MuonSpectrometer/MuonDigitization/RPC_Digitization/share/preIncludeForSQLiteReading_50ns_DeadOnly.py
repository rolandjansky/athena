useLocalSql = True
if useLocalSql:
    from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
    from IOVDbSvc.CondDB import conddb
    include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")

    from MuonCondSvc.MuonCondSvcConf import RPC_STATUSConditionsSvc
    ServiceMgr +=RPC_STATUSConditionsSvc()

    from MuonCondTool.MuonCondToolConf import RpcDetectorStatusDbTool
    RpcDetectorStatusDbTool = RpcDetectorStatusDbTool("RpcDetectorStatusDbTool")
    RpcDetectorStatusDbTool.OutputLevel = DEBUG 
    RpcDetectorStatusDbTool.RpcDqFolder = "/OFFLINE/FINAL"
    ToolSvc += RpcDetectorStatusDbTool
    ToolSvc.RpcDetectorStatusDbTool.OutputLevel = DEBUG

#    dbConn_Rpc="sqlite://;schema=RPCConditionDB_271595_atLeast1TrackOnPanel.db;dbname=RPC_DQA"
#    dbConn_Rpc="sqlite://;schema=temp17RPCConditionDB.db;dbname=RPC_DQA"
#    dbConn_Rpc="sqlite://;schema=MergedHistograms.data15_13TeV.physics_Main.merge.DESDM_MCP.f166.FullRpcMonitoring_v02.db;dbname=RPC_DQA"
#    dbConn_Rpc="sqlite://;schema=MergedHistograms.data15_13TeV.physics_Main.merge.DESDM_MCP.f628.FullRpcMonitoring_v02.db;dbname=RPC_DQA"
#    dbConn_Rpc="sqlite://;schema=temp19RPCConditionDB.db;dbname=RPC_DQA"
    dbConn_Rpc="sqlite://;schema=RPCConditionDB_temp50ns.db;dbname=RPC_DQA"
    folder_Rpc="/OFFLINE/FINAL"
    
    ServiceMgr.IOVDbSvc.dbConnection=dbConn_Rpc
    ServiceMgr.IOVDbSvc.Folders+=[folder_Rpc+"<tag>RecoCondDB</tag> <dbConnection>"+dbConn_Rpc+"</dbConnection>"]


#----------- private options 
#--- cabling & trigger roads
from AthenaCommon.AppMgr import ServiceMgr; 
import MuonRPC_Cabling.MuonRPC_CablingConfig ; 
ServiceMgr.MuonRPC_CablingSvc.RPCTriggerRoadsfromCool=False


from AthenaCommon import CfgGetter
from AthenaCommon.CfgGetter import getPublicTool, getPrivateTool, getService, getAlgorithm

RPC_DigitizationTool=getPublicTool("RpcDigitizationTool")
#RPC_DigitizationTool.ValidationSetup = 1   
RPC_DigitizationTool.IgnoreRunDependentConfig       = True
RPC_DigitizationTool.Force_BOG_BOF_DoubletR2_OFF    = False
RPC_DigitizationTool.Efficiency_fromCOOL            = True  # dead strips from COOL          
RPC_DigitizationTool.ClusterSize_fromCOOL           = True  
RPC_DigitizationTool.RPCInfoFromDb                  = True
RPC_DigitizationTool.CutProjectedTracks             = 999999999; #always apply python eff. and cs values; dead strips from COOL
#RPC_DigitizationTool.CutProjectedTracks             = 1;         #always apply COOL   eff. and cs values; dead strips from COOL
#RPC_DigitizationTool.CutProjectedTracks             = 10;         # apply COOL   eff. and cs values, if at least 10 tracks were used for the meas.; dead strips from COOL
#RPC_DigitizationTool.DumpFromDbFirst=True #### if True OutputLevel must be DEBUG
#RPC_DigitizationTool.OutputLevel=DEBUG
RPC_DigitizationTool.OutputLevel=INFO

### cond db tool 
RpcDetectorStatusDbTool=getPublicTool("RpcDetectorStatusDbTool")
#RpcDetectorStatusDbTool.OutputLevel=DEBUG

