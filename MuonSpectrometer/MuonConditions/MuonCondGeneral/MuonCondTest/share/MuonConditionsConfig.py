# read alignment constants from DB to update MuonGeoModel
#
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.GlobalFlags import globalflags

from MuonRecExample.MuonRecUtils import logMuon
from IOVDbSvc.CondDB import conddb


###############################################################
# There are 2 types of technologies RPC and MDT
# 
#
#
# 
#
###############################################################


import re

logMuon.info("Reading MDT DCS constants from DB")

## these are in COMP200 force to be read by simulation
conddb.DBInstance="COOLOFL_DCS"
conddb.addFolder('DCS_OFL','/MDT/DCS/DROPPEDCH',forceData=True)
conddb.addFolder('DCS_OFL','/MDT/DCS/PSHVMLSTATE',forceData=True)
conddb.addFolder('DCS_OFL','/MDT/DCS/PSLVCHSTATE',forceData=True)
conddb.addFolder('DCS_OFL','/MDT/DCS/JTAGCHSTATE',forceData=True)
conddb.addFolder('DCS_OFL','/MDT/DCS/PSV0SETPOINTS',forceData=True)
conddb.addFolder('DCS_OFL','/MDT/DCS/PSV1SETPOINTS',forceData=True)


## these are in OFLP200 force to be read by simulation


logMuon.info("Reading MDT Dead Flags constants from DB")
conddb.DBInstance="COOLOFL_MDT"
conddb.addFolder('MDT','/MDT/TUBE_STATUS/DEAD_TUBE',forceMC=True)


logMuon.info("Reading RPC Flags constants from DB")
conddb.DBInstance="COOLOFL_RPC"
conddb.addFolder('RPC','/RPC/DQMF/ELEMENT_STATUS',forceMC=True)


## MDT Conditions Data

from MuonCondSvc.MuonCondSvcConf import MDT_DCSConditionsSvc
ServiceMgr +=MDT_DCSConditionsSvc()

from MuonCondTool.MuonCondToolConf import MDT_DCSConditionsTool
MDT_DCSConditionsTool = MDT_DCSConditionsTool("MDT_DCSConditionsTool")
MDT_DCSConditionsTool.OutputLevel = VERBOSE 

MDT_DCSConditionsTool.DropChamberFolder = "/MDT/DCS/DROPPEDCH"
MDT_DCSConditionsTool.HVFolder          = "/MDT/DCS/PSHVMLSTATE"
MDT_DCSConditionsTool.LVFolder          = "/MDT/DCS/PSLVCHSTATE"
MDT_DCSConditionsTool.JTAGFolder        = "/MDT/DCS/JTAGCHSTATE"
MDT_DCSConditionsTool.SetPointsV0Folder = "/MDT/DCS/PSV0SETPOINTS"
MDT_DCSConditionsTool.SetPointsV1Folder = "/MDT/DCS/PSV1SETPOINTS"

ToolSvc += MDT_DCSConditionsTool


from MuonCondSvc.MuonCondSvcConf import MDTCondSummarySvc
ServiceMgr +=MDTCondSummarySvc()


MDTCondSummary=ServiceMgr.MDTCondSummarySvc
MDTCondSummary.ConditionsServices=["MDT_DCSConditionsSvc","MDT_DeadTubeConditionsSvc"]


## RPC Conditions Data

from MuonCondSvc.MuonCondSvcConf import RPC_STATUSConditionsSvc
ServiceMgr +=RPC_STATUSConditionsSvc()

from MuonCondTool.MuonCondToolConf import RpcDetectorStatusDbTool
RpcDetectorStatusDbTool = RpcDetectorStatusDbTool("RpcDetectorStatusDbTool")
RpcDetectorStatusDbTool.OutputLevel = DEBUG 
RpcDetectorStatusDbTool.RpcDqFolder = ""
ToolSvc += RpcDetectorStatusDbTool


from MuonCondSvc.MuonCondSvcConf import RPCCondSummarySvc
ServiceMgr +=RPCCondSummarySvc()


RPCCondSummary=ServiceMgr.RPCCondSummarySvc
RPCCondSummary.ConditionsServices=["RPC_STATUSConditionsSvc"]



conddb.dumpFolderTags('MuonDbAccess.txt',True)
