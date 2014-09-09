# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def TrigIDpvtxMonitoringTool():

	# dataTypes: userDefined = 0, monteCarlo, collisions, cosmics 
	if not 'DQMonFlags' in dir():
		from AthenaMonitoring.DQMonFlags import DQMonFlags
	dataType = DQMonFlags.monManDataType()


	# MonTool
	from TrigIDtrkMonitoring.TrigIDtrkMonitoringConf import HLTIDtrkMonTool
	from AthenaCommon.AppMgr import ToolSvc
	

	# DumpTool
	from TrigIDtrkMonitoring.TrigIDtrkMonitoringConf import HLTIDpvtxTool
	HLTIDpvtx = HLTIDpvtxTool(name               = 'HLTIDpvtx',
				  histoPathBase      = "/Trigger/HLT");

	#HLTIDpvtx.ChainName                        = "InDetMon_???"
	#HLTIDpvtx.ChainName                        = "L2_b10_IDTrkNoCut"
	#HLTIDpvtx.ChainName                        = "L2_b10_j30_a4tc_EFFS_IDTrkNoCut"
	#HLTIDpvtx.ChainName                        = "L2_b50_NoCut_j50_c4cchad"
	#HLTIDpvtx.ChainName                        = "EF_b10_IDTrkNoCut"
	#HLTIDpvtx.ChainName                        = "EF_b10_j30_a4tc_EFFS_IDTrkNoCut"
	HLTIDpvtx.ChainName                        = "EF_b55_NoCut_j55_a4tchad"
	#HLTIDpvtx.OnlinePrimaryVertexContainerName = "L2BjetFex"
	HLTIDpvtx.OnlinePrimaryVertexContainerName = "PrimVx"
	HLTIDpvtx.OnlineEfficiancyRangeCutX         = 0.01
	HLTIDpvtx.OnlineEfficiancyRangeCutY         = 0.01
	HLTIDpvtx.OnlineEfficiancyRangeCutZ         = 0.01
	
	ToolSvc += HLTIDpvtx;
	#list += [ "HLTIDpvtxTool/HLTIDpvtx" ];
	list = [ "HLTIDpvtxTool/HLTIDpvtx" ];

	return list
