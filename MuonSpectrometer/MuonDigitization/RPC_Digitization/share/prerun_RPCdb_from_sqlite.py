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

    dbConn_Rpc="sqlite://;schema=RPCConditionDB_271595.db;dbname=RPC_DQA"
    folder_Rpc="/OFFLINE/FINAL"
    
    ServiceMgr.IOVDbSvc.dbConnection=dbConn_Rpc
    ServiceMgr.IOVDbSvc.Folders+=[folder_Rpc+"<tag>RecoCondDB</tag> <dbConnection>"+dbConn_Rpc+"</dbConnection>"]
