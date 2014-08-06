# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def TrigIDJpsiMonitoringTool():
	if not 'rec' in dir():
		from RecExConfig.RecFlags import rec
		
	if rec.doInDet:  
		# MonTool
		from TrigIDJpsiMonitoring.TrigIDJpsiMonitoringConf import TrigJpsiMonTool
		from AthenaCommon.AppMgr import ToolSvc
		TrigJpsiMon = TrigJpsiMonTool(name		= 'TrigJpsiMon',
					      histoPathBase 	= "/Trigger/HLT/IDJpsiMon")
		
		ToolSvc += TrigJpsiMon
		list = [ "TrigJpsiMonTool/TrigJpsiMon" ]
		return list
