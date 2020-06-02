# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import TrigMuonMonitoring.TrigMuonMonitCategory as muon


def TrigMuonMonitoringTool():
	from TrigMuonMonitoring.MuZTPChains import GetMuZTPChains
	from TrigMuonMonitoring.TrigMuonMonitoringConf import HLTMuonMonTool
	from TrigHLTMonitoring.HLTMonTriggerList import hltmonList  # access to central tool
	HLTMuonMon = HLTMuonMonTool(name                 = 'HLTMuonMon',
				    histoPathBase        = "/Trigger/HLT",
				    TriggerStreamOfFile	 = getTriggerStreamOfFile(),
				    ZTPPtCone30RelCut    = 0.5)
	HLTMuonMon.monitoring_muonNonIso = hltmonList.monitoring_muonNonIso
	HLTMuonMon.monitoring_muonIso = hltmonList.monitoring_muonIso
	HLTMuonMon.monitoring_MSonly = hltmonList.monitoring_MSonly
	HLTMuonMon.monitoring_muonEFFS = hltmonList.monitoring_muonEFFS
	HLTMuonMon.monitoring_muonLowpt = hltmonList.monitoring_muonLowpt
	HLTMuonMon.monitoring_muon_Support = hltmonList.monitoring_muon_Support
	setL2HypoTENames( HLTMuonMon, hltmonList)
	from RecExConfig.RecFlags import rec
	from AthenaCommon.BeamFlags import jobproperties
	if rec.doHeavyIon == True or rec.doHIP == True:
	    HLTMuonMon.HI_pp_mode = False
	else:
	    HLTMuonMon.HI_pp_mode = True
	from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool	
	HLTMuonMon.BCTool = BunchCrossingTool()
	from AthenaCommon.AppMgr import ToolSvc
	#ToolSvc += HLTMuonMon;
	from AthenaCommon.GlobalFlags import globalflags
	inputDataType = str(globalflags.DataSource.StoredValue)
	if inputDataType == 'data':
		HLTMuonMon.truthmon_isData = 1
	else:
		HLTMuonMon.truthmon_isData = 0
	list = [ HLTMuonMon ];
	return list


def getTriggerStreamOfFile():
	from PyUtils.MetaReaderPeeker import metadata
	stag = metadata['triggerStreamOfFile']
	return stag


def setL2HypoTENames( HLTMuonMon, hltmonList):

        #default
	HLTMuonMon.monitoring_muonNonIso_L2SAHypo = muon.monitoring_muonNonIso_L2SAHypo
	HLTMuonMon.monitoring_muonIso_L2SAHypo = muon.monitoring_muonIso_L2SAHypo
	HLTMuonMon.monitoring_MSonly_L2SAHypo = muon.monitoring_MSonly_L2SAHypo
	HLTMuonMon.monitoring_muonEFFS_L2SAHypo = muon.monitoring_muonEFFS_L2SAHypo
	HLTMuonMon.monitoring_muonLowpt_L2SAHypo = muon.monitoring_muonLowpt_L2SAHypo 
	HLTMuonMon.monitoring_muon_Support_L2SAHypo = muon.monitoring_muon_Support_L2SAHypo

	HLTMuonMon.monitoring_muonNonIso_L2CBHypo = muon.monitoring_muonNonIso_L2CBHypo
	HLTMuonMon.monitoring_muonIso_L2CBHypo = muon.monitoring_muonIso_L2CBHypo
	HLTMuonMon.monitoring_MSonly_L2CBHypo = muon.monitoring_MSonly_L2CBHypo
	HLTMuonMon.monitoring_muonEFFS_L2CBHypo = muon.monitoring_muonEFFS_L2CBHypo
	HLTMuonMon.monitoring_muonLowpt_L2CBHypo = muon.monitoring_muonLowpt_L2CBHypo 
	HLTMuonMon.monitoring_muon_Support_L2CBHypo = muon.monitoring_muon_Support_L2CBHypo

	if hltmonList._get_monitoring_mode_success == False:
		# what should be done in this case?
		#print "HLTMonTriggerList: Error getting monitoring mode, default monitoring lists will be used."
		pass
	
	
	elif hltmonList.pp_mode == True:
		HLTMuonMon.monitoring_muonNonIso_L2SAHypo = muon.monitoring_muonNonIso_pp_L2SAHypo
		HLTMuonMon.monitoring_muonIso_L2SAHypo = muon.monitoring_muonIso_pp_L2SAHypo
		HLTMuonMon.monitoring_MSonly_L2SAHypo = muon.monitoring_MSonly_pp_L2SAHypo
		HLTMuonMon.monitoring_muonEFFS_L2SAHypo = muon.monitoring_muonEFFS_pp_L2SAHypo
		HLTMuonMon.monitoring_muonLowpt_L2SAHypo = muon.monitoring_muonLowpt_pp_L2SAHypo 
		HLTMuonMon.monitoring_muon_Support_L2SAHypo = muon.monitoring_muon_Support_pp_L2SAHypo

		HLTMuonMon.monitoring_muonNonIso_L2CBHypo = muon.monitoring_muonNonIso_pp_L2CBHypo
		HLTMuonMon.monitoring_muonIso_L2CBHypo = muon.monitoring_muonIso_pp_L2CBHypo
		HLTMuonMon.monitoring_MSonly_L2CBHypo = muon.monitoring_MSonly_pp_L2CBHypo
		HLTMuonMon.monitoring_muonEFFS_L2CBHypo = muon.monitoring_muonEFFS_pp_L2CBHypo
		HLTMuonMon.monitoring_muonLowpt_L2CBHypo = muon.monitoring_muonLowpt_pp_L2CBHypo 
		HLTMuonMon.monitoring_muon_Support_L2CBHypo = muon.monitoring_muon_Support_pp_L2CBHypo

		
	elif hltmonList.HI_mode == True:
		HLTMuonMon.monitoring_muonNonIso_L2SAHypo = muon.monitoring_muonNonIso_HI_L2SAHypo
		HLTMuonMon.monitoring_muonIso_L2SAHypo = muon.monitoring_muonIso_HI_L2SAHypo
		HLTMuonMon.monitoring_MSonly_L2SAHypo = muon.monitoring_MSonly_HI_L2SAHypo
		HLTMuonMon.monitoring_muonEFFS_L2SAHypo = muon.monitoring_muonEFFS_HI_L2SAHypo
		HLTMuonMon.monitoring_muonLowpt_L2SAHypo = muon.monitoring_muonLowpt_HI_L2SAHypo 
		HLTMuonMon.monitoring_muon_Support_L2SAHypo = muon.monitoring_muon_Support_HI_L2SAHypo

		HLTMuonMon.monitoring_muonNonIso_L2CBHypo = muon.monitoring_muonNonIso_HI_L2CBHypo
		HLTMuonMon.monitoring_muonIso_L2CBHypo = muon.monitoring_muonIso_HI_L2CBHypo
		HLTMuonMon.monitoring_MSonly_L2CBHypo = muon.monitoring_MSonly_HI_L2CBHypo
		HLTMuonMon.monitoring_muonEFFS_L2CBHypo = muon.monitoring_muonEFFS_HI_L2CBHypo
		HLTMuonMon.monitoring_muonLowpt_L2CBHypo = muon.monitoring_muonLowpt_HI_L2CBHypo 
		HLTMuonMon.monitoring_muon_Support_L2CBHypo = muon.monitoring_muon_Support_HI_L2CBHypo


