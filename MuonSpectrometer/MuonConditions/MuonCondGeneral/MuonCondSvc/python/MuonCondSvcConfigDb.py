# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool,addService

addService("MuonCondSvc.MuonTopCondSvcConfig.MDTCondSummarySvc", "MDTCondSummarySvc")
addService("MuonCondSvc.MuonTopCondSvcConfig.RPCCondSummarySvc", "RPCCondSummarySvc")
addService("MuonCondSvc.MuonTopCondSvcConfig.TGCCondSummarySvc", "TGCCondSummarySvc")
addService("MuonCondSvc.MuonTopCondSvcConfig.CSCCondSummarySvc", "CSCCondSummarySvc")
addTool("MuonCondSvc.MuonTopCondSvcConfig.MDT_DCSConditionsTool","MDT_DCSConditionsTool")
addTool("MuonCondSvc.MuonTopCondSvcConfig.RpcDetectorStatusDbTool","RpcDetectorStatusDbTool")
addTool("MuonCondSvc.MuonTopCondSvcConfig.CSC_DCSConditionsTool","CSC_DCSConditionsTool")
addTool("MuonCondSvc.MuonTopCondSvcConfig.RPC_DCSConditionsTool","RPC_DCSConditionsTool") 
addTool("MuonCondSvc.MuonTopCondSvcConfig.MDT_DCSConditionsRun2Tool","MDT_DCSConditionsRun2Tool")
#addTool("MuonCondSvc.MuonTopCondSvcConfig.TGC_STATUSConditionsTool","TGC_STATUSConditionsTool")
