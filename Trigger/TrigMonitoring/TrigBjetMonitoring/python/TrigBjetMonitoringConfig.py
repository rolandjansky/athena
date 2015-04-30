# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ToolSvc
	
def TrigBjetMonitoringConfig():
	
	from TrigBjetMonitoring.TrigBjetMonitoringConf import HLTBjetMonTool
	
	HLTBjetMon = HLTBjetMonTool(name = 'HLTBjetMon', histoPathBase = "/Trigger/HLT");


	from TrigHLTMonitoring.HLTMonTriggerList import hltmonList  # access to central tool
	HLTBjetMon.monitoring_mujet = hltmonList.monitoring_mujet
	HLTBjetMon.monitoring_bjet = hltmonList.monitoring_bjet


	from AthenaCommon.AppMgr import ToolSvc
	ToolSvc += HLTBjetMon;
	list = [ "HLTBjetMonTool/HLTBjetMon" ];
	
	return list




