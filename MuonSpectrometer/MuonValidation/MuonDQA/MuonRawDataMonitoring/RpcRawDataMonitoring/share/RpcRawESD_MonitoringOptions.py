#printfunc ('\n************************************************************************************************\n')
#printfunc ("   ******** Welcome to the Offline BS MuonRawDataMonitoring/RpcRawDataMonitoring package. ******** "  )
#printfunc ("   ******** Documentation may be found at:******** "  )
#printfunc ("   **** https://twiki.cern.ch/twiki/bin/view/Atlas/RPCOfflineMonitoringPlots ***** "  )
#printfunc ('\n************************************************************************************************\n')

if not 'MuonDQAFlags' in dir():
    printfunc ("MuonDQAFlags.py: MuonDQAFlags not yet imported - I import them now")
    from MuonDQAMonFlags.MuonDQAFlags import MuonDQAFlags as MuonDQAFlags


#################### RPCStandaloneTracksMon #######################

rpcTrackMonMan = AthenaMonManager(name="RpcTrackMonManager",
                                FileKey             = DQMonFlags.monManFileKey(),
                                Environment         = DQMonFlags.monManEnvironment(),
                                OutputLevel         = muonOutputLevel)

from RpcRawDataMonitoring.RpcRawDataMonitoringConf import RPCStandaloneTracksMon
from StoreGate.StoreGateConf import StoreGateSvc
if not hasattr(topSequence,"RpcClusterBuilderPRD"):
    from RpcClusterization.RpcClusterizationConf import RpcClusterBuilderPRD
    topSequence += RpcClusterBuilderPRD(ClusterTimeSpread = 10000,
                                        OutputLevel = FATAL)

OutputMessageLevel = WARNING

RPCStandaloneTracksMon = RPCStandaloneTracksMon(name='RPCStandaloneTracksMon',
                                                       RpcFile               = False,
                                                       DoRpcEsd              = True,
                                                       RpcChamberHist        = False,
                                                       RPCStandaloneTracks   = True,
                                                       RpcReduceNbins        = 8,
                                                       RpcReduceNbinsStrip   = 8,
						       rpc_readout_window    = 0.2, ## micro sec
                                                       doRadiography         = False,
						       doCoolDB              = False,
                                                       ClusterContainer      = "rpcClusters")
if globalflags.DataSource() != 'data':
    RPCStandaloneTracksMon.isMC = True
#ToolSvc += RPCStandaloneTracksMon
rpcTrackMonMan.AthenaMonTools += [ RPCStandaloneTracksMon ]
topSequence += rpcTrackMonMan
printfunc (rpcTrackMonMan)

############################ rpcLv1RawEfficiencyMonManager ######################################
rpcLv1RawEfficiencyMonMan = AthenaMonManager(name="rpcLv1RawEfficiencyMonManager",
                               FileKey             = DQMonFlags.monManFileKey(),
                               Environment         = DQMonFlags.monManEnvironment(),
                               OutputLevel         = muonOutputLevel)

############# RpcLv1RawDataEfficiency ###################################################
from RpcRawDataMonitoring.RpcRawDataMonitoringConf import RpcLv1RawDataEfficiency
from StoreGate.StoreGateConf import StoreGateSvc
if not hasattr(topSequence,"RpcClusterBuilderPRD"):
    from RpcClusterization.RpcClusterizationConf import RpcClusterBuilderPRD
    topSequence += RpcClusterBuilderPRD(ClusterTimeSpread = 10000,
                                        OutputLevel = FATAL)
rpcLv1RawDataEfficiency = RpcLv1RawDataEfficiency(name='rpcLv1RawDataEfficiency')
if globalflags.DataSource() != 'data':
    rpcLv1RawDataEfficiency.isMC = True
#ToolSvc += rpcLv1RawDataEfficiency
rpcLv1RawEfficiencyMonMan.AthenaMonTools += [ rpcLv1RawDataEfficiency ]
topSequence += rpcLv1RawEfficiencyMonMan
printfunc (rpcLv1RawEfficiencyMonMan)

###################################################################
#
##--------------------------------------------------------------
## Load IOVDbSvc
##--------------------------------------------------------------
#IOVDbSvc = Service("IOVDbSvc")
#from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
#from IOVDbSvc.CondDB import conddb
##IOVDbSvc.GlobalTag="OFLCOND-FDR-01-02-00"
#IOVDbSvc.OutputLevel = 3
#
### Monitoring DB
##
##from MuonCondSvc.MuonCondSvcConf import RPC_STATUSConditionsSvc
##ServiceMgr +=RPC_STATUSConditionsSvc()
##
##
##from MuonCondTool.MuonCondToolConf import RpcDetectorStatusDbTool
##RpcDetectorStatusDbTool = RpcDetectorStatusDbTool("RpcDetectorStatusDbTool")
##RpcDetectorStatusDbTool.OutputLevel = DEBUG 
###MessageSvc.OutputLevel = DEBUG
##
###RpcDetectorStatusDbTool.RpcDqFolder = "/RPC/DQMF/ELEMENT_STATUS"
##RpcDetectorStatusDbTool.RpcDqFolder = "/OFFLINE/FINAL"
##
##ToolSvc += RpcDetectorStatusDbTool
##
###dbConn="oracle://intr;schema=ATLAS_COOL_RPCDQ;dbname=RPC_DQA"
####dbConn="COOLOFL_RPC/OFLP200"
###;user=ATLAS_COOL_RPCDQ"
##
##
##
##
##dbConn="sqlite://;schema=RPCConditionDB_16_07_2011_with_time.db;dbname=RPC_DQA"
##
###folder="/RPC/DQMF/ELEMENT_STATUS"
##folder="/OFFLINE/FINAL"
##
##include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")
##
##ServiceMgr.IOVDbSvc.dbConnection=dbConn
##ServiceMgr.IOVDbSvc.Folders+=[folder+" <tag>RecoCondDB</tag> <dbConnection>"+dbConn+"</dbConnection>"]
###ServiceMgr.IOVDbSvc.Folders+=[folder+" <tag>RPCConditionDB_2010_Aug_20</tag> <dbConnection>"+dbConn+"</dbConnection>"]
#
#
## Dead Chambers
#
#from MuonCondSvc.MuonCondSvcConf import RPC_STATUSConditionsSvc
#ServiceMgr +=RPC_STATUSConditionsSvc()
#
#from MuonCondSvc.MuonCondSvcConf import RPCCondSummarySvc
#ServiceMgr +=RPCCondSummarySvc()
#
#include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")
#
#
##########
#from MuonCondSvc.MuonCondSvcConf import RPC_DCSConditionsSvc
#ServiceMgr +=RPC_DCSConditionsSvc()
#from MuonCondTool.MuonCondToolConf import RPC_DCSConditionsTool
#RPC_DCSConditionsTool= RPC_DCSConditionsTool("RPC_DCSConditionsTool")
#RPC_DCSConditionsTool.OutputLevel = INFO 
#RPC_DCSConditionsTool.DeadPanel       = "/RPC/DCS/DeadRopanels"
#RPC_DCSConditionsTool.OffPanelFolder  = "/RPC/DCS/OffRopanels"
#
#ToolSvc += RPC_DCSConditionsTool
#
#IOVDbSvc.DBInstance="COOLOFL_DCS"
##dbConn1="COOLOFL_DCS/COMP200"
##dbConn1="DCS_OFL"
##dbConn1="DCS_OFL"
#folder1="/RPC/DCS/DeadRopanels"
#folder2="/RPC/DCS/OffRopanels"
# 
#dbConn1="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200"
#
#ServiceMgr.IOVDbSvc.dbConnection=dbConn1
#ServiceMgr.IOVDbSvc.Folders+=[folder2+"<dbConnection>"+dbConn1+"</dbConnection>"]
#ServiceMgr.IOVDbSvc.Folders+=[folder1+"<dbConnection>"+dbConn1+"</dbConnection>"]
##ServiceMgr.IOVDbSvc.Folders+=[folder2+"<dbConnection>"+dbConn1+"</dbConnection>"]
#
#
#
###RPCCondSummary=ServiceMgr.RPCCondSummarySvc
###RPCCondSummary.ConditionsServices=["RPC_STATUSConditionsSvc","RPC_DCSConditionsTool"]
#
#from MuonCondSvc.MuonCondSvcConf import RPCCondSummarySvc
#ServiceMgr +=RPCCondSummarySvc()
