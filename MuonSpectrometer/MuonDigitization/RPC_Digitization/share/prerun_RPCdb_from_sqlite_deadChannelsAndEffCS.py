useLocalSql = True
if useLocalSql:
    from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
    from IOVDbSvc.CondDB import conddb
    include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")

    from MuonCondSvc.MuonCondSvcConf import RPC_STATUSConditionsSvc
    ServiceMgr +=RPC_STATUSConditionsSvc()

    from MuonCondTool.MuonCondToolConf import RpcDetectorStatusDbTool
    RpcDetectorStatusDbTool = RpcDetectorStatusDbTool("RpcDetectorStatusDbTool")
    RpcDetectorStatusDbTool.OutputLevel = INFO 
    RpcDetectorStatusDbTool.RpcDqFolder = "/OFFLINE/FINAL"
    ToolSvc += RpcDetectorStatusDbTool

    dbConn_Rpc="sqlite://;schema=RPCConditionDB_271595_atLeast1TrackOnPanel.db;dbname=RPC_DQA"
    folder_Rpc="/OFFLINE/FINAL"
    
    ServiceMgr.IOVDbSvc.dbConnection=dbConn_Rpc
    ServiceMgr.IOVDbSvc.Folders+=[folder_Rpc+"<tag>RecoCondDB</tag> <dbConnection>"+dbConn_Rpc+"</dbConnection>"]



from AthenaCommon import CfgGetter
from AthenaCommon.CfgGetter import getPublicTool, getPrivateTool, getService, getAlgorithm
RPC_DigitizationTool=getPublicTool("RpcDigitizationTool")
#RPC_DigitizationTool.ValidationSetup = 1   
RPC_DigitizationTool.IgnoreRunDependentConfig = True
RPC_DigitizationTool.Force_BOG_BOF_DoubletR2_OFF    = False
RPC_DigitizationTool.Efficiency_fromCOOL      = True  # dead strips from COOL          
RPC_DigitizationTool.ClusterSize_fromCOOL     = True  
RPC_DigitizationTool.RPCInfoFromDb            = True
#RPC_DigitizationTool.CutProjectedTracks = 999999999; #always apply python eff. and cs values; dead strips from COOL
RPC_DigitizationTool.CutProjectedTracks = 1;         #always apply COOL   eff. and cs values; dead strips from COOL
#RPC_DigitizationTool.OutputLevel=DEBUG
RPC_DigitizationTool.OutputLevel=INFO
RPC_DigitizationTool.DumpFromDbFirst=True


svcMgr.MessageSvc.debugLimit    = 999999999

RpcDetectorStatusDbTool=getPublicTool("RpcDetectorStatusDbTool")
RpcDetectorStatusDbTool.OutputLevel=INFO


from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.defaultLimit = 99999999

