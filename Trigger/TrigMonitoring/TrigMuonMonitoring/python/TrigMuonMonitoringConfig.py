# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def TrigMuonMonitoringTool():
	from TrigMuonMonitoring.MuZTPChains import GetMuZTPChains
	from TrigMuonMonitoring.TrigMuonMonitoringConf import HLTMuonMonTool
	from TrigHLTMonitoring.HLTMonTriggerList import hltmonList  # access to central tool
	HLTMuonMon = HLTMuonMonTool(name                 = 'HLTMuonMon',
				    histoPathBase        = "/Trigger/HLT",
				    ZTPPtCone30RelCut    = 0.1)
	HLTMuonMon.monitoring_muonNonIso = hltmonList.monitoring_muonNonIso
	HLTMuonMon.monitoring_muonIso = hltmonList.monitoring_muonIso
	HLTMuonMon.monitoring_MSonly = hltmonList.monitoring_MSonly
	HLTMuonMon.monitoring_muonEFFS = hltmonList.monitoring_muonEFFS
	HLTMuonMon.monitoring_muon_Support = hltmonList.monitoring_muon_Support

	from RecExConfig.RecFlags import rec
	from AthenaCommon.BeamFlags import jobproperties
	if rec.doHeavyIon == True or rec.doHIP == True:
	    HLTMuonMon.HI_pp_mode = False
	else:
	    HLTMuonMon.HI_pp_mode = True

	from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool	
	HLTMuonMon.BCTool = BunchCrossingTool()
	from AthenaCommon.AppMgr import ToolSvc
	ToolSvc += HLTMuonMon;
	list = [ "HLTMuonMonTool/HLTMuonMon" ];
	return list


